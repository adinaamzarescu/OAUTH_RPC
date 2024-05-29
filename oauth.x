/* Enum for standard response types */
enum response_type {
    RESPONSE_SUCCESS = 0,
    RESPONSE_FAILURE = 1
};

/* Enum for RPC method identifiers */
enum rpc_method {
    METHOD_REQUEST_AUTHORIZATION = 1,
    METHOD_REQUEST_ACCESS_TOKEN = 2,
    METHOD_VALIDATE_DELEGATED_ACTION = 3,
    METHOD_APPROVE_REQUEST_TOKEN = 4,
    METHOD_CHECK_TOKEN_VALIDITY = 5
};

/* Structure for holding all token-related information */
struct token_details {
    string request_token<>;
    string access_token<>;
    string refresh_token<>;
    int token_validity_operations;
    string token_error_message<>;
};

/* Structure to hold user and token information during refresh */
struct token_refresh_request {
    string user_identifier<>;
    string request_token<>;
    bool refresh_token;
    bool initiate_refresh;
};

/* Structure for describing operations on resources */
struct operation_details {
    string type_of_operation<>;
    string targeted_resource<>;
    string access_token<>;
};

/* Structure for resource permissions */
struct permissions_detail {
    string targeted_resource<>;
    string granted_permissions<>;
};

/* Structure representing a user with associated token information */
struct user_credentials {
    string user_identifier<>;
    struct token_details tokens;
    bool is_token_validated;
};

/* RPC program definition for OAUTH */
program OAUTH {
    version OAUTHVERS {
        /* Check the validity of user's access and refresh tokens */
        int check_token_validity(string user_identifier<>) = METHOD_CHECK_TOKEN_VALIDITY;
        
        /* Request authorization for a user */
        string request_authorization(string user_identifier<>) = METHOD_REQUEST_AUTHORIZATION;
        
        /* Approve and validate the request token for further use */
        string approve_request_token(string access_token<>) = METHOD_APPROVE_REQUEST_TOKEN;
        
        /* Request access and refresh tokens using user and request token data */
        struct token_details request_access_token(struct token_refresh_request user_data) = METHOD_REQUEST_ACCESS_TOKEN;
        
        /* Validate a user's action on a resource based on their token */
        string validate_delegated_action(struct operation_details resource_operation) = METHOD_VALIDATE_DELEGATED_ACTION;
    } = 1;
} = 0x11111111;
