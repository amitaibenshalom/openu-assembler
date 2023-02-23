#ifndef ERRORS_HEADER
#define ERRORS_HEADER

/* error messages */
typedef enum {
    FILE_OPEN_ERROR,
    MACRO_SYNTAX_ERROR,
    UNKNOWN_COMMAND,
    LABEL_SYNTAX_ERROR,
    MULTIPLE_LABEL_DEFINITIONS,
    DATA_SYNTAX_ERROR,
    EXTERN_MISSING_ARGUMENT,
    ENTRY_MISSING_ARGUMENT,
    COMMAND_NOT_FOUND
} ERRORS_INDEX;

extern const char* ERRORS[];

#define error(error_index) (fprintf(stderr, "[error]: %s\n", ERRORS[error_index]))
#define line_error(error_index, file_name, line) (fprintf(stderr, "[error]: %s (file: %s, line: %lu)\n", ERRORS[error_index], file_name, line))
#define file_error(error_index, file_name) (fprintf(stderr, "[error]: %s (file: %s)\n", ERRORS[error_index], file_name))

/* warning messages */
typedef enum {
    FILE_NOT_SPECIFIED,
    MACRO_SYNTAX_WARNING
} WARNINGS_INDEX;

extern const char* WARNINGS[];

#define warn(warning) (fprintf(stderr, "[warning]: %s\n", WARNINGS[warning]))
#define line_warning(warning_index, file_name, line) (fprintf(stderr, "[warning]: %s (file: %s, line: %lu)\n", WARNINGS[warning_index], file_name, line))
#define file_warning(warning_index, file_name) (fprintf(stderr, "[warning]: %s (file: %s)\n", WARNINGS[warning_index], file_name))


#endif
