#ifndef YMIRDB_H
#define YMIRDB_H

#define MAX_KEY 16
#define MAX_LINE 1024

#include <stddef.h>
#include <sys/types.h>

enum item_type {
    INTEGER=0,
    ENTRY=1
};

typedef struct element element;
typedef struct entry entry;
typedef struct snapshot snapshot;
typedef struct reference reference;
typedef struct copy_reference copy_reference;

struct element {
  enum item_type type;
  union {
    int value;
    struct entry *entry;
  };
};

struct reference {
	char key[MAX_KEY];
	struct reference* next;
	struct reference* prev;
};

struct entry {
  bool is_initialised;
  char key[MAX_KEY];
  char is_simple;
  element * values;
  size_t length;

  entry* next;
  entry* prev;
  
  size_t forward_size; 
  size_t forward_max; 
  char * forward[MAX_KEY];  // this entry depends on these
    
  size_t backward_size; 
  size_t backward_max; 
  char * backward[MAX_KEY]; // these entries depend on this
};

struct snapshot {
  int id;
  bool is_initialised;
  entry* entries;
  snapshot* next;
  snapshot* prev;
};

struct whole_db {
	snapshot * snapshot_head;
	entry * current_db_state;
};

struct copy_reference {
	char key_entry[MAX_KEY];
	char key_val[MAX_KEY];
	int pos;
};


const char* HELP =
	"BYE   clear database and exit\n"
	"HELP  display this help message\n"
	"\n"
	"LIST KEYS       displays all keys in current state\n"
	"LIST ENTRIES    displays all entries in current state\n"
	"LIST SNAPSHOTS  displays all snapshots in the database\n"
	"\n"
	"GET <key>    displays entry values\n"
	"DEL <key>    deletes entry from current state\n"
	"PURGE <key>  deletes entry from current state and snapshots\n"
	"\n"
	"SET <key> <value ...>     sets entry values\n"
	"PUSH <key> <value ...>    pushes values to the front\n"
	"APPEND <key> <value ...>  appends values to the back\n"
	"\n"
	"PICK <key> <index>   displays value at index\n"
	"PLUCK <key> <index>  displays and removes value at index\n"
	"POP <key>            displays and removes the front value\n"
	"\n"
	"DROP <id>      deletes snapshot\n"
	"ROLLBACK <id>  restores to snapshot and deletes newer snapshots\n"
	"CHECKOUT <id>  replaces current state with a copy of snapshot\n"
	"SNAPSHOT       saves the current state as a snapshot\n"
	"\n"
	"MIN <key>  displays minimum value\n"
	"MAX <key>  displays maximum value\n"
	"SUM <key>  displays sum of values\n"
	"LEN <key>  displays number of values\n"
	"\n"
	"REV <key>   reverses order of values (simple entry only)\n"
	"UNIQ <key>  removes repeated adjacent values (simple entry only)\n"
	"SORT <key>  sorts values in ascending order (simple entry only)\n"
	"\n"
	"FORWARD <key> lists all the forward references of this key\n"
	"BACKWARD <key> lists all the backward references of this key\n"
	"TYPE <key> displays if the entry of this key is simple or general\n\n";

//Function to free all entries in a current data base state - as passed in as a parameter.
void free_all_entries(entry * cur_state);

//Function for bye command
void command_bye(snapshot * snaps, entry * cur_state);

void command_help();

//Function for set command.
bool command_set(const char line[MAX_LINE], entry * db_head);

//Function for get command.
entry * command_get(const char line[MAX_LINE], entry * db_head, bool display, bool newline);

//Function for push command
bool command_push(const char line[MAX_LINE], entry * db_head);

//Function for append command
bool command_append(const char line[MAX_LINE], entry * db_head);

//Function for pick command
bool command_pick(const char line[MAX_LINE], entry * db_head);

//Function for pluck command
bool command_pluck(const char line[MAX_LINE], entry * db_head);

//Function for pop command
bool command_pop(const char line[MAX_LINE], entry * db_head);

//Function for min command
int command_min(const char line[MAX_LINE], entry * db_head, bool print);

//Function for max command
int command_max(const char line[MAX_LINE], entry * db_head, bool print);

//Function for sum command
int command_sum(const char line[MAX_LINE], entry * db_head, bool print);

//Function for len command
int command_len(const char line[MAX_LINE], entry * db_head, bool print);

//Function for del command
entry * command_del(const char line[MAX_LINE], entry * db_head, bool print);

//Function for list keys command
bool command_list_keys(entry * db_head);

//Function for list entries command
bool command_list_entries(entry * db_head);

//Function for reverse command
bool command_rev(const char line[MAX_LINE], entry * db_head);

//Function for unig command
bool command_uniq(const char line[MAX_LINE], entry * db_head);

//Comparison function for sort
int cmpfunc (const void * a, const void * b);

//Function for sort command
bool command_sort(const char line[MAX_LINE], entry * db_head);

//Function for type command
bool command_type(const char line[MAX_LINE], entry * db_head);

//Function to copy database to new memory
entry * copy_database(entry * old_curr_entry);

//Function for snapshot commmand
bool command_snapshot (entry * db_head, snapshot * snapshot_head);

//Function for checkout command
entry * command_checkout (const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head, bool print);

//Function for drop command
snapshot * command_drop(const char line[MAX_LINE], snapshot * snapshot_head, bool print);

//Function for list snapshots command
bool command_list_snapshots(snapshot * snapshot_head);

//Function for rollback command
bool command_rollback(const char line[MAX_LINE], snapshot * snapshot_head);

//Function for purge command
entry * command_purge(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head);

//Comparison function for reference list sorting
int sortstring(const void* a, const void* b);

//Function for forward command
bool command_forward(const char line [MAX_LINE], entry * db_head, bool print);

//Function to recursively calculate forwards
reference * calculate_forwards (const char line[MAX_LINE], entry * db_head, reference * f);

//Function for backward command
bool command_backward(const char line[MAX_LINE], entry * db_head, bool print);

//Validate line and perform commands
struct whole_db validate_line(const char line[MAX_LINE], entry * db_head, snapshot * snapshot_head);


#endif