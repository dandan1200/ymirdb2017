/**
 * comp2017 - assignment 2
 * Daniel Chorev
 * dcho3009
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#include <ctype.h>

#include "ymirdb.h"

// We recommend that you design your program to be
// modular where each function performs a small task
//
// e.g.
//
// command_bye
// command_help
// ...
// entry_add
// entry_delete
// ...
//

//Function to free all entries in a current data base state - as passed in as a parameter.
void free_all_entries(entry * cur_state) {
    // Check if the database head has been initialised before clearing any further entries.
	if (cur_state->is_initialised) {
        //Free the first entry.
		free(cur_state->values);
		
		if(cur_state->next != NULL) {
            cur_state = cur_state->next;
            
		    free(cur_state->prev);
        } else{
            free(cur_state);
            return;
        }
		//Free any further entries until reaching the end.
		while (cur_state->next != NULL) {
			free(cur_state->values);

			cur_state = cur_state->next;
			
			free(cur_state->prev);
		}
		free(cur_state->values);
		
		free(cur_state);
	} else {
		free(cur_state);
	}
	return;
}

//Function for bye command
void command_bye(snapshot * snaps, entry * cur_state) {
	printf("bye\n");

	//Free the current state.
	free_all_entries(cur_state);

	//Free all snapshots and their entries.
	if (snaps->is_initialised) {
		while (snaps->next != NULL) {
			free_all_entries(snaps->entries);
			snaps = snaps->next;
			free(snaps->prev);
		}
		free_all_entries(snaps->entries);
		free(snaps);

	} else {
		free(snaps);
	}
	
	//Exit program
	exit(1);
}

void command_help() {
	printf("%s\n", HELP);
}

//Function for set command.
bool command_set(const char line[MAX_LINE], entry * db_head) {

	//Create duplicate of the input line to store original pointer.
	char * sep_line = calloc(strlen(line)+1,1);
	char * original_line_pointer = sep_line;
	strcpy(sep_line,line);

	//Extract key from the line.
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");

	//Initialise other variables.
	char * cur_elem;
	entry * update_existing_key = NULL;

	
	// Check if key exists.		
	if(isalpha(key[0])== false){
		printf("no such key, must start with an alphabetic character.\n\n");
		free(original_line_pointer);
		return false;
	}
	
	//Check if key already exists.
	int i = 0;
	update_existing_key = command_get(line, db_head, false,false);

	//Update existing key
	if (update_existing_key != NULL){
		
		//Key already exists:
		//Initialise variables.
		bool is_simple = true;
		element elements[MAX_LINE];

		//Read each element in input line until end (NULL)
		while ((cur_elem = strsep(&sep_line, " ")) != NULL) {
			//Check for Simple or general entry.
			if (isdigit(*cur_elem) || cur_elem[0] == '-') {
				elements[i].type = INTEGER;
				elements[i].value = atoi(cur_elem);
				i++;
				
			} else {
				//If it is a general element.

				//Create Get command for the element.
				char command1[MAX_LINE] = "GET ";
				strcat(command1,cur_elem);

				//Use get command to check if the entry key exists.
				entry * found_entry = command_get(command1,db_head,false,false);

				//If it does exist.
				if (found_entry != NULL ) {
					//Check if trying to add circular reference to itself.
					if (strcmp(found_entry->key, key) == 0) {
						printf("not permitted\n\n");
                        free(original_line_pointer);
						return false;
					}

					//Add element to array.
					is_simple = false;
					elements[i].type = ENTRY;
					elements[i].entry = found_entry;
					
					
				} else {
					//No such key exists.
					printf("no such key\n\n");
					free(original_line_pointer);
					return false;
				}
				i++;

			}
		}
		//Free old values.
		free(update_existing_key->values);

		update_existing_key->is_simple = is_simple;

		//Add values
		element * e = malloc(sizeof(element)*i);

		for (int j = 0; j < i; j++){
			(e+j)->type = elements[j].type;
			if (elements[j].type == INTEGER){
				(e+j)->value = elements[j].value;	
			} else {
				(e+j)->entry = elements[j].entry;	
			}
		}

		update_existing_key->values = e;
		update_existing_key->length = i;

		printf("ok\n\n");



	} else if (db_head->is_initialised == false){
		//Case if adding first element in database state
		//Similar as previous if block, no references allowed.

		db_head->is_simple = true;
		element elements[MAX_LINE];
		
		
		while ((cur_elem = strsep(&sep_line, " ")) != NULL) {
			if (isdigit(*cur_elem) || cur_elem[0] == '-') {
				elements[i].type = INTEGER;
				elements[i].value = atoi(cur_elem);
				i++;
				
			} else {

				printf("no such key\n\n");
                free(original_line_pointer);
				return false;
				
			}
		}
		

		//Same defaults and setting new values to entry
		db_head->forward_size = 0;
		db_head->length = i;
		element * e = malloc(sizeof(element)*i);

		for (int j = 0; j < i; j++){
			(e+j)->type = elements[j].type;
			if (elements[j].type == INTEGER){
				(e+j)->value = elements[j].value;	
			} else {
				(e+j)->entry = elements[j].entry;	
			}
			
		}

		strcpy(db_head->key,key);
		db_head->is_initialised = true;
		db_head->prev = NULL;
		db_head->next = NULL;
		db_head->values = e;

		printf("ok\n\n");

	} else {
		//Case for adding to existing initialised database
		//Find end of linked list to add new entry
		entry * last_key;
		last_key = db_head;
		while (last_key->next != NULL){
			last_key = last_key->next;
		}

		//New entry
		bool is_simple = true;
		element elements[MAX_LINE];

		while ((cur_elem = strsep(&sep_line, " ")) != NULL) {
			if (isdigit(*cur_elem) || cur_elem[0] == '-') {
				elements[i].type = INTEGER;
				elements[i].value = atoi(cur_elem);
				i++;
				
			} else {
				char command1[MAX_LINE] = "GET ";
				strcat(command1,cur_elem);
				
				entry * found_entry = command_get(command1,db_head,false,false);


				if (found_entry != NULL) {
					is_simple = false;
					elements[i].type = ENTRY;
					elements[i].entry = found_entry;
					
				} else {
					printf("no such key\n\n");
                    free(original_line_pointer);
					return false;
				}
				i++;
				
			}
		}

		entry * new_entry = malloc(sizeof(entry));
		strcpy(new_entry->key,key);
		new_entry->is_simple = is_simple;

		element * e = malloc(sizeof(element)*i);

		for (int j = 0; j < i; j++){
			(e+j)->type = elements[j].type;
			if (elements[j].type == INTEGER){
				(e+j)->value = elements[j].value;	
			} else {
				(e+j)->entry = elements[j].entry;	
			}
		}

		new_entry->values = e;
		new_entry->is_initialised = true;
		new_entry->length = i;
		new_entry->prev = last_key;
		new_entry->next = NULL;
		last_key->next = new_entry;
		
		printf("ok\n\n");
	}
    free(original_line_pointer);
	return true;
}

//Function for get command.
entry * command_get(const char line[MAX_LINE], entry * db_head, bool display, bool newline) {
	//Check there are entries in the database.
	if (db_head == NULL || db_head->is_initialised == false){
		if (display == true) {
			printf("no such key\n\n");
		}		
		return NULL;
	}
	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));
	
	//Extract key from line
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");
	
	if (key == NULL) {
		free(original_line_pointer);
		return NULL;
	}

	//Search for entry
	entry * entry_search;
	entry_search = db_head;
	while (strcmp(entry_search->key, key) != 0 && entry_search->next != NULL){
		entry_search = entry_search->next;
	}
	
	//If NULL - entry doesnt exist. Else, print entry contents.
	if (strcmp(entry_search->key, key) != 0){
		if (display == true) {
			printf("no such key\n\n");
		}
        free(original_line_pointer);
		return NULL;
	} else {
		//Print entry values.
		if (display == true) {
			printf("[");
		}
		for (int i = 0; i < entry_search->length; i++){
			if (display == true) {
				//Check for simple or general value type and print with format accordingly.
				if ((entry_search->values + i)->type == INTEGER){

					if (i == entry_search->length - 1){
						printf("%d", (entry_search->values + i)->value);
					} else {
						printf("%d ", (entry_search->values + i)->value);
					}

				} else {

					if (i == entry_search->length - 1){
						printf("%s", (entry_search->values + i)->entry->key);
					} else {
						printf("%s ", (entry_search->values + i)->entry->key);
					}

				}
			}
		}
		if (display == true && newline == true) {
			printf("]\n\n");
		}
        free(original_line_pointer);
		return entry_search;
	}
}

//Function for push command
bool command_push(const char line[MAX_LINE], entry * db_head) {
	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Skip to values to push
	strsep(&sep_line, " ");
	strsep(&sep_line, " ");
	
	char * cur_elem;
	entry * key_to_push = NULL;

	//Find the key to push to.
	key_to_push = command_get(line, db_head, false,false);

	//No key found
	if (key_to_push == NULL) {
		printf("no such key\n\n");
        free(original_line_pointer);
		return false;
	}

	//Initialise variables
	int i = 0;
	bool is_simple = key_to_push->is_simple;
	element elements[MAX_LINE];
	char * forwards[MAX_KEY];
	int num_forwards = 0;

	while ((cur_elem = strsep(&sep_line, " ")) != NULL) {
		//Read elements from line similarly to the get function.
		if (isdigit(*cur_elem) || cur_elem[0] == '-') {
			elements[i].type = INTEGER;
			elements[i].value = atoi(cur_elem);
			i++;
			
		} else {
			char command1[MAX_LINE] = "GET ";
			strcat(command1,cur_elem);
				
			entry * found_entry = command_get(command1,db_head,false,false);

			if (found_entry != NULL) {
				is_simple = false;
				elements[i].type = ENTRY;
				elements[i].entry = found_entry;

				*(forwards + num_forwards) = found_entry->key;
				num_forwards++;
			} else {
				printf("no such entry\n\n");
                free(original_line_pointer);
				return false;
			}
			i++;
		}
	}
	//Create new elements linked list
	element * e = malloc(sizeof(element)*(i + key_to_push->length));


	//First add new elements to new element linked list
	for (int j = 0; j < i; j++){
		(e+j)->type = elements[i-j-1].type;
		if (elements[i-j-1].type == INTEGER){
			(e+j)->value = elements[i-j-1].value;	
		} else {
			(e+j)->entry = elements[i-j-1].entry;	
		}
	}
	//Now add the existing elements to the end.
	for (int j = 0; j < key_to_push->length; j++){
		(e+j+i)->type = (key_to_push->values + j)->type;
		if ((key_to_push->values + j)->type == INTEGER){
			(e+j+i)->value = (key_to_push->values + j)->value;	
		} else {
			(e+j+i)->entry = (key_to_push->values + j)->entry;	
		}
		
	}
	
	key_to_push->is_simple = is_simple;
	free(key_to_push->values);
	key_to_push->values = e;
	key_to_push->length += i;

	printf("ok\n\n");
	free(original_line_pointer);
	return true;

}

//Function for append command
bool command_append(const char line[MAX_LINE], entry * db_head) {
	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Skip to values to be appended.
	strsep(&sep_line, " ");
	strsep(&sep_line, " ");
	
	char * cur_elem;
	entry * key_to_append = NULL;

	key_to_append = command_get(line, db_head, false,false);

	if (key_to_append == NULL) {
		printf("no such key\n\n");
        free(original_line_pointer);
		return false;
	}
	int i = 0;

	bool is_simple = key_to_append->is_simple;
	element elements[MAX_LINE];

	while ((cur_elem = strsep(&sep_line, " ")) != NULL) {
		if (isdigit(*cur_elem) || cur_elem[0] == '-') {
			elements[i].type = INTEGER;
			elements[i].value = atoi(cur_elem);
			i++;
			
		} else {
			char command1[MAX_LINE] = "GET ";
			strcat(command1,cur_elem);
				
			entry * found_entry = command_get(command1,db_head,false,false);

			if (found_entry != NULL) {
				is_simple = false;
				elements[i].type = ENTRY;
				elements[i].entry = found_entry;
					
			} else {
				printf("no such entry\n\n");
                free(original_line_pointer);
				return false;
			}
			i++;
		}
	}
	
	element * e = malloc(sizeof(element)*(i + key_to_append->length));
	
	//Add existing values first
	for (int j = 0; j < key_to_append->length; j++){
		(e+j)->type = (key_to_append->values + j)->type;
		if ((key_to_append->values + j)->type == INTEGER){
			(e+j)->value = (key_to_append->values + j)->value;	
		} else {
			(e+j)->entry = (key_to_append->values + j)->entry;	
		}
		
	}

	//Add new elements
	for (int j = 0; j < i; j++){
		(e+j+key_to_append->length)->type = elements[j].type;
		if (elements[j].type == INTEGER){
			(e+j+key_to_append->length)->value = elements[j].value;	
		} else {
			(e+j+key_to_append->length)->entry = elements[j].entry;	
		}
	}
	key_to_append->is_simple = is_simple;

	free(key_to_append->values);
	key_to_append->values = e;
	key_to_append->length += i;

	printf("ok\n\n");
    free(original_line_pointer);
	return true;

}

//Function for pick command
bool command_pick(const char line[MAX_LINE], entry * db_head) {
	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//skip to index to pick
	strsep(&sep_line, " ");
	strsep(&sep_line, " ");
	int print_index = atoi(strsep(&sep_line, " ")) - 1;

	//Find entry to pick from.
	entry * entry_to_print = command_get(line,db_head,false,false);

	//Check edge cases
	if (entry_to_print == NULL) {
		printf("no such key\n\n");
        free(original_line_pointer);
		return false;

	} else if (entry_to_print-> length <= print_index) {
		printf("index out of range\n\n");
        free(original_line_pointer);
		return false;

	} else {
		//Print value.
		if ((entry_to_print->values + print_index)->type == INTEGER){
			printf("%d\n\n", (entry_to_print->values + print_index)->value);	
		} else {
			printf("%s\n\n", (entry_to_print->values + print_index)->entry->key);
		}
		
        free(original_line_pointer);
		return true;
	}

}

//Function for pluck command
bool command_pluck(const char line[MAX_LINE], entry * db_head) {
	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Get index to pluck
	strsep(&sep_line, " ");
	strsep(&sep_line, " ");
	int print_index = atoi(strsep(&sep_line, " ")) - 1;

	//Find entry to pluck
	entry * entry_to_pluck = command_get(line,db_head,false,false);

	//Check edge cases
	if (entry_to_pluck == NULL) {
		printf("no such key\n\n");
        free(original_line_pointer);
		return false;

	} else if (entry_to_pluck-> length <= print_index) {
		printf("index out of range\n\n");
        free(original_line_pointer);
		return false;

	} else {
		//Print element
		if ((entry_to_pluck->values + print_index)->type == INTEGER){
			printf("%d\n\n", (entry_to_pluck->values + print_index)->value);	
		} else {
			printf("%s\n\n", (entry_to_pluck->values + print_index)->entry->key);
		}

		//Removing element from entry
		//Create new element 1 element smaller.
		element * e = malloc(sizeof(element)*(entry_to_pluck->length-1));
		int offset = 0;
		
		//Iterate through elements, appending if they are not the plucked value.
		for (int j = 0; j < entry_to_pluck->length; j++){
			if (j != print_index) {
				
				(e+j + offset)->type = (entry_to_pluck->values + j)->type;

				if ((entry_to_pluck->values + j)->type == INTEGER){
					(e+j + offset)->value = (entry_to_pluck->values + j)->value;	
				} else {
					(e+j + offset)->entry = (entry_to_pluck->values + j)->entry;	
				}
				
			} else {
				//Calculate offset from index to store values after the plucked element.
				offset = -1;
			}
		}

		entry_to_pluck->length -= 1;

		free(entry_to_pluck->values);
		entry_to_pluck->values = e;

        free(original_line_pointer);
		return true;
	}
}

//Function for pop command
bool command_pop(const char line[MAX_LINE], entry * db_head) {
	//Get entry to pop from.
	entry * entry_to_pop = command_get(line,db_head,false,false);

	//Check edge cases.
	if (entry_to_pop == NULL) {
		printf("no such key\n\n");
		return false;

	} else if (entry_to_pop->length == 0){
		printf("nil\n\n");
		return true;

	} else {
		//Print popped value
		if ((entry_to_pop->values)->type == INTEGER){
			printf("%d\n\n", (entry_to_pop->values)->value);	
		} else {
			printf("%s\n\n", (entry_to_pop->values)->entry->key);
		}
		
		element * e = malloc(sizeof(element)*(entry_to_pop->length-1));
		int offset = -1;
		
		//Iterate and copy over values with offset starting at -1 and starting at the second element after the popped element.
		for (int j = 1; j < entry_to_pop->length; j++){
			
			(e+j + offset)->type = (entry_to_pop->values + j)->type;

			if ((entry_to_pop->values + j)->type == INTEGER){
				(e+j + offset)->value = (entry_to_pop->values + j)->value;	
			} else {
				(e+j + offset)->entry = (entry_to_pop->values + j)->entry;	
			}

		}

		entry_to_pop->length -= 1;
		free(entry_to_pop->values);
		entry_to_pop->values = e;

		return true;
	}


}

//Function for min command
int command_min(const char line[MAX_LINE], entry * db_head, bool print){
	//Find entry to find min
	entry * search_entry = command_get(line,db_head,false,false);
	
	//Check if entry was found.
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;

	} else {
		//Set 1st value as min.
		//Look for 1st value recursively if 1st element is a reference key
		int min = 0;
		if (search_entry->values->type == INTEGER){
			min = search_entry->values->value;
		} else {
			//Create command and call min recursively.
			char command1[MAX_LINE] = "MIN ";
			strcat(command1,search_entry->values->entry->key);
			min = command_min(command1,db_head, false);
		}
		
		//Iterate through values and search for min - search recursively for references.
		for (int i = 1; i < search_entry->length; i++){
			if ((search_entry->values+i)->value < min){

				if ((search_entry->values +i)->type == INTEGER){
					min = (search_entry->values +i)->value;
				} else {
					char command1[MAX_LINE] = "MIN ";
					strcat(command1,(search_entry->values + i)->entry->key);
					min = command_min(command1,db_head,false);
				}
			}
		}
		if (print == true){
			printf("%d\n\n",min);
		}
		return min;
	}

}

//Function for max command
int command_max(const char line[MAX_LINE], entry * db_head, bool print){
	//Find entry for max.
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		//Look for 1st value recursively if 1st element is a reference key
		int max = 0;
		if (search_entry->values->type == INTEGER){
			max = search_entry->values->value;
		} else {
			char command1[MAX_LINE] = "MAX ";
			strcat(command1,search_entry->values->entry->key);
			max = command_max(command1,db_head, false);
		}
		
		//Iterate through values and search for min - search recursively for references.
		for (int i = 1; i < search_entry->length; i++){
			if ((search_entry->values+i)->value > max){

				if ((search_entry->values +i)->type == INTEGER){
					max = (search_entry->values +i)->value;
				} else {
					char command1[MAX_LINE] = "max ";
					strcat(command1,(search_entry->values + i)->entry->key);
					max = command_max(command1,db_head,false);
				}
			}
		}
		if (print == true){
			printf("%d\n\n",max);
		}
		return max;
	}

}

//Function for sum command
int command_sum(const char line[MAX_LINE], entry * db_head, bool print){
	//Find entry for sum
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int sum = 0;
		
		//Iterate through values adding to sum. Sum recursively for reference elements.
		for (int i = 0; i < search_entry->length; i++){
			if ((search_entry->values +i)->type == INTEGER){
				sum += (search_entry->values + i)->value;
			} else {
				char command1[MAX_LINE] = "sum ";
				strcat(command1,(search_entry->values + i)->entry->key);
				sum += command_sum(command1,db_head,false);
			}
		}
		if (print == true) {
			printf("%d\n\n",sum);
		}
		return sum;
	}

}

//Function for len command
int command_len(const char line[MAX_LINE], entry * db_head, bool print) {
	//Find entry for len
	entry * search_entry = command_get(line,db_head,false,false);

	int len = 0;
	if (search_entry == NULL) {
		if (print == true) {
			printf("no such key\n\n");
		}
		return 0;
	} else {
		//Iterate through values, summing len recursively for reference keys.
		for (int i = 0; i < search_entry->length; i++) {
			if ((search_entry->values +i)->type == INTEGER){
				len += 1;
			} else {
				char command1[MAX_LINE] = "LEN ";
				strcat(command1,(search_entry->values + i)->entry->key);
				len += command_len(command1,db_head,false);
			}
		}
		if (print == true) {
			printf("%d\n\n",len);
		}
		return len;
	}
}

//Function for del command
entry * command_del(const char line[MAX_LINE], entry * db_head, bool print) {
	//Find entry to delete
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		if (print == true){
			printf("no such key\n\n");
		}
		return db_head;
	} else {

		if (command_backward(line, db_head,false)){
			if (print == true) {
				printf("not permitted\n\n");
			}
			return db_head;
		}

		// If trying to delete head.
		if (search_entry->prev == NULL){
			if (search_entry->next != NULL) {
				//Set next entry as head.
				search_entry->next->prev = NULL;
				db_head = search_entry->next;

				//Free current values
				if (search_entry->length > 0){
					free(search_entry->values);
				}
				
				//Free entry to be deleted.
				free(search_entry);
			} else {
				db_head->is_initialised = false;
				if (search_entry->length > 0){
					free(search_entry->values);
				}
			}
		
		//Deleting entry in middle
		} else {
			if(search_entry->next == NULL){
				search_entry->prev->next = NULL;
				free(search_entry->values);
				free(search_entry);
			} else {
				search_entry->prev->next = search_entry->next;
				search_entry->next->prev = search_entry->prev;
				free(search_entry->values);
				free(search_entry);
			}
		}
		if (print == true){
			printf("ok\n\n");
		}
		return db_head;
	}
}

//Function for list keys command
bool command_list_keys(entry * db_head){
	if (db_head->is_initialised == false) {
		printf("no keys\n\n");
		return false;
	} else {
		//Print keys for each entry until end
		entry * search_entry = db_head;
		while (search_entry->next != NULL) {
			search_entry = search_entry->next;
		}
		while (search_entry != NULL) {
			printf("%s\n",search_entry->key);
			search_entry = search_entry->prev;
		}
		printf("\n");
	}
	return true;
}

//Function for list entries command
bool command_list_entries(entry * db_head){
	if (db_head->is_initialised == false) {
		printf("no entries\n\n");
		return false;
	} else {
		
		//Find end of entries linked list.
		entry * search_entry = db_head;
		while (search_entry->next != NULL) {
			search_entry = search_entry->next;
		}
		
		//Go backwards from end to beginning printing each value from each entry.
		while (search_entry != NULL) {
			printf("%s ",search_entry->key);
			printf("[");
			for (int i = 0; i < search_entry->length; i++){	
				
				if ((search_entry->values + i)->type == INTEGER){
					if (i == search_entry->length - 1){
						printf("%d", (search_entry->values + i)->value);
					} else {
						printf("%d ", (search_entry->values + i)->value);
					}
				} else {
					if (i == search_entry->length - 1){
						printf("%s", (search_entry->values + i)->entry->key);
					} else {
						printf("%s ", (search_entry->values + i)->entry->key);
					}
				}

			}
			printf("]\n");
			search_entry = search_entry->prev;
		}
		printf("\n");
	}
	return true;
}

//Function for reverse command
bool command_rev(const char line[MAX_LINE], entry * db_head){
	
	//Get entry to reverse
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		//Add values to list
		int * list = malloc(sizeof(int)*search_entry->length);
		for (int i = 0; i < search_entry->length; i++){
			*(list + i) = (search_entry->values + i)->value;
		}

		//Add values from list to entry backwards
		for (int i = 0; i < search_entry->length; i++){
			(search_entry->values + i)->value = *(list + search_entry->length-i-1);
		}

		free(list);
		printf("ok\n\n");
		return true;
	}
	return false;
}

//Function for uniq command
bool command_uniq(const char line[MAX_LINE], entry * db_head){
	//Get entry to ammend
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {

		int shortened = 0;

		//Iterate through values checking adjacent values for equality.
		//Iterate through values after finding adjacent equal values and shuffle them back one space.
		//Continue from position of duplicate and continue comparisons.
		for (int i = 0; i < search_entry->length - 1 - shortened ; i++) {
			if ((search_entry->values + i)->value == (search_entry->values + i + 1)->value) {
				for (int j = i; j < search_entry->length -1; j++) {
					(search_entry->values + j)->value = (search_entry->values + j + 1)->value;
				}
				shortened += 1;
				i--;
			}
		}

		//Change length
		search_entry->length -= shortened;

		printf("ok\n\n");
		return true;
	}
	return false;
}

//Comparison function for sort
int cmpfunc(const void * a, const void * b){
	return (((element*) a)->value - ((element*)b)->value);
}

//Function for sort command
bool command_sort(const char line[MAX_LINE], entry * db_head){
	//Find entry to sort
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		qsort(search_entry->values,search_entry->length,sizeof(element),cmpfunc);
		printf("ok\n\n");
		return true;
	}
}

//Function for type command
bool command_type(const char line[MAX_LINE], entry * db_head) {
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		if (command_forward(line,db_head,false)){
			printf("general\n\n");
		} else {
			printf("simple\n\n");
		}
		return true;
	}
}

//Function to copy database to new memory
entry * copy_database(entry * old_curr_entry){
	entry * new_db_head = malloc(sizeof(entry));
	entry * new_current_entry = new_db_head;
	entry * last_entry = NULL;
	element * new_vals;
	
	if (old_curr_entry->is_initialised == false) {
		new_db_head->is_initialised = false;
		return new_db_head;
	}

	copy_reference * cr = NULL;
	int num_ref_copies = 0;

	while (old_curr_entry != NULL) {
		
		new_current_entry->backward_max = old_curr_entry->backward_max;
		new_current_entry->backward_size = old_curr_entry->backward_size;
		new_current_entry->forward_max = old_curr_entry->forward_max;
		new_current_entry->forward_size = old_curr_entry->forward_size;
		new_current_entry->is_initialised = old_curr_entry->is_initialised;
		new_current_entry->is_simple = old_curr_entry->is_simple;
		strncpy(new_current_entry->key,old_curr_entry->key,MAX_KEY);
		new_current_entry->length = old_curr_entry->length;
		new_current_entry->next = NULL;
		new_current_entry->prev = NULL;

		//Copy over values to new element linked list
		if (new_current_entry->length > 0){
			new_vals = malloc((new_current_entry->length)*sizeof(element));
		
			for (int i = 0; i < new_current_entry->length; i++) {
				
				(new_vals+i)->type = (old_curr_entry->values+i)->type;

				if ((new_vals+i)->type == INTEGER) {
					(new_vals+i)->value = (old_curr_entry->values+i)->value;
				} else {
					//Keep record of all references to copy entry addresses later.
					if (cr == NULL) {
						cr = malloc(sizeof(copy_reference));
						strncpy(cr->key_entry,old_curr_entry->key, MAX_KEY);
						strncpy(cr->key_val,(old_curr_entry->values+i)->entry->key, MAX_KEY);
						cr->pos = i;
						num_ref_copies = 1;
					} else {
						num_ref_copies++;
						cr = realloc(cr,sizeof(copy_reference)*num_ref_copies);
						strncpy((cr + num_ref_copies-1)->key_entry,old_curr_entry->key, MAX_KEY);
						strncpy((cr + num_ref_copies-1)->key_val,(old_curr_entry->values+i)->entry->key, MAX_KEY);
						(cr + num_ref_copies-1)->pos = i;

					}
				}

			}
			new_current_entry->values = new_vals;
		}

		//Copy next and prev addresses
		if (old_curr_entry->next == NULL){
			new_current_entry->prev = last_entry;
			break;
		} else {
			//Allocate next entry memory
			new_current_entry->next = malloc(sizeof(entry));
			new_current_entry->prev = last_entry;
			
			last_entry = new_current_entry;
			new_current_entry = new_current_entry->next;
		}
		old_curr_entry = old_curr_entry->next;
	}
	
	//Copy references
	for (int i = 0; i < num_ref_copies; i++) {
		char command1[MAX_LINE] = "GET ";
		strcat(command1,(cr + i)->key_entry);

		char command2[MAX_LINE] = "GET ";
		strcat(command2,(cr + i)->key_val);

		(command_get(command1,new_db_head,false,false)->values + (cr + i)->pos)->entry = command_get(command2,new_db_head,false,false);
	}
	free(cr);

	return new_db_head;
}

//Function for snapshot commmand
bool command_snapshot(entry * db_head, snapshot * snapshot_head){
	int snap_counter = snapshot_head->id;
	//No snapshots yet
	if (snapshot_head->is_initialised == false){
		
		snapshot_head->id = snap_counter;
		snapshot_head->prev = NULL;
		snapshot_head->next = NULL;
		snapshot_head->is_initialised = true;
		snapshot_head->entries = copy_database(db_head);
		printf("saved as snapshot 1\n\n");

	} else {
		//Find last snapshot
		snapshot * last_snap;
		last_snap = snapshot_head;
		while (last_snap->next != NULL) {
			last_snap = last_snap->next;
			snap_counter++;
		}

		//Create new snapshot with database copy
		snapshot * next_snap = malloc(sizeof(snapshot));
		next_snap->id = snap_counter + 1;
		next_snap->prev = last_snap;
		next_snap->is_initialised = true;
		next_snap->entries = copy_database(db_head);
		last_snap->next = next_snap;
		next_snap->next = NULL;
		printf("saved as snapshot %d\n\n", next_snap->id);
	}
	return true;
} 

//Function for checkout command
entry * command_checkout(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head, bool print) {
	snapshot * snap_search = snapshot_head;
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Extract snapshot key from line
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");

	if (key == NULL || snap_search->is_initialised == false) {
		if (print == true){
			printf("no such snapshot\n\n");
		}
		free(original_line_pointer);
		return db_head;
	}

	//Find snapshot from snapshot key
	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			//Copy database to new database state
			entry * new_db_head = copy_database(snap_search->entries);
			
			//Free current state
			
			free_all_entries(db_head);
			if (print == true){
				printf("ok\n\n");
			}
			free(original_line_pointer);
			return new_db_head;
		}
		snap_search = snap_search->next;

	}
	if (print == true){
		printf("no such snapshot\n\n");
	}
    free(original_line_pointer);
	return db_head;

}

//Function for drop command
snapshot * command_drop(const char line[MAX_LINE], snapshot * snapshot_head, bool print) {
	snapshot * snap_search = snapshot_head;
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Extract snapshot key
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");

	if (key == NULL || snap_search->is_initialised == false) {
		printf("no such snapshot\n\n");
        free(original_line_pointer);
		return snapshot_head;
	}

	entry * entry_iterator = NULL;
	
	//Find snapshot from snapshot key
	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			
			//Free all entries from snapshot
			entry_iterator = snap_search->entries;
			free_all_entries(entry_iterator);

			if (print == true){
				printf("ok\n\n");
			}
			
			//Adjust next and prev pointers based on deleted snapshot
			snapshot * new_snap_head;
            free(original_line_pointer);
			if (snap_search->prev == NULL && snap_search->next == NULL) {
				new_snap_head = malloc(sizeof(snapshot));
				new_snap_head->is_initialised = false;
				
				free(snap_search);
				return new_snap_head;
			} else if (snap_search->prev == NULL && snap_search->next != NULL) {
				new_snap_head = snap_search->next;
				
				free(snap_search);
				return new_snap_head;
			} else if (snap_search-> prev != NULL && snap_search->next == NULL) {
				snap_search->prev->next = NULL;
				
				free(snap_search);
				return snapshot_head;
			} else {
				snap_search->prev->next = snap_search->next;
				snap_search->next->prev = snap_search->prev;
				
				free(snap_search);
				return snapshot_head;
			}

		}
		snap_search = snap_search->next;

	}
	printf("no such snapshot\n\n");
    free(original_line_pointer);
	return snapshot_head;

}

//Function for list snapshots command
bool command_list_snapshots(snapshot * snapshot_head){
	if (snapshot_head->is_initialised == false){
		printf("no snapshots\n\n");
		return false;
	} else {
		//Find end of linked list
		snapshot * search_snap = snapshot_head;
		while (search_snap->next != NULL){
			search_snap = search_snap->next;
		}
		//Print snapshot keys backwards
		while (search_snap != NULL){
			printf("%d\n", search_snap->id);
			search_snap = search_snap->prev;
		}
		printf("\n");
		return true;
	}

}

//Function for rollback command
bool command_rollback(const char line[MAX_LINE], snapshot * snapshot_head) {
	snapshot * snap_search = snapshot_head;
	
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Extract snapshot key from command
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");

	if (key == NULL || snap_search->is_initialised == false) {
		printf("no such snapshot\n\n");
        free(original_line_pointer);
		return false;
	}

	//Find snapshot from key
	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			snap_search = snap_search->next;

			//Free all snapshots and values onwards
			if (snap_search != NULL){
				while (snap_search->next != NULL) {
					free_all_entries(snap_search->entries);
					snap_search = snap_search->next;
					free(snap_search->prev);
				}
				free_all_entries(snap_search->entries);
				snap_search->prev->next = NULL;
				free(snap_search);
				
			}
			printf("ok\n\n");
            free(original_line_pointer);
			return true;
		}
		snap_search = snap_search->next;

	}
	printf("no such snapshot\n\n");
    free(original_line_pointer);
	return false;
}

//Function for purge command
entry * command_purge(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head) {
	snapshot * snap_search = snapshot_head;
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Extract key
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");
	if (snap_search->is_initialised == true){
		while (snap_search != NULL) {
			if (command_backward(line,snap_search->entries,false)){
				printf("not permitted\n\n");
				free(original_line_pointer);
				return db_head;
			}
			
			snap_search = snap_search->next;
		}
		snap_search = snapshot_head;
	}
	

	//Delete key from current state
	char command1[MAX_LINE] = "DEL ";
	strcat(command1,key);
	entry * del_return = command_del(command1,db_head,false);

	//Set new database head pointer for current state if it is changed by delete command
	if (del_return != NULL) {
		db_head = del_return;
	}
	
	//Iterate through each snapshot
	while(snap_search != NULL && snap_search->is_initialised == true ) {
		//Delete key from each snapshot
		char command2[MAX_LINE] = "DEL ";
		strcat(command2,key);
		del_return = command_del(command2,snap_search->entries,false);

		//Adjust database head for snapshot if changed by delete
		if (del_return != NULL){
			snap_search->entries = del_return;
		}
		snap_search = snap_search->next;
	}
	
	printf("ok\n\n");
	free(original_line_pointer);
	return db_head;
}

//Comparison function for reference list sorting
int sortstring(const void* a, const void* b) {
    const char * pa = (const char *) a;
    const char * pb = (const char *) b;
    return strcmp(pa,pb);
}

//Function for forward command
bool command_forward(const char line[MAX_LINE], entry * db_head, bool print){
    //Check if entry given exists
	if (command_get(line,db_head,false,false) == NULL){
		if (print == true) {
			printf("no such key\n\n");
		}
        return false;
	}
	
	//Get linked list of forward references unordered
	reference * f = calculate_forwards(line, db_head, NULL);
    if (f == NULL) {
		if (print == true) {
			printf("nil\n\n");
		}
        
        return false;
    }

	//Store head of linked list
    reference * head = f;
    int num_refs = 0;

	//Count number of references
    while (f != NULL) {
        num_refs++;
        f = f->next;
    }
	//Reset to head of linked list
    f = head;

	//Move keys to char array in heap memory
    char * keys = malloc(num_refs*MAX_KEY);
    for (int i = 0; i < num_refs; i++){
        strncpy(keys + i*MAX_KEY,f->key,MAX_KEY);
        
		//Free linked list memory after key is added to char array
        if (f->next == NULL) {
            free(f);
            break;
        } else {
            f = f->next;
            free(f->prev);
        }
        
    }

	//Sort keys using custom string comparator
    qsort(keys,num_refs,MAX_KEY,sortstring);

	//Print out keys comma separated
    if (print == true) {
		for (int i = 0; i < num_refs-1; i++){
        	printf("%s, ", keys + i*MAX_KEY);
    	}
		printf("%s\n\n",keys + MAX_KEY*(num_refs - 1));
	}
    
    free(keys);
    return true;
}

//Function to recursively calculate forwards
reference * calculate_forwards(const char line[MAX_LINE], entry * db_head, reference * f){
	//Find entry for forwards
	entry * search_entry = command_get(line,db_head,false,false);

    reference * f_head = NULL;
    
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return NULL;
	} else {
		//For each entry in database state
		for (int i = 0; i < search_entry->length; i++){
			//Check for reference
            if ((search_entry->values + i)->type == ENTRY) {
                if (f == NULL) {
					//Initialise forwards list
                    f = malloc(sizeof(reference));
                    f->prev = NULL;
                    f->next = NULL;
                    
					//Copy key to forwards list
                    strncpy(f->key,(search_entry->values+i)->entry->key,MAX_KEY);
                    f_head = f;
                    
					//Recursively calculate forward references of the forward reference
                    char command[MAX_LINE] = "FORWARD ";
	                strcat(command,f->key);
                    calculate_forwards(command,db_head,f);

                } else {
					//Extend linked list
                    f->next = malloc(sizeof(reference));
                    f->next->prev = f;
                    
                    f = f->next;
                    f->next = NULL;

					//Copy key to list of references
                    strncpy(f->key,(search_entry->values+i)->entry->key,MAX_KEY);

					//Recursively calculate forward references of the forward reference
                    char command[MAX_LINE] = "FORWARD ";
	                strcat(command,f->key);
                    calculate_forwards(command,db_head,f);
                }
            }
        }
		//Return head to linked list of references
		return f_head;
	}
}

//Function for backward command
bool command_backward(const char line[MAX_LINE], entry * db_head, bool print){
	//Check if key exists
	if (command_get(line,db_head,false,false) == NULL){
		if (print == true) {
			printf("no such key\n\n");
		}
        return false;
	}
	//Copy command line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));

	//Extract snapshot key from line
	strsep(&sep_line, " ");
	char * key = strsep(&sep_line, " ");
	
	//Initialise lists
	reference * backwards = NULL;
	reference * head = NULL;
	entry * search_entry = db_head;

	//Search each entry in database state
	while (search_entry != NULL) {
		
		//Calculate forward references for each entry
		char command[MAX_LINE] = "FORWARD ";
	    strcat(command,search_entry->key);
		reference * f = NULL;
		f = calculate_forwards(command,db_head,f);

		//Check all references calculated
		while (f != NULL) {

			//Search for input key in list of forward references of each entry
			if (strcmp(f->key,key) == 0){
				
				//If found, add key of entry to backward reference list
				if (backwards == NULL) {
					backwards = malloc(sizeof(reference));
					backwards->prev = NULL;
                	backwards->next = NULL;
                	strncpy(backwards->key,search_entry->key,MAX_KEY);

					head = backwards;

				} else {
					backwards->next = malloc(sizeof(reference));
					backwards->next->prev = backwards;
						
					backwards = backwards->next;
					backwards->next = NULL;
					strncpy(backwards->key,search_entry->key,MAX_KEY);
				}
			}
			//Free forward reference linked list as we go
			if (f->next == NULL) {
            	free(f);
            	break;
        	} else {
            	f = f->next;
            	free(f->prev);
       		}
		}
		search_entry = search_entry->next;
	}

	//No backwards references found
	if (backwards == NULL) {
		if (print == true) {
			printf("nil\n\n");
		}
		free(original_line_pointer);
        return false;
    }

	//Store head of backwards ref linked list
    backwards = head;
    int num_refs = 0;

	//Count number of backward references
    while (backwards != NULL) {
        num_refs++;
        backwards = backwards->next;
    }

	//Reset to head of linked list
    backwards = head;

	//Copy over reference keys to char array in heap memory
    char * keys = malloc(num_refs*MAX_KEY);
    for (int i = 0; i < num_refs; i++){
        strncpy(keys + i*MAX_KEY,backwards->key,MAX_KEY);

		//Free backward linked list as we go
        if (backwards->next == NULL) {
            free(backwards);
            break;
        } else {
            backwards = backwards->next;
            free(backwards->prev);
        }
        
    }

	//Sort list of keys with custom string comparator
    qsort(keys,num_refs,MAX_KEY,sortstring);

	//Print backward references in order comma separated
    if (print == true) {
		for (int i = 0; i < num_refs-1; i++){
        	printf("%s, ", keys + i*MAX_KEY);
    	}
    	printf("%s\n\n",keys + MAX_KEY*(num_refs - 1));
	}
    free(keys);
	free(original_line_pointer);
    return true;

}

//Validate line and perform commands
struct whole_db validate_line(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head){
	
	//Store whole database struct fill in passed through data
	struct whole_db db_return;
	db_return.current_db_state = db_head;
	db_return.snapshot_head = snapshot_head;

	//Duplicate input line
	char * sep_line = calloc(strlen(line)+1, 1);
	char * original_line_pointer = sep_line;
	strncpy(sep_line,line,strlen(line));
	
	//Extract command words
	char *cmd1, *cmd2;
	cmd1 = strsep(&sep_line, " ");
	cmd2 = strsep(&sep_line, " ");


	//Find command
	if (strcasecmp(cmd1,"HELP") == 0){
		
		printf("%s",HELP);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"BYE") == 0) {

		free(original_line_pointer);
		command_bye(snapshot_head, db_head);
        
		return db_return;

	} else if (strcasecmp(cmd1,"GET") == 0) {
		//GET
		
		free(original_line_pointer);
		command_get(line,db_head,true,true);
        
		return db_return;

	} else if (strcasecmp(cmd1,"DEL") == 0) {
		//DEL
		
		db_return.current_db_state = command_del(line,db_head,true);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"PURGE") == 0) {
		//PURGE

		db_return.current_db_state = command_purge(line,db_head,snapshot_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"SET") == 0) {
		//SET

		command_set(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"PUSH") == 0) {
		//PUSH

		command_push(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"APPEND") == 0) {
		//APPEND

		command_append(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"PICK") == 0) {
		//PICK

		command_pick(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"PLUCK") == 0) {
		//PLUCK

		command_pluck(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"POP") == 0) {
		//POP

		command_pop(line,db_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"DROP") == 0) {
		//DROP

		db_return.snapshot_head = command_drop(line,snapshot_head,true);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"ROLLBACK") == 0) {
		//ROLLBACK
		
		db_return.current_db_state = command_checkout(line,db_head,snapshot_head,false);
		command_rollback(line,snapshot_head);
        free(original_line_pointer);

		return db_return;

	} else if (strcasecmp(cmd1,"CHECKOUT") == 0) {
		//CHECKOUT
		
		db_return.current_db_state = command_checkout(line,db_head,snapshot_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"SNAPSHOT") == 0) {
		//SNAPSHOT
		
		command_snapshot(db_head, snapshot_head);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"MIN") == 0) {
		//MIN
		
		command_min(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"MAX") == 0) {
		//MAX
		
		command_max(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"SUM") == 0) {
		//SUM
		
		command_sum(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"LEN") == 0) {
		//LEN
		
		command_len(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"REV") == 0) {
		//REV
		
		command_rev(line,db_head);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"UNIQ") == 0) {
		//UNIQ
		
		command_uniq(line,db_head);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"SORT") == 0) {
		//SORT
		
		command_sort(line,db_head);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"FORWARD") == 0) {
		//FORWARD
		
		command_forward(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"BACKWARD") == 0) {
		//BACKWARD
		
		command_backward(line,db_head,true);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"TYPE") == 0) {
		//TYPE
		
		command_type(line,db_head);
        free(original_line_pointer);
		
		return db_return;
	
	} else if (strcasecmp(cmd1,"LIST") == 0 && cmd2 != NULL){
		
		if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"KEYS") == 0) {
			//LIST KEYS
			
			command_list_keys(db_head);
			free(original_line_pointer);
			
			return db_return;
		
		} else if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"ENTRIES") == 0) {
			//LIST ENTRIES
			
			command_list_entries(db_head);
			free(original_line_pointer);
			
			return db_return;
		
		} else if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"SNAPSHOTS") == 0) {
			//LIST SNAPSHOTS
			
			command_list_snapshots(snapshot_head);
			free(original_line_pointer);
			
			return db_return;
		
		}
	} else {
		printf("Invalid command\n\n");

	}

    free(original_line_pointer);
	return db_return;
}

int main(void) {

	entry * db_head = malloc(sizeof(entry));
	db_head->is_initialised = false;

	snapshot * snapshot_head = malloc(sizeof(snapshot));
	snapshot_head->is_initialised = false;
	snapshot_head->id = 1;

	struct whole_db db;
	db.current_db_state = db_head;
	db.snapshot_head = snapshot_head;

	char line[MAX_LINE];

	while (true) {
		printf("> ");

		if (NULL == fgets(line, MAX_LINE, stdin)) {
			printf("\n");
			command_bye(db.snapshot_head,db.current_db_state);
			return 0;
		}
		//Get input
		line[strlen(line)-1] = '\0';
		//Check command
		db = validate_line(line, db.current_db_state, db.snapshot_head);

  	}
	return 0;
}