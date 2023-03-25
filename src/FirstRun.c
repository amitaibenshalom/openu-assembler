#include "FirstRun.h"

int firstRun(FILE* file, char* base_file_name) {
    char* line = (char*) malloc(MAX_LINE_SIZE * sizeof (char));
    size_t line_number = 0;
    size_t command_index = 0;
    char* token;

    /* pointers to start and finish of string in .string*/
    char* first_quote, *last_quote;

    /* create list for labels */
    node_t* label_list = NULL;

    char* current_label = NULL;
    bool label_flag = false;

    /* error flag */
    bool error_flag = false;

    /* create list for external and entry labels */
    node_t* extern_list = NULL;
    node_t* entry_list = NULL;

    /* init IC and DC */
    size_t IC = START_ADD;
    size_t DC = 0;

    /* read new line from file */
    while(fgets(line, MAX_LINE_SIZE, file) != NULL) {
        /* increase line counter */
        line_number++;
        /* split line into tokens */
        token = strtok(strdup(line), SPACE_SEP);

        label_flag = false;

        /* check if there is a label, TODO: improve maybe move to other function */
        if(strchr(line, LABEL_SEP)) {
            /* check if label is valid */
            if (isValidLabel(token)) {
                label_flag = true;
                current_label = token;
                current_label[strlen(current_label)-1] = NULL_TERMINATOR;
                token = strtok(NULL, SPACE_SEP);

                if (token == NULL) {
                    line_error(MISSING_CODE_AFTER_LABEL, base_file_name, line_number);
                    error_flag = true;
                    continue;
                }

                /* check if label exists in future labels */
                if (findLabelInList(current_label, label_list)) {
                    line_error(MULTIPLE_LABEL_DEFINITIONS, base_file_name, line_number);
                    error_flag = true;
                    continue;
                }
            } else {
                /* label is not valid */
                line_error(LABEL_SYNTAX_ERROR, base_file_name, line_number);
                error_flag = true;
                continue;
            }
        }

        /* check all symbol types */
        if (IS_DATA_SYMBOL(token)) {
            /* check if label is already defined */
            if (label_flag) {
                label_list = addLabelNode(label_list, current_label, DC, Data);
            }
            /* check for .data symbol */
            if (isStrEqual(token, DATA_SYMBOL)) {
                /* calculate data length */
                while((token = strtok(NULL, COMMA_SEP)) != NULL) {
                    if (is_number(token)) {
                        /* TODO: calculate if number if out of range and number of words needed for it */
                        DC++;
                    } else {
                        line_error(DATA_SYNTAX_ERROR, base_file_name,line_number);
                        error_flag = true;
                        continue;
                    }
                }
            }
            else if (isStrEqual(token, STRING_SYMBOL)) {
                token = strtok(NULL,SPACE_SEP);
                first_quote = strchr(line, STRING_QUOTE);
                last_quote = strrchr(line, STRING_QUOTE);
                if (token == NULL) {
                    line_error(STRING_MISSING_ARGUMENT, base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                if (last_quote == NULL || first_quote == NULL || last_quote <= first_quote) {
                    line_error(STRING_MISSING_QUOTE, base_file_name, line_number);
                    error_flag = true;
                    continue;
                }
                if (strtok(last_quote+1,SPACE_SEP) != NULL) {
                    line_error(STRING_SYNTAX_ERROR, base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                strncpy(token, first_quote, last_quote-first_quote);
                token[last_quote-first_quote+1] = NULL_TERMINATOR;
                if (token[0] != STRING_QUOTE || token[strlen(token)-1] != STRING_QUOTE) {
                    line_error(STRING_SYNTAX_ERROR, base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                DC += strlen(token)-1;
            }
        }
        else if (IS_EXTERN_SYMBOL(token) || IS_ENTRY_SYMBOL(token)) {

            /* case: extern symbol */
            if(IS_EXTERN_SYMBOL(token)) {
                /* get external label */
                token = strtok(NULL,SPACE_SEP);

                /* if not found raise error */
                if (token == NULL) {
                    line_error(EXTERN_MISSING_ARGUMENT,base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                /* if more than one found raise error */
                if(strtok(NULL,SPACE_SEP) != NULL) {
                    line_error(EXTERN_TOO_MANY_ARGUMENTS, base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                /* if the label is not valid raise error */
                if (!isValidLabel(token)) {
                    line_error(LABEL_SYNTAX_ERROR,base_file_name,line_number);
                    error_flag = true;
                    continue;
                }

                /* if the label is already called from external file raise error */
                if (findLabelInList(token, extern_list)) {
                    line_error(MULTIPLE_EXTERN_CALLS, base_file_name, line_number);
                    error_flag = true;
                    continue;
                }
                /* add to external list */

                if (label_flag) {
                    line_warning(LABEL_DEF_BEFORE_EXTERN, base_file_name, line_number);
                }
                extern_list = addLabelNode(extern_list, token, line_number, Extern);
            } else {

                /* get entry label */
                token = strtok(NULL,SPACE_SEP);

                /* if no token found raise error */
                if (token == NULL) {
                    line_error(ENTRY_MISSING_ARGUMENT,base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                /* if more than one found raise error */
                if(strtok(NULL,SPACE_SEP) != NULL) {
                    line_error(ENTRY_TOO_MANY_ARGUMENTS, base_file_name,line_number);
                    error_flag = true;
                    continue;
                }
                /* if label is not valid raise error */
                if (!isValidLabel(token)) {
                    line_error(LABEL_SYNTAX_ERROR,base_file_name,line_number);
                    error_flag = true;
                    continue;
                }

                /* if label is already defined raise error */
                if (findLabelInList(token, entry_list)) {
                    line_error(MULTIPLE_ENTRY_CALLS, base_file_name, line_number);
                    error_flag = true;
                    continue;
                }

                if (label_flag) {
                    line_warning(LABEL_DEF_BEFORE_ENTRY, base_file_name, line_number);
                }
                /* add to entry list */
                entry_list = addLabelNode(entry_list, token, line_number, Entry);
            }
        }
        else {
            /* get current command index */
            if ((command_index = find_command(token)) != -1) {

                /* add label if needed */
                if (label_flag) {
                    label_list = addLabelNode(label_list, current_label, IC, Code);
                }

                /* check command type (group) */
                int command_length = 1;
                int args_counter = 0;
                command_t command = commands[command_index];

                bool is_jump = (!command.arg1_optional_types) && (command.arg2_optional_types & Jump);

                arg_type source_type = None, dest_type = None;

                /* if expecting 1 arg */
                if(command.arg1_optional_types) {
                    token = strtok(NULL, COMMA_SEP);
                    if(token) {
                        if((source_type = get_arg_type(token, command.arg1_optional_types)) == None) {
                            line_error(INVALID_SOURCE_ARG, base_file_name, line_number);
                            error_flag = true;
                            continue;
                        }

                        if(!(source_type & command.arg1_optional_types)) {
                            line_error(INVALID_SOURCE_ARG, base_file_name, line_number);
                            error_flag = true;
                        } else {
                            command_length++;
                        }
                    } else {
                        line_error(TOO_FEW_ARGS, base_file_name, line_number);
                        continue;
                    }
                }

                /* if expecting 2 args, check if there is a second arg */
                if(command.arg2_optional_types) {
                    token = strtok(NULL, is_jump ? LINE_BREAK : COMMA_SEP);
                    if(token) {
                        /* ok lets check for Jump type */
                        if ((dest_type = get_arg_type(token, command.arg2_optional_types)) == None) {
                            line_error(INVALID_DEST_ARG, base_file_name, line_number);
                            error_flag = true;
                            continue;
                        }
                        if (!(dest_type & command.arg2_optional_types)) {
                            line_error(INVALID_DEST_ARG, base_file_name, line_number);
                            error_flag = true;
                        } else {
                            if(dest_type == Jump) {
                                /* get jump params and check the length of overall command */
                                command_length += getJumpParamsLength(token);
                            } else {
                                command_length++;
                            }
                        }
                    } else {
                        line_error(TOO_FEW_ARGS, base_file_name, line_number);
                        continue;
                    }

                }

                if(strtok(NULL, SPACE_SEP) != NULL) {
                    line_error(TOO_MANY_ARGS, base_file_name, line_number);
                    continue;
                }

                /* if both args are registers, decrease command length by 1 */
                if(source_type == Register && dest_type == Register)
                    command_length--;

                IC += command_length;
                /* printf("\t%lu command: %s length: %d\n", line_number, command.name, command_length); */
            } else {
                line_error(COMMAND_NOT_FOUND, base_file_name, line_number);
                continue;
            }
        }
    }

    /* update DC */
    updateDC(IC, label_list, NULL);
    DC += IC;

    info_file("Finished first run", base_file_name);
    /* printf("IC: %d DC: %d\n", IC, DC); */

    /* start second run */
    rewind(file);
    return second_run(IC, DC, label_list, extern_list, entry_list, error_flag, file ,base_file_name);

    return error_flag;
}

bool deleteLabel(char* name, node_t** head) {
    node_t* current = *head;
    node_t* prev = NULL;
    label_t* label;
    while(current != NULL) {
        if(isStrEqual((label = (label_t*) current->data)->name, name)) {
            if(prev == NULL) {
                *head = (node_t*) current->next;
            } else {
                prev->next = current->next;
            }
            free(label->name);
            free(label);
            free(current);
            return true;
        }
        prev = current;
        current = (node_t*) current->next;
    }
    return false;
}



