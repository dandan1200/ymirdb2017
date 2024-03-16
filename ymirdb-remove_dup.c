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

#include "ymirdb-remove_dup.h"

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

void command_bye() {
	printf("bye\n");
	exit(1);
}

void command_help() {
	printf("%s\n", HELP);
}

bool command_set(const char line[MAX_LINE], entry * db_head) {
	
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	

	char * cur_elem;
	entry * update_existing_key = NULL;

	// printf("%s\n", key);
	// printf("%s\n", cur_elem);
	// Check if key exists.		

	if(isalpha(key[0])== false){
		printf("no such key, must start with an alphabetic character.\n\n");
		return false;
	}
	
	int i = 0;

	update_existing_key = command_get(line, db_head, false,false);

	//Update existing key
	if (update_existing_key != NULL){

		// printf("%s\n", update_existing_key->key);
		
		bool is_simple = true;
		element elements[MAX_LINE];
		char * forwards[MAX_KEY];
		int num_forwards = 0;
		while ((cur_elem = next_sep(line,sep_index)) != NULL) {
			sep_index += strlen(cur_elem)+1;
			if (isdigit(*cur_elem)) {
				elements[i].type = INTEGER;
				elements[i].value = atoi(cur_elem);
				i++;
				
			} else {
				char command1[MAX_LINE] = "GET ";
				strcat(command1,cur_elem);
				
				entry * found_entry = command_get(command1,db_head,false,false);

				

				if (found_entry != NULL ) {
					if (strcmp(found_entry->key, key) == 0) {
						printf("not permitted\n\n");
						free(key);
						free(cur_elem);
						return false;

					}
					is_simple = false;
					elements[i].type = ENTRY;
					elements[i].entry = found_entry;

					*(forwards + num_forwards) = found_entry->key;
					num_forwards++;
					
				} else {
					printf("no such key\n\n");
					free(key);
					free(cur_elem);
					return false;
				}
				i++;

			}
			free(cur_elem);
		}
		
		free(cur_elem);
		free(update_existing_key->values);

		db_head->forward_size = num_forwards;
		*db_head->forward = malloc(MAX_KEY*num_forwards);

		memcpy(db_head->forward, forwards,MAX_KEY*num_forwards);

		update_existing_key->is_simple = is_simple;

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
		free(key);
		printf("ok\n\n");



	} else if (db_head->is_initialised == false){
		
		db_head->is_simple = true;
		element elements[MAX_LINE];
		
		
		while ((cur_elem = next_sep(line,sep_index)) != NULL) {
			sep_index += strlen(cur_elem)+1;
			if (isdigit(*cur_elem)) {
				elements[i].type = INTEGER;
				elements[i].value = atoi(cur_elem);
				i++;
				
			} else {
				char command1[MAX_LINE] = "GET ";
				strcat(command1,cur_elem);
				
				entry * found_entry = command_get(command1,db_head,false,false);
				if (found_entry != NULL) {
					db_head->is_simple = false;
					elements[i].type = ENTRY;
					elements[i].entry = found_entry;
					
					

				} else {
					printf("no such key\n\n");
					free(key);
					free(cur_elem);
					return false;
				}
				i++;
			}
			free(cur_elem);
		}
		free(cur_elem);
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
		// for (int j = 0; j < i; j++){
		// 	printf("%d\n",(db_head->values + j*sizeof(element))->value);
		// }
		printf("ok\n\n");
		free(key);

	} else {
		//Find end of linked list to add new entry
		entry * last_key;
		last_key = db_head;
		while (last_key->next != NULL){
			last_key = last_key->next;
		}

		//New entry
		

		bool is_simple = true;
		element elements[MAX_LINE];
		char * forwards[MAX_KEY];
		int num_forwards = 0;

		while ((cur_elem = next_sep(line,sep_index)) != 0) {
			sep_index += strlen(cur_elem)+1;
			if (isdigit(*cur_elem)) {
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
					free(key);
					free(cur_elem);
					printf("no such key\n\n");
					return false;
				}
				i++;
				
			}
			free(cur_elem);
		}
		free(cur_elem);
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

		new_entry->forward_size = num_forwards;
		*new_entry->forward = malloc(MAX_KEY*num_forwards);

		memcpy(new_entry->forward, forwards,MAX_KEY*num_forwards);

		new_entry->values = e;
		new_entry->is_initialised = true;
		new_entry->length = i;
		new_entry->prev = last_key;
		new_entry->next = NULL;
		last_key->next = new_entry;
		
		printf("ok\n\n");
		free(key);
	}
	return true;
}

entry * command_get(const char line[MAX_LINE], entry * db_head, bool display, bool newline) {
	
	if (db_head == NULL || db_head->is_initialised == false){
		if (display == true) {
			printf("no such key\n\n");
		}		
		return NULL;
	}
	
	// char * sep_line = malloc(strlen(line));
	// strncpy(sep_line,line,strlen(line));
	// strsep(&sep_line, " ");
	// char * key = strsep(&sep_line, " ");

	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;

	if (key == NULL) {
		
		return NULL;
	}


	entry * entry_search;
	entry_search = db_head;
	//printf("%s\n",key);
	while (strcmp(entry_search->key, key) != 0 && entry_search->next != NULL){
		entry_search = entry_search->next;
		
	}
	
	if (strcmp(entry_search->key, key) != 0){
		if (display == true) {
			printf("no such key\n\n");
		}
		free(key);
		return NULL;
	} else {
		if (display == true) {
			printf("[");
		}
		for (int i = 0; i < entry_search->length; i++){
			if (display == true) {
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
		free(key);
		return entry_search;
	}


	
}

bool command_push(const char line[MAX_LINE], entry * db_head) {
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	free(key);
	
	
	char * cur_elem;
	entry * key_to_push = NULL;

	key_to_push = command_get(line, db_head, false,false);

	if (key_to_push == NULL) {
		printf("no such key\n\n");
		return false;
	}
	int i = 0;
	bool is_simple = key_to_push->is_simple;
	element elements[MAX_LINE];
	char * forwards[MAX_KEY];
	int num_forwards = 0;

	while ((cur_elem = next_sep(line,sep_index)) != NULL) {
		sep_index += strlen(cur_elem)+1;
		if (isdigit(*cur_elem)) {
			
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
				free(cur_elem);
				return false;
			}
			
			i++;
		}
		free(cur_elem);
	}
	free(cur_elem);
	
	element * e = malloc(sizeof(element)*(i + key_to_push->length));

	for (int j = 0; j < i; j++){
		(e+j)->type = elements[i-j-1].type;
		if (elements[i-j-1].type == INTEGER){
			(e+j)->value = elements[i-j-1].value;	
		} else {
			(e+j)->entry = elements[i-j-1].entry;	
		}
	}
	for (int j = 0; j < key_to_push->length; j++){
		(e+j+i)->type = (key_to_push->values + j)->type;
		if ((key_to_push->values + j)->type == INTEGER){
			(e+j+i)->value = (key_to_push->values + j)->value;	
		} else {
			(e+j+i)->entry = (key_to_push->values + j)->entry;	
		}
		
	}
	key_to_push->forward_size = num_forwards;
	if (num_forwards > 0) {
		*key_to_push->forward = malloc(MAX_KEY*num_forwards);
		memcpy(key_to_push->forward, forwards,MAX_KEY*num_forwards);
	}

	

	key_to_push->is_simple = is_simple;
	free(key_to_push->values);
	key_to_push->values = e;
	key_to_push->length += i;

	printf("ok\n\n");
	return true;

}

bool command_append(const char line[MAX_LINE], entry * db_head) {
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	free(key);

	char * cur_elem;
	entry * key_to_append = NULL;

	key_to_append = command_get(line, db_head, false,false);

	if (key_to_append == NULL) {
		printf("no such key\n\n");
		return false;
	}
	int i = 0;

	bool is_simple = key_to_append->is_simple;
	element elements[MAX_LINE];

	while ((cur_elem = next_sep(line,sep_index)) != NULL) {
		sep_index += strlen(cur_elem)+1;
		if (isdigit(*cur_elem)) {
			
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
				free(cur_elem);
				printf("no such entry\n\n");
				return false;
			}
			i++;
		}
		free(cur_elem);
	}
	
	element * e = malloc(sizeof(element)*(i + key_to_append->length));
	
	for (int j = 0; j < key_to_append->length; j++){
		(e+j)->type = (key_to_append->values + j)->type;
		if ((key_to_append->values + j)->type == INTEGER){
			(e+j)->value = (key_to_append->values + j)->value;	
		} else {
			(e+j)->entry = (key_to_append->values + j)->entry;	
		}
		
	}

	for (int j = 0; j < i; j++){
		(e+j+key_to_append->length)->type = elements[j].type;
		if (elements[j].type == INTEGER){
			(e+j+key_to_append->length)->value = elements[j].value;	
		} else {
			(e+j+key_to_append->length)->entry = elements[j].entry;	
		}
	}
	key_to_append->is_simple = is_simple;

	// for (int j = 0; j < key_to_append->length; j++){
	// 		(e+j)->type = INTEGER;
	// 		(e+j)->value = (key_to_append->values + j)->value;
	// }

	// for (int j = 0; j < i; j++){
	// 	(e+(j+key_to_append->length))->type = INTEGER;
	// 	(e+(j+key_to_append->length))->value = elements[j];
	// }

	free(key_to_append->values);
	key_to_append->values = e;
	key_to_append->length += i;

	printf("ok\n\n");
	return true;

}

bool command_pick(const char line[MAX_LINE], entry * db_head) {
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	free(key);
	char * next = next_sep(line,sep_index);
	int print_index = atoi(next) - 1;
	free(next);
	entry * entry_to_print = command_get(line,db_head,false,false);

	if (entry_to_print == NULL) {
		printf("no such key\n\n");
		return false;
	} else if (entry_to_print-> length <= print_index) {
		printf("index out of range\n\n");
		return false;
	} else {
		
		if ((entry_to_print->values + print_index)->type == INTEGER){
			printf("%d\n\n", (entry_to_print->values + print_index)->value);	
		} else {
			printf("%s\n\n", (entry_to_print->values + print_index)->entry->key);
		}
		
		//printf("%d\n\n",(entry_to_print->values + print_index)->value);
		return true;
	}

}

bool command_pluck(const char line[MAX_LINE], entry * db_head) {
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	free(key);
	char * next = next_sep(line,sep_index);
	int print_index = atoi(next) - 1;
	free(next);

	entry * entry_to_pluck = command_get(line,db_head,false,false);

	if (entry_to_pluck == NULL) {
		printf("no such key\n\n");
		return false;
	} else if (entry_to_pluck-> length <= print_index) {
		printf("index out of range\n\n");
		return false;
	} else {
		//printf("%d\n\n",(entry_to_pluck->values + print_index)->value);
		if ((entry_to_pluck->values + print_index)->type == INTEGER){
			printf("%d\n\n", (entry_to_pluck->values + print_index)->value);	
		} else {
			printf("%s\n\n", (entry_to_pluck->values + print_index)->entry->key);
		}

		element * e = malloc(sizeof(element)*(entry_to_pluck->length-1));
		int offset = 0;
		for (int j = 0; j < entry_to_pluck->length; j++){
			if (j != print_index) {
				(e+j + offset)->type = (entry_to_pluck->values + j)->type;
				if ((entry_to_pluck->values + j)->type == INTEGER){
					(e+j + offset)->value = (entry_to_pluck->values + j)->value;	
				} else {
					(e+j + offset)->entry = (entry_to_pluck->values + j)->entry;	
				}
				// (e+j + offset)->type = INTEGER;
				// (e+j + offset)->value = (entry_to_pluck->values + j)->value;
			} else {
				offset = -1;
			}
		}
		entry_to_pluck->length -= 1;
		free(entry_to_pluck->values);
		entry_to_pluck->values = e;
		return true;
	}
}

bool command_pop(const char line[MAX_LINE], entry * db_head) {
	entry * entry_to_pop = command_get(line,db_head,false,false);

	if (entry_to_pop == NULL) {
		printf("no such key\n\n");
		return false;
	} else if (entry_to_pop->length == 0){
		printf("nil\n\n");
		return true;
	} else {
		//printf("%d\n\n",entry_to_pop->values->value);
		if ((entry_to_pop->values)->type == INTEGER){
			printf("%d\n\n", (entry_to_pop->values)->value);	
		} else {
			printf("%s\n\n", (entry_to_pop->values)->entry->key);
		}
		
		element * e = malloc(sizeof(element)*(entry_to_pop->length-1));
		int offset = -1;
		for (int j = 1; j < entry_to_pop->length; j++){
			// (e+j + offset)->type = INTEGER;
			// (e+j + offset)->value = (entry_to_pop->values + j)->value;
			
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

int command_min(const char line[MAX_LINE], entry * db_head, bool print){
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int min = 0;
		if (search_entry->values->type == INTEGER){
			min = search_entry->values->value;
		} else {
			char command1[MAX_LINE] = "MIN ";
			strcat(command1,search_entry->values->entry->key);
			min = command_min(command1,db_head, false);
		}
		
		for (int i = 1; i < search_entry->length; i++){
			if ((search_entry->values+i)->value < min){
				//min = (search_entry->values+i)->value;

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

int command_max(const char line[MAX_LINE], entry * db_head, bool print){
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int max = 0;
		// = search_entry->values->value;
		// for (int i = 1; i < search_entry->length; i++){
		// 	if ((search_entry->values+i)->value > max){
		// 		max = (search_entry->values+i)->value;
		// 	}
		// }
		// printf("%d\n\n",max);

		if (search_entry->values->type == INTEGER){
			max = search_entry->values->value;
		} else {
			char command1[MAX_LINE] = "MAX ";
			strcat(command1,search_entry->values->entry->key);
			max = command_max(command1,db_head, false);
		}
		
		for (int i = 1; i < search_entry->length; i++){
			if ((search_entry->values+i)->value > max){
				//min = (search_entry->values+i)->value;

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

int command_sum(const char line[MAX_LINE], entry * db_head, bool print){
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int sum = 0;
		for (int i = 0; i < search_entry->length; i++){
			//sum += (search_entry->values + i)->value;

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

int command_len(const char line[MAX_LINE], entry * db_head, bool print) {
	entry * search_entry = command_get(line,db_head,false,false);

	// if (search_entry == NULL) {
	// 	printf("no such key\\nn");
	// 	return false;
	// } else {
	// 	printf("%zu\n\n",search_entry->length);
	// 	return true;
	// }

	int len = 0;
	if (search_entry == NULL) {
		if (print == true) {
			printf("no such key\n\n");
		}
		return 0;
	} else {
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

entry * command_del(const char line[MAX_LINE], entry * db_head, bool print) {
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		if (print == true){
			printf("no such key\n\n");
		}
		
		return db_head;
	} else {
		// If trying to delete head.
		if (search_entry->prev == NULL){

			if (search_entry->next != NULL) {
				search_entry->next->prev = NULL;

				db_head = search_entry->next;
				free(search_entry->values);
				free(search_entry);


			} else {
				db_head->is_initialised = false;
				free(search_entry->values);

			}
			
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

bool command_list_keys(entry * db_head){
	if (db_head->is_initialised == false) {
		printf("no keys\n\n");
		return false;
	} else {
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

bool command_list_entries(entry * db_head){
	if (db_head->is_initialised == false) {
		printf("no entries\n\n");
		return false;
	} else {
		entry * search_entry = db_head;
		while (search_entry->next != NULL) {
			search_entry = search_entry->next;
		}
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

bool command_rev(const char line[MAX_LINE], entry * db_head){
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int * list = malloc(sizeof(int)*search_entry->length);
		for (int i = 0; i < search_entry->length; i++){
			*(list + i) = (search_entry->values + i)->value;
		}
		for (int i = 0; i < search_entry->length; i++){
			(search_entry->values + i)->value = *(list + search_entry->length-i-1);
		}
		free(list);
		printf("ok\n\n");
		return true;
	}
	return false;
}

bool command_uniq(const char line[MAX_LINE], entry * db_head){
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		int shortened = 0;
		for (int i = 0; i < search_entry->length -1 -shortened ; i++) {
			if ((search_entry->values + i)->value == (search_entry->values + i + 1)->value) {
				for (int j = i; j < search_entry->length -1; j++) {
					(search_entry->values + j)->value = (search_entry->values + j + 1)->value;

				}
				shortened += 1;
				i--;
				
			}
		}


		search_entry->length -= shortened;

		element * e = malloc(search_entry->length*sizeof(element));

		for (int i = 0; i < search_entry->length; i++){
			(e+i)->type = (search_entry->values + i)->type;
			(e+i)->value = (search_entry->values + i)->value;
		}

		free(search_entry->values);
		search_entry->values = e;

		printf("ok\n\n");
		return true;
	}
	return false;
}

int cmpfunc(const void * a, const void * b){
	return (((element*) a)->value - ((element*)b)->value);
}

bool command_sort(const char line[MAX_LINE], entry * db_head){
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

bool command_type(const char line[MAX_LINE], entry * db_head) {
	entry * search_entry = command_get(line,db_head,false,false);
	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		if (search_entry->is_simple == true) {
			printf("simple\n\n");
		} else {
			printf("general\n\n");
		}
		return true;
	}
}

entry * copy_database(entry * old_curr_entry){
	entry * new_db_head = malloc(sizeof(entry));
	entry * new_current_entry = new_db_head;
	entry * last_entry = NULL;
	element * new_vals;

	if (old_curr_entry->is_initialised == false) {
		new_db_head->is_initialised = false;
		return new_db_head;
	}

	while (old_curr_entry != NULL) {
		//memcpy(new_current_entry,old_curr_entry, sizeof(entry));
		
		//new_current_entry->backward = &old_curr_entry->backward;
		new_current_entry->backward_max = old_curr_entry->backward_max;
		new_current_entry->backward_size = old_curr_entry->backward_size;
		//new_current_entry->forward = &old_curr_entry->forward;
		new_current_entry->forward_max = old_curr_entry->forward_max;
		new_current_entry->forward_size = old_curr_entry->forward_size;
		new_current_entry->is_initialised = old_curr_entry->is_initialised;
		new_current_entry->is_simple = old_curr_entry->is_simple;
		strncpy(new_current_entry->key,old_curr_entry->key,MAX_KEY);
		new_current_entry->length = old_curr_entry->length;
		new_current_entry->next = NULL;
		new_current_entry->prev = NULL;

		//printf("%zu\n",new_current_entry->length);
		//values
		//printf("Copying entry %s %lu\n",new_current_entry->key, new_current_entry->length*sizeof(element));
		new_vals = malloc((new_current_entry->length)*sizeof(element));
		
		for (int i = 0; i < new_current_entry->length; i++) {
			//memcpy(new_vals + i,old_curr_entry->values+i,sizeof(element));
			
			(new_vals+i)->type = (old_curr_entry->values+i)->type;
			if ((new_vals+i)->type == INTEGER) {
				(new_vals+i)->value = (old_curr_entry->values+i)->value;
			} else {
				(new_vals+i)->entry = (old_curr_entry->values+i)->entry;
			}
			

		}
		new_current_entry->values = new_vals;
		
		//next prev
		if (old_curr_entry->next == NULL){
			new_current_entry->prev = last_entry;
			break;
		} else {
			new_current_entry->next = malloc(sizeof(entry));
			new_current_entry->prev = last_entry;
			
			last_entry = new_current_entry;
			new_current_entry = new_current_entry->next;
			
		}
		old_curr_entry = old_curr_entry->next;

	} 

	return new_db_head;

}

bool command_snapshot(entry * db_head, snapshot * snapshot_head){
	int snap_counter = snapshot_head->id;

	if (snapshot_head->is_initialised == false){
		snapshot_head->id = snap_counter;
		snapshot_head->prev = NULL;
		snapshot_head->next = NULL;
		snapshot_head->is_initialised = true;
		snapshot_head->entries = copy_database(db_head);
		
		printf("saved as snapshot 1\n\n");


	} else {
		snapshot * last_snap;
		last_snap = snapshot_head;
		while (last_snap->next != NULL) {
			last_snap = last_snap->next;
			snap_counter++;
		}
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

entry * command_checkout(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head, bool print) {
	snapshot * snap_search = snapshot_head;
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;

	if (key == NULL || snap_search->is_initialised == false) {
		if (print == true){
			printf("no such snapshot\n\n");
		}
		
		free(key);
		return db_head;
	}
	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			
			entry * new_db_head = copy_database(snap_search->entries);
			free(db_head);
			if (print == true){
				printf("ok\n\n");
			}
			free(key);
			return new_db_head;
		}
		snap_search = snap_search->next;

	}
	printf("no such snapshot\n\n");
	free(key);
	return db_head;

}

snapshot * command_drop(const char line[MAX_LINE], snapshot * snapshot_head, bool print) {
	snapshot * snap_search = snapshot_head;
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;

	if (key == NULL || snap_search->is_initialised == false) {
		printf("no such snapshot\n\n");
		free(key);
		return snapshot_head;
	}
	entry * entry_iterator = NULL;
	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			
			entry_iterator = snap_search->entries;
			
			while (entry_iterator->next != NULL){
				free(entry_iterator->values);
				entry_iterator = entry_iterator->next;
				free(entry_iterator->prev);
			}
			free(entry_iterator->values);
			free(entry_iterator);
			if (print == true){
				printf("ok\n\n");
			}
			
			snapshot * new_snap_head;
			free(key);
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
	free(key);
	return snapshot_head;

}

bool command_list_snapshots(snapshot * snapshot_head){
	if (snapshot_head->is_initialised == false){
		printf("no snapshots\n\n");
		return false;
	} else {
		snapshot * search_snap = snapshot_head;
		while (search_snap->next != NULL){
			search_snap = search_snap->next;
		}
		while (search_snap != NULL){
			printf("%d\n", search_snap->id);
			search_snap = search_snap->prev;
		}
		printf("\n");
		return true;
	}

}

bool command_rollback(const char line[MAX_LINE], snapshot * snapshot_head) {
	snapshot * snap_search = snapshot_head;
	snapshot * found_snap;
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;

	if (key == NULL || snap_search->is_initialised == false) {
		printf("no such snapshot\n\n");
		free(key);
		return false;
	}

	while (snap_search != NULL) {
		if (snap_search->id == atoi(key)){
			found_snap = snap_search;
			snap_search = snap_search->next;
			
			
			while (snap_search != NULL){

				char command[MAX_LINE] = "DROP ";
				int length = snprintf( NULL, 0, "%d", snap_search->id);
				char * str = malloc( length + 1 );
				snprintf(str, length + 1, "%d", snap_search->id );
				strcat(command,str);
				if (snap_search->next != NULL) {
					snap_search = snap_search->next;
				} else {
					break;
				}
				command_drop(command,snapshot_head,false);
				
				
				if (snap_search != NULL) {
					free(snap_search->prev);
				}
				
			}
			found_snap->next = NULL;
			printf("ok\n\n");
			free(key);
			return true;
		}
		snap_search = snap_search->next;

	}
	printf("no such snapshot\n\n");
	free(key);
	return false;
}

entry * command_purge(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head) {
	snapshot * snap_search = snapshot_head;
	char * str_sep = next_sep(line,0);
	int sep_index = strlen(str_sep) + 1;
	free(str_sep);
	char * key = next_sep(line,sep_index);
	sep_index += strlen(key)+1;
	char command1[MAX_LINE] = "DEL ";
	strcat(command1,key);

	entry * del_return = command_del(command1,db_head,false);
	if (del_return != NULL) {
		db_head = del_return;
	}
	
	while(snap_search != NULL && snap_search->is_initialised == true ) {
		char command2[MAX_LINE] = "DEL ";
		strcat(command2,key);
		del_return = command_del(command2,snap_search->entries,false);
		if (del_return != NULL){
			snap_search->entries = del_return;
		}
		snap_search = snap_search->next;
	}
	
	printf("ok\n\n");
	free(key);
	return db_head;
}

bool command_forward(const char line[MAX_LINE], entry * db_head){
	entry * search_entry = command_get(line,db_head,false,false);

	if (search_entry == NULL) {
		printf("no such key\n\n");
		return false;
	} else {
		if (search_entry->forward_size == 0) {
			printf("nil\n\n");
			return false;
		}
		//printf("%zu",search_entry->forward_size);
		for (int i = 0; i < search_entry->forward_size; i++) {
			if (i == search_entry->forward_size - 1){
				printf("%s", *(search_entry->forward + i));
			} else {
				printf("%s, ", *(search_entry->forward + i));
			}
		}
		printf("\n\n");
		return true;
	}
}

struct whole_db validate_line(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head){
	struct whole_db db_return;
	db_return.current_db_state = db_head;
	db_return.snapshot_head = snapshot_head;

	char * cmd1 = next_sep(line,0);
	printf("%s\n",cmd1);
	int sep_index = strlen(cmd1) + 1;
	char * cmd2 = next_sep(line,sep_index);
	sep_index += strlen(cmd2)+1;
	

	// printf("%s\n", cmd1);
	// printf("%s\n", cmd2);

	// printf("%s\n", line);
	// printf("%s\n", sep_line);


	
	//Find command
	if (strcasecmp(cmd1,"HELP") == 0){
		printf("%s",HELP);
		free(cmd1);
		free(cmd2);
		return db_return;

	} else if (strcasecmp(cmd1,"BYE") == 0) {
		command_bye();
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"KEYS") == 0) {
		//LIST KEYS
		command_list_keys(db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"ENTRIES") == 0) {
		//LIST ENTRIES
		command_list_entries(db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"LIST") == 0 && strcasecmp(cmd2,"SNAPSHOTS") == 0) {
		//LIST SNAPSHOTS
		command_list_snapshots(snapshot_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"GET") == 0) {
		//GET
		command_get(line,db_head,true,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"DEL") == 0) {
		//DEL
		db_return.current_db_state = command_del(line,db_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"PURGE") == 0) {
		//PURGE
		db_return.current_db_state = command_purge(line,db_head,snapshot_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"SET") == 0) {
		//SET
		command_set(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"PUSH") == 0) {
		command_push(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"APPEND") == 0) {
		command_append(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"PICK") == 0) {
		command_pick(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"PLUCK") == 0) {
		command_pluck(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"POP") == 0) {
		command_pop(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"DROP") == 0) {
		//DROP
		db_return.snapshot_head = command_drop(line,snapshot_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"ROLLBACK") == 0) {
		//ROLLBACK
		db_return.current_db_state = command_checkout(line,db_head,snapshot_head,false);
		command_rollback(line,snapshot_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"CHECKOUT") == 0) {
		//CHECKOUT
		db_return.current_db_state = command_checkout(line,db_head,snapshot_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"SNAPSHOT") == 0) {
		//SNAPSHOT
		command_snapshot(db_head, snapshot_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"MIN") == 0) {
		//MIN
		command_min(line,db_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"MAX") == 0) {
		//MAX
		command_max(line,db_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"SUM") == 0) {
		//SUM
		command_sum(line,db_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"LEN") == 0) {
		//LEN
		command_len(line,db_head,true);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"REV") == 0) {
		//REV
		command_rev(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"UNIQ") == 0) {
		//UNIQ
		command_uniq(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"SORT") == 0) {
		//SORT
		command_sort(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"FORWARD") == 0) {
		//FORWARD
		command_forward(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"BACKWARD") == 0) {
		//BACKWARD
		free(cmd1);
		free(cmd2);
		return db_return;
	} else if (strcasecmp(cmd1,"TYPE") == 0) {
		//TYPE
		command_type(line,db_head);
		free(cmd1);
		free(cmd2);
		return db_return;
	} else {
		printf("Invalid command\n");
		free(cmd1);
		free(cmd2);

	}

	return db_return;
}

char * next_sep(const char line[MAX_LINE], int pos){
	char * res = malloc(strlen(line));
	int i = pos;
	while (line[i] != 0 && line[i] != 32){
		*(res + i-pos) = line[i];
		i++;
	}
	if (i == pos){
		return NULL;
	}
	return res;
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
			command_bye();
			return 0;
		}
		//Get input
		line[strlen(line)-1] = '\0';
		
		db = validate_line(line, db.current_db_state, db.snapshot_head);

  	}

	return 0;
}