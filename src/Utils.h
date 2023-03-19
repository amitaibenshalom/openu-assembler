#ifndef UTILS_HEADER
#define UTILS_HEADER

#include "Includes.h"
#include "Consts.h"
#include "Types.h"

#define isStrEqual(a, b) (strcmp(a, b) == 0)

#define info(msg) printf("[info]: %s\n", msg)
#define info_file(msg, file_name) printf("[info]: %s: %s\n", msg, file_name)

bool is_number(char*);

int find_command(char*);

bool isValidLabel(char*);
label_t* findLabelInList(char *name, node_t *head);
label_t* findLabel(char*, node_t*, ...);
node_t* addLabelNode(node_t*, char*, size_t, label_type);

bool isValidLabelFormat(char*);

int getJumpParamsLength(char*);
arg_type get_arg_type(char*, arg_type);
int find_register(char*);

char* getFileName(char*, char*);
FILE* openFile(char*, char*);

void free_list(node_t*);

#endif
