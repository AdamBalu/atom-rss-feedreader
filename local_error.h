#ifndef FEEDREADER_ERROR_H
#define FEEDREADER_ERROR_H

#define ERR_WRONG_ARGS 50
#define ERR_ESTABLISHING_CONNECTION 51
#define ERR_CONNECTION 52
#define ERR_READ 53
#define ERR_CERTIFICATES 54
#define ERR_XML_NOT_DETECTED 55
#define ERR_NO_LINK 57
#define ERR_INVALID_LINK 58
#define ERR_OUT 59
#define ERR_OPEN_FILE 60
#define ERR_FILE_FORMAT 61
#define ERR_XML_FORMAT 62
#define ERR_HTTP_RESPONSE 63

#define WARNING_MISSING_VALS 70
#define WARNING_TOO_MANY_PARAMS 71

/**
 * Prints provided error on standard error output and exits the program if needed
 * @param err error
 * @return error status (1)
 */
int handleError(int err);

/**
 * Prints provided warning on standard error output
 * @param warning warning
 */
void handleWarning(int warning);

/**
 * Prints provided warning on standard error output, handles warning with option value
 * @param option
 */
void handleWarningOption(char option);

#endif // FEEDREADER_ERROR_H
