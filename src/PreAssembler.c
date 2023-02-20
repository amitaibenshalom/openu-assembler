#include "PreAssembler.h"

char* pre_assemble(FILE* file, char* input_file_name) {
    /* open pre assembler file */
    FILE* pre_assembled_file;
    char* output_file_name = strdup(input_file_name);
    /* line buffer & line number */
    char* line = (char*) malloc(MAX_LINE_SIZE * sizeof (char));
    size_t line_number = 0;
    /* token pointer */
    char* token;

    /* macro list & macro flag */
    macro_node_t* macro_list = NULL;
    macro_t* current_macro = NULL;
    macro_node_t* current_macro_node = NULL;
    bool macro_flag = false;

    /* add pre assembler file extension & start writing to file */
    strcat(output_file_name, PRE_ASSEMBLER_FILE_EXTENSION);
    pre_assembled_file = fopen(output_file_name, "w");

    /* read new line from file */
    while(fgets(line, MAX_LINE_SIZE, file) != NULL) {
        /* increase line counter */
        line_number++;

        /* split line into tokens */
        token = strtok(strdup(line), SPACE_SEP);

        /* skip comment & empty line */
        if(token == NULL || token[0] == COMMENT_CHAR) continue;

        /* check if in macro def */
        if(macro_flag) {
            /* check for end of macro def */
            if(strcmp(token, END_MACRO_SYMBOL) == 0) {
                /* check for extra tokens */
                if(strtok(NULL, SPACE_SEP)) {
                    line_error(MACRO_SYNTAX_ERROR, input_file_name, line_number);
                }
                macro_flag = false;
            } else {
                /* add line to macro */
                strcat(current_macro->data, line);
            }
        } else {
            /* check if token is an existing macro */
            if((current_macro = find_macro(token, macro_list)) != NULL) {
                fputs(current_macro->data, pre_assembled_file);
            } else {
                /* check for start of new macro def */
                if(strcmp(token, START_MACRO_SYMBOL) == 0) {
                    /* get macro name */
                    token = strtok(NULL, SPACE_SEP);

                    if( !is_valid_macro_name(token) ||
                            find_macro(token, macro_list) ||
                            strtok(NULL, SPACE_SEP)) {
                        line_error(MACRO_SYNTAX_ERROR, input_file_name, line_number);
                    } else {
                        /* create new macro */
                        macro_flag = true;

                        /* set current macro to the new macro */
                        current_macro = (macro_t*) (malloc(sizeof (macro_t)));

                        /* set macro name and allocate its data */
                        current_macro->name = strdup(token);
                        current_macro->data = (char*) malloc(MAX_LINE_SIZE * sizeof (char));

                        /* create new macro node and append it to the start of list */
                        current_macro_node = (macro_node_t*) (malloc(sizeof (macro_node_t)));
                        current_macro_node->macro = current_macro;
                        current_macro_node->next = (struct macro_node_t *) macro_list;
                        macro_list = current_macro_node;
                    }

                } else {
                    /* write line to pre-assembled file */
                    fputs(line, pre_assembled_file);
                }
            }
        }
    }

    /* free all variables */
    while(macro_list != NULL) {
        current_macro_node = macro_list;
        macro_list = (macro_node_t *) macro_list->next;
        free(current_macro_node->macro->name);
        free(current_macro_node->macro->data);
        free(current_macro_node->macro);
        free(current_macro_node);
    }

    free(line);

    if(macro_flag) {
        file_error(MACRO_SYNTAX_ERROR, input_file_name);
    }

    fclose(pre_assembled_file);
    fprintf(stdout, "Created Pre Assembled file: %s\n", output_file_name);

    return output_file_name;
}

macro_t* find_macro(char* name, macro_node_t* head) {
    while(head != NULL) {
        if(strcmp(head->macro->name, name) == 0) {
            return head->macro;
        }
        head = (macro_node_t *) head->next;
    }
    return NULL;
}

bool is_valid_macro_name(char* macro_name) {
    /* check if macro name is valid - only contains alphabetic and numbers */
    for(; *macro_name != '\0'; macro_name++) {
        if( !isalpha(*macro_name) && !isdigit(*macro_name) )
            return false;
    }
    return true;
}
