/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "oauth.h"
#include "token.h"
#include "utils.h"

// Global map to store approval details for each user
map<string, vector<struct permissions_detail>> approvals;

// Function to generate an access token for a given client ID token
char *generate_access_token(char *clientIdToken);

// Function to find a user by user ID and generate an access token if found
bool find_user(char *user_id, char **result_token) {
    for (auto& user : user_list) {
        if (!strcmp(user.user_identifier, user_id)) {
			// Generate access token
            strcpy(*result_token, generate_access_token(user_id));
			// Store request token
            strcpy(user.tokens.request_token, *result_token);
            return true;
        }
    }
    return false;
}

// Function to initialize a permissions_detail struct with targeted resource and granted permissions
permissions_detail initialize_permissions_detail(string targeted_resource, string granted_permissions) {
    permissions_detail permissions_detail;
	// Allocate memory for targeted resource and granted permissions
    ALLOCATE_STRING(permissions_detail.targeted_resource, MEMORY_SIZE);
    ALLOCATE_STRING(permissions_detail.granted_permissions, MEMORY_SIZE);
	// Copy targeted resource string and granted permissions string
    strcpy(permissions_detail.targeted_resource, targeted_resource.c_str());
    strcpy(permissions_detail.granted_permissions, granted_permissions.c_str());
    return permissions_detail;
}

// Function to update the permissions details for a given result token using a line of data
void update_permissions_details(string result_token, string line) {
    istringstream lineStream(line);
    string token, targeted_resource, granted_permissions;
    vector<struct permissions_detail> perms;
    int i = 2;
    while (getline(lineStream, token, CHAR_COMMA)) {
        if (i % 2 == 0) targeted_resource = token;
        else {
            granted_permissions = token;
			// Add permissions detail
            perms.push_back(initialize_permissions_detail(targeted_resource, granted_permissions));
        }
        i++;
    }
	// Update global map with permissions details
    approvals[result_token] = perms;
}

// RPC service function to handle authorization requests
char **request_authorization_1_svc(char **argp, struct svc_req *rqstp)
{
    static char *result;
	// Allocate memory for the result
    ALLOCATE_STRING(result, MEMORY_SIZE);

	// Log the authorization request
    LOG_ACTION(*argp, STR_AUTHZ);

    if (!find_user(*argp, &result)) {
		// User is not found
        strcpy(result, STR_USER_NOT_FOUND);
        return &result;
    }

    string line;
    if (!getline(approvals_file, line, CHAR_NEWLINE)) {
		// Print the request token
        PRINT_REQUEST_TOKEN(result);
        return &result;
    }

	// Update permissions details
    update_permissions_details(string(result), line);
	// Print the request token
    PRINT_REQUEST_TOKEN(result);
    return &result;
}

// Function to initialize the token_details struct
void initialize_token_details(struct token_details *details) {
	// Allocate memory for request token, access token, refresh token and error message
    ALLOCATE_STRING(details->request_token, MEMORY_SIZE);
    ALLOCATE_STRING(details->access_token, MEMORY_SIZE);
    ALLOCATE_STRING(details->refresh_token, MEMORY_SIZE);
    ALLOCATE_STRING(details->token_error_message, MEMORY_SIZE);
}

// Function to determine if a refresh action should be logged
bool should_log_refresh_action(bool refresh_token_requested, int token_validity, bool initiate_refresh) {
    return refresh_token_requested && token_validity == 0 && initiate_refresh;
}

// Function to set access and refresh tokens in user credentials and token_details struct
void set_tokens(user_credentials& user, token_details& result, const char* access_token, const char* refresh_token = nullptr) {
	// Set access token in user credentials and result
    strcpy(user.tokens.access_token, access_token);
    strcpy(result.access_token, access_token);
	// Print the access token
    PRINT_ACCESS_TOKEN(result.access_token);

    if (refresh_token) {
		// Set refresh token in user credentials and result
        strcpy(user.tokens.refresh_token, refresh_token);
        strcpy(result.refresh_token, refresh_token);
		// Print the refresh token
        PRINT_REFRESH_TOKEN(result.refresh_token);
    }
}

// RPC service function to handle access token requests
struct token_details *request_access_token_1_svc(struct token_refresh_request *argp, struct svc_req *rqstp) {
    static struct token_details result;
	// Initialize token details
    initialize_token_details(&result);

    bool found = false;
    for (auto& user : user_list) {
        if (!strcmp(user.user_identifier, argp->user_identifier)) {
            found = true;
            if (!user.is_token_validated) {
				// Set error message if token is not validated
                strcpy(result.token_error_message, STR_REQUEST_DENIED);
                break;
            }

			// Allocate memory for access token and refresh token
            ALLOCATE_STRING(user.tokens.access_token, MEMORY_SIZE);
            ALLOCATE_STRING(user.tokens.refresh_token, MEMORY_SIZE);

            char *access_token = NULL;
			// Allocate memory for generated access token
            ALLOCATE_STRING(access_token, MEMORY_SIZE);
			// Generate access token
            strcpy(access_token, generate_access_token(argp->request_token));

            char *refresh_token = NULL;
            if (argp->refresh_token) {
				// Allocate memory for refresh token
                ALLOCATE_STRING(refresh_token, MEMORY_SIZE);
				// Generate refresh token
                strcpy(refresh_token, generate_access_token(access_token));
            }

            if (should_log_refresh_action(argp->refresh_token, user.tokens.token_validity_operations, argp->initiate_refresh)) {
				// Log refresh action
                LOG_ACTION(argp->user_identifier, STR_AUTHZ_REFRESH);
            }

			// Set access and refresh tokens
            set_tokens(user, result, access_token, refresh_token);

			// Set token validity seconds
            user.tokens.token_validity_operations = token_validity_operations;
			// Set result token validity seconds
            result.token_validity_operations = token_validity_operations;

			// Free allocated memory
            FREE_MEMORY(access_token);
            if (refresh_token) {
                FREE_MEMORY(refresh_token);
            }
            break;
        }
    }

    if (!found) {
		// User is not found
        strcpy(result.token_error_message, STR_ERROR_USER_NOT_FOUND);
    }

    return &result;
}

// Function to check if a requested operation is permitted for a given token and resource
bool is_operation_permitted(const char* requested_operation, const char* targeted_resource, const string& request_token) {
    // Find permissions associated with the request token
    auto& permissions = approvals[request_token];

    for (auto& p : permissions) {
        // Check if the current permission matches the targeted resource
        if (!strcmp(p.targeted_resource, targeted_resource)) {
            string permissions(p.granted_permissions);
            char permission_required;

            // Determine the required permission based on the requested operation
            if (!strcmp(requested_operation, STR_READ)) {
                permission_required = CHAR_R;
            } else if (!strcmp(requested_operation, STR_EXECUTE)) {
                permission_required = CHAR_X;
            } else if (!strcmp(requested_operation, STR_MODIFY)) {
                permission_required = CHAR_M;
            } else if (!strcmp(requested_operation, STR_DELETE)) {
                permission_required = CHAR_D;
            } else if (!strcmp(requested_operation, STR_INSERT)) {
                permission_required = CHAR_I;
            } else {
                // If the operation is unknown or not supported, deny permission
                return false;
            }

            // Check if the required permission is present in the permissions string
            if (permissions.find(permission_required) != string::npos) {
                return true;
            }
        }
    }

    // Return false if no matching permissions are found
    return false;
}



// RPC service function to validate a delegated action.
char **validate_delegated_action_1_svc(struct operation_details *argp, struct svc_req *rqstp) {
    static char *result;
	// Allocate memory for the result
    ALLOCATE_STRING(result, MEMORY_SIZE);
	// Initialize result with null terminator
    strcpy(result, STR_NULL_TERMINATOR);

    if (!strlen(argp->access_token)) {
		// Log deny action if access token is empty
        LOG_DENY(argp->type_of_operation, argp->targeted_resource, STR_EMPTY, 0);
		// Set result to "permission denied"
        strcpy(result, STR_PERMISSION_DENIED);
        return &result;
    }

    bool found = false;
    int token_validity_operations = 0;
    char access_token[MEMORY_SIZE];

    for (auto& user : user_list) {
        if (!strcmp(user.tokens.access_token, argp->access_token)) {
			// Copy access token
            strcpy(access_token, argp->access_token);
            found = true;

            if (!user.is_token_validated) {
				// Log deny action if token is not validated
                LOG_DENY(argp->type_of_operation, argp->targeted_resource, STR_EMPTY, user.tokens.token_validity_operations);
				// Set result to "request denied"
                strcpy(result, STR_REQUEST_DENIED);
                break;
            }

            if (!user.tokens.token_validity_operations) {
				// Log deny action if token is expired
                LOG_DENY(argp->type_of_operation, argp->targeted_resource, STR_EMPTY, user.tokens.token_validity_operations);
				// Set result to "token expired"
                strcpy(result, STR_TOKEN_EXPIRED);
                break;
            }

            auto index = find(resources.begin(), resources.end(), string(argp->targeted_resource));
            if (index != resources.end()) {
				// Check if operation is permitted
                bool permitted = is_operation_permitted(argp->type_of_operation, argp->targeted_resource, user.tokens.request_token);
				// Decrease token validity seconds
                user.tokens.token_validity_operations--;
                token_validity_operations = user.tokens.token_validity_operations;
                if (permitted) {
					// Log permit action
                    LOG_PERMIT(argp->type_of_operation, argp->targeted_resource, access_token, user.tokens.token_validity_operations);
					// Set result to "permission granted"
                    strcpy(result, STR_PERMISSION_GRANTED);
                } else {
					// Log deny action
                    LOG_DENY(argp->type_of_operation, argp->targeted_resource, access_token, user.tokens.token_validity_operations);
					// Set result to "operation not permitted"
                    strcpy(result, STR_OPERATION_NOT_PERMITTED);
                }
                break;
            } else {
				// Decrease token validity seconds
                user.tokens.token_validity_operations--;
				// Log deny action if resource not found
                LOG_DENY(argp->type_of_operation, argp->targeted_resource, access_token, user.tokens.token_validity_operations);
				// Set result to "resource not found"
                strcpy(result, STR_RESOURCE_NOT_FOUND);
                break;
            }
        }
    }

    if (!found) {
		// Log deny action if user is not found
        LOG_DENY(argp->type_of_operation, argp->targeted_resource, STR_EMPTY, token_validity_operations);
		// Set result to "permission denied"
        strcpy(result, STR_PERMISSION_DENIED);
    }

    return &result;
}

// RPC service function to approve a request token.
char **approve_request_token_1_svc(char **argp, struct svc_req *rqstp)
{
    static char *result;
	// Allocate memory for the result
    ALLOCATE_STRING(result, MEMORY_SIZE);
    bool no_permissions = false;

    for (auto value : approvals)
    {
        const string& user_value = value.first;
        const auto& permissions_value = value.second;

        if (*argp == user_value) {
            if (!strcmp(permissions_value[0].targeted_resource, STR_WILDCARD) &&
                !strcmp(permissions_value[0].granted_permissions, STR_DASH))
            {
                // Loop through the user list to find the user
                for (auto& user : user_list) {
                    // Compare the request_token of each user with the input token
                    if (!strcmp(user.tokens.request_token, *argp)) {
                        // Set the token as invalid since there are no permissions
                        // for this user
                        user.is_token_validated = false;
                        no_permissions = true;
                        break;
                    }
                }
            }
        }
    }
    if (no_permissions == false)
    {
        // Loop through the user list to find the user
        for (auto& user : user_list) {
            // Compare the request_token of each user with the input token
            if (!strcmp(user.tokens.request_token, *argp)) {
                // Set the token as validated since permissions exist
                user.is_token_validated = true;
                break;
            }
        }
    }

	// Initialize result with the request token
    INITIALIZE_STRING(result, *argp);

    return &result;
}

// RPC service function to check the validity of a token.
int *check_token_validity_1_svc(char **argp, struct svc_req *rqstp)
{
    static thread_local int result = -1;

    result = -1;
    for (const auto& user : user_list)
    {
        if (!strcmp(user.user_identifier, *argp))
        {
			// Set result to the token validity seconds
            result = user.tokens.token_validity_operations;
            break;
        }
    }

    return &result;
}