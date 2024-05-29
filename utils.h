#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define ALLOCATE_STRING(var, size) do { \
    var = (char *)malloc(size); \
    if (!var) { \
        perror("Memory allocation failed"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define LOG_ACTION(user, action) printf("BEGIN %s %s\n", user, action)

#define LOG_DENY(operation, resource, token, seconds) \
    printf("DENY (%s,%s,%s,%d)\n", operation, resource, token, seconds)

#define LOG_PERMIT(operation, resource, token, seconds) \
    printf("PERMIT (%s,%s,%s,%d)\n", operation, resource, token, seconds)

#define LOG_ERROR(message) \
    printf("ERROR (%s)\n", message)

#define FREE_MEMORY(ptr) do { free(ptr); ptr = NULL; } while(0)

#define CHECK_TOKEN_VALIDITY(user) ((user).is_token_validated && (user).tokens.token_validity_seconds > 0)

#define INITIALIZE_STRING(var, str) do { strncpy(var, str, sizeof(var) - 1); var[sizeof(var) - 1] = '\0'; } while(0)

#define ASSIGN_TOKEN(user, field, token) do { \
    INITIALIZE_STRING(user.tokens.field, token); \
} while(0)

#define PRINT_REQUEST_TOKEN(token) printf("  RequestToken = %s\n", token)
#define PRINT_ACCESS_TOKEN(token) printf("  AccessToken = %s\n", token)
#define PRINT_REFRESH_TOKEN(token) printf("  RefreshToken = %s\n", token)
#define PRINT_STRING(result) printf("%s\n", *(result))

#define PRINT_TOKEN_DETAILS(result, access_token) \
    printf("%s -> %s", *(result), (access_token))

#define PRINT_COMPLETE_TOKEN_DETAILS(result, access_token, refresh_token, use_refresh) \
    do { \
        PRINT_TOKEN_DETAILS(result, access_token); \
        if (use_refresh) { \
            printf(",%s\n", (refresh_token)); \
        } else { \
            printf("\n"); \
        } \
    } while (0)

#define STR_OPERATION_NOT_PERMITTED "OPERATION_NOT_PERMITTED"
#define STR_RESOURCE_NOT_FOUND "RESOURCE_NOT_FOUND"
#define STR_PERMISSION_DENIED "PERMISSION_DENIED"
#define STR_PERMISSION_GRANTED "PERMISSION_GRANTED"
#define STR_TOKEN_EXPIRED "TOKEN_EXPIRED"
#define STR_USER_NOT_FOUND "USER_NOT_FOUND"
#define STR_REQUEST_DENIED "REQUEST_DENIED"
#define STR_AUTHZ "AUTHZ"
#define STR_AUTHZ_REFRESH "AUTHZ REFRESH"
#define STR_EXECUTE "EXECUTE"
#define STR_CALL_FAILED "call failed"
#define STR_UDP "udp"
#define STR_ERROR_OPENING_FILE "error at opening file!"
#define STR_ERROR_USER_NOT_FOUND "User not found"
#define STR_REQUEST_OPERATION "REQUEST"

#define STR_EMPTY ""
#define STR_DASH "-"
#define STR_WILDCARD "*"
#define STR_NULL_TERMINATOR "\0"

#define CHAR_COMMA ','
#define CHAR_NEWLINE '\n'

#define MEMORY_SIZE 50
#define NULLPTR (char **)NULL
#define TOKEN_DETAILS_NULL (struct token_details *)NULL
#define INT_PTR_NULL (int *)NULL


map<string, bool> auto_refresh;
map<string, string> user_refresh;
map<string, string> user_data;

#endif /* UTILS_H */
