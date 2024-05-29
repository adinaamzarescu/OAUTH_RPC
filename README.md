# OAUTH_RPC

### How to run

```
make
```

```
./check.sh all
```

# Explanation of `oauth.x`

The `oauth.x` file defines the data structures and RPC program for an OAuth-based authentication and authorization system.

## Enum Definitions

### `enum response_type`
- **Purpose**: Defines standard response types for RPC methods.

### `enum rpc_method`
- **Purpose**: Defines identifiers for the different RPC methods.

## Structure Definitions

### `struct token_details`
- **Purpose**: Holds all token-related information.

### `struct token_refresh_request`
- **Purpose**: Holds user and token information needed during a token refresh request.

### `struct operation_details`
- **Purpose**: Describes operations on resources.

### `struct permissions_detail`
- **Purpose**: Stores resource permissions.

### `struct user_credentials`
- **Purpose**: Represents a user along with their associated token information.

## RPC Program Definition

### `program OAUTH`
- **Purpose**: Defines the RPC program for OAuth with its version and methods.
- **Version**: `OAUTHVERS = 1`
- **Program ID**: `0x11111111`

#### Methods

1. **`int check_token_validity(string user_identifier<>) = METHOD_CHECK_TOKEN_VALIDITY`**
   - Checks the validity of a user's access and refresh tokens.

2. **`string request_authorization(string user_identifier<>) = METHOD_REQUEST_AUTHORIZATION`**
   - Requests authorization for a user.

3. **`string approve_request_token(string access_token<>) = METHOD_APPROVE_REQUEST_TOKEN`**
   - Approves and validates the request token for further use.

4. **`struct token_details request_access_token(struct token_refresh_request user_data) = METHOD_REQUEST_ACCESS_TOKEN`**
   - Requests access and refresh tokens using user and request token data.

5. **`string validate_delegated_action(struct operation_details resource_operation) = METHOD_VALIDATE_DELEGATED_ACTION`**
   - Validates a user's action on a resource based on their token.

# Changes in `oauth.h`

## Global Variables

- **`extern map<string, vector<struct permissions_detail>> approvals`**: Global map storing approval details for each user, where the key is a string and the value is a vector of `permissions_detail` structures.
- **`extern int token_validity_operations`**: Global integer indicating the default validity for tokens (number of operations).
- **`extern vector<string> resources`**: Global vector storing the list of resources.
- **`extern vector<user_credentials> user_list`**: Global vector storing user credentials.
- **`extern ifstream approvals_file`**: Global input file stream used for reading approvals data.

# Server Logic Overview

When a client interacts with the server by making a request the following steps occur:

1. **Receiving Requests**: When a client makes a request, the server receives it at one of its exposed RPC endpoints.
2. **Processing Requests**: Based on the request type, the server calls the corresponding function to process the data. This includes:
   - Validating user credentials
   - Generating tokens
   - Updating permissions
   - Checking permissions for specific operations
3. **Responding to Clients**: Once the server has processed the request, it sends a response back to the client. This response might contain a newly generated token, a validation status, or results of a permission check.

## Key Functions Explained

### `generate_access_token(char *clientIdToken)`
Generates an access token for a client identified by `clientIdToken`.

### `find_user(char *user_id, char **result_token)`
Searches for a user by `user_id`. If found, generates an access token, updates the user's record with this token, and returns `true`. If not found, returns `false`.

### `initialize_permissions_detail(string targeted_resource, string granted_permissions)`
Creates and initializes a `permissions_detail` structure with specified resources and permissions.

### `update_permissions_details(string result_token, string line)`
Parses a string to extract permission details and updates the global `approvals` map that tracks permissions for each token.

### `request_authorization_1_svc(char **argp, struct svc_req *rqstp)`
Handles authorization requests. Attempts to identify the user and generate a token using `find_user`, then updates permission details based on additional provided data.

### `initialize_token_details(struct token_details *details)`
Initializes a `token_details` structure to store tokens and an error message.

### `should_log_refresh_action(bool refresh_token_requested, int token_validity, bool initiate_refresh)`
Determines if a token refresh action should be logged based on conditions like refresh request, token validity, and initiation flag.

### `set_tokens(user_credentials& user, token_details& result, const char* access_token, const char* refresh_token)`
Updates user credentials and result details with access and optionally refresh tokens.

### `request_access_token_1_svc(struct token_refresh_request *argp, struct svc_req *rqstp)`
Processes access token requests, validates the user's token, potentially generates new access and refresh tokens, and logs actions as necessary.

### `is_operation_permitted(const char* requested_operation, const char* targeted_resource, the string& request_token)`
Checks if an operation is permitted based on the targeted resource and the permissions linked with the request token.

### `validate_delegated_action_1_svc(struct operation_details *argp, struct svc_req *rqstp)`
Validates if a delegated action is permissible, considering operation type, targeted resource, and token status.

### `approve_request_token_1_svc(char **argp, struct svc_req *rqstp)`
Manages the approval process of a request token by updating its validation status based on existing permissions.

### `check_token_validity_1_svc(char **argp, struct svc_req *rqstp)`
Checks the validity of a token based on how many seconds it remains valid and updates the result accordingly.


# Client-Side RPC Code Explanation

The client-side code handles user authentication, manages token lifecycles, and performs operations based on tokens.

## Functions Overview

### `allocate_and_copy_string(char **destination, const string &source)`
Allocates memory and copies the source string into the destination. This function is used for handling dynamic string data within the client operations.

### `assign_token_data(int index, const string& token, string& user_identifier, string& type_of_operation, string& targeted_resource)`
Assigns token data to appropriate variables based on the index. This function is used in parsing and organizing data from strings into structured data.

### `handle_user_auth_and_token_refresh(CLIENT *clnt, string user_identifier, bool refresh_token)`
Manages the process of user authentication and token refreshing. It requests authorization, approves the token, and if necessary, requests a new access token using the refresh token logic.

## Main Functional Workflow `oauth_1(char *host, char *input_file)`

This is the main function that manages the client's interactions with the RPC server based on a specified client file. It manages the sequence of operations including:
- Creating the RPC client connection.
- Reading and processing data from the client file.
- Handling user authentication and token refresh based on operation requirements.
- Validating token validity and performing automated refresh if necessary.
- Handling delegated actions based on user permissions.

### Detailed Steps

1. **RPC Client Initialization**: Creates an RPC client that communicates with the server specified by `host`.
2. **File Processing**: Reads from `input_file` and processes each line to extract user identifiers and their respective operation types and targeted resources.
3. **Operation Handling**:
   - For operations requiring user authentication, it manages the flow from requesting authorization to handling token refreshes.
   - For other types of operations, it checks token validity and handles token refreshes if tokens are expired and auto-refresh is enabled.
   - Validates delegated actions to ensure operations are permitted for the given tokens and resources.
4. **Memory Management**: Ensures all dynamically allocated memory is properly freed after operations to prevent memory leaks.

## Error Handling

- Implements robust error handling by checking the success of RPC calls and appropriately responding to failures, such as by freeing allocated memory and logging errors.

## Command-Line Argument Validation `check_arguments(int argc, char* argv[])`

Checks if the correct number of arguments are provided when running the client program, ensuring that both the server host and client file are specified.

## Entry Point `main(int argc, char *argv[])`

- Validates command-line arguments.
- Initializes client operations by passing the host and client file to the `oauth_1` function.
- Manages memory for command-line arguments to prevent leaks.


# Changes in `oauth_svc.cpp`

This part outlines the changes made to the `oauth_svc.cpp` file generated by `rpcgen`.

## Additional Headers and Macros

### New Headers
- `<fstream>`: Included for file stream operations.
- `<vector>`: Included for using vectors.

### New Macros
- `FREE_MEMORY`: Safely frees memory and nullifies the pointer.
- `OPEN_FILE`: Opens a file and checks for errors.
- `CHECK_FILE`: Checks if a file is open, otherwise exits with an error.
- `CHECK_GETLINE`: Checks the success of getting a line from a file, with error handling.
- `CLOSE_FILE`: Closes the file.
- `CHAR_NEWLINE`: Defines the newline character.

## Global Variables and Structures

- `token_validity_operations`: Integer to store token validity (number of operations).
- `resources`: Vector to store resource strings.
- `user_list`: Vector to store user credentials.
- `approvals_file`: File stream for handling approvals.

## Memory Allocation Functions

- `allocate_and_copy_string`: Allocates memory for a string and copies content safely.
- `allocate_and_copy_user_identifier`: Specifically allocates and copies user identifiers with size constraints.
- `allocate_request_token`: Allocates memory for a request token and initializes it.

## Main Function

The main function has been changed to handle command-line arguments, open and process files, and manage memory efficiently. Below are the detailed steps involved in these changes.

#### Command-Line Argument Handling and File Processing

1. **Initialize File Path Variables**:
   - `user_file_path`, `resources_file_path`, and `approvals_file_path` are initialized to `NULL`.

2. **Allocate and Copy Command-Line Arguments**:
   - `allocate_and_copy_string(&user_file_path, argv[1])`: Allocates memory and copies the first command-line argument (userIDs.db) to `user_file_path`.
   - `allocate_and_copy_string(&resources_file_path, argv[2])`: Allocates memory and copies the second command-line argument (resources.db) to `resources_file_path`.
   - `allocate_and_copy_string(&approvals_file_path, argv[3])`: Allocates memory and copies the third command-line argument (approvals.db) to `approvals_file_path`.

3. **Set Token Validity**:
   - `token_validity_operations = stoi(argv[4])`: Converts the fourth command-line argument ($TOKEN_LIFETIME) to an integer and assigns it to `token_validity_operations`.

#### User File Handling

4. **Open User File**:
   - `OPEN_FILE(input_file_1, user_file_path)`: Opens the user file using the `OPEN_FILE` macro and assigns it to `input_file_1`.

5. **Read and Process User Data**:
   - `CHECK_GETLINE(input_file_1, line, 0)`: Reads the first line from the file, which contains the number of users.
   - Converts this line to an integer (`size`), representing the number of users.
   - A loop runs `size` times to read and process each user's data:
     - `CHECK_GETLINE(input_file_1, line, i + 1)`: Reads the user data line by line.
     - `allocate_and_copy_user_identifier(&u.user_identifier, line)`: Allocates memory and copies the user identifier.
     - Checks if `u.user_identifier` is `NULL`, logs an error, and exits if memory allocation fails.
     - `allocate_request_token(&u.tokens.request_token)`: Allocates memory for the request token.
     - Checks if `u.tokens.request_token` is `NULL`, logs an error, frees `u.user_identifier`, and exits if memory allocation fails.
     - Adds the user credentials to the `user_list`.

6. **Close User File**:
   - `CLOSE_FILE(input_file_1)`: Closes the user file using the `CLOSE_FILE` macro.

#### Resource File Handling

7. **Open Resource File**:
   - `OPEN_FILE(input_file_2, resources_file_path)`: Opens the resources file using the `OPEN_FILE` macro and assigns it to `input_file_2`.

8. **Read and Process Resource Data**:
   - `CHECK_GETLINE(input_file_2, line, 0)`: Reads the first line from the file, which contains the number of resources.
   - Converts this line to an integer (`size`), representing the number of resources.
   - A loop runs `size` times to read and process each resource:
     - `CHECK_GETLINE(input_file_2, line, i + 1)`: Reads the resource data line by line.
     - Adds each resource to the `resources` vector.

9. **Close Resource File**:
   - `CLOSE_FILE(input_file_2)`: Closes the resources file using the `CLOSE_FILE` macro.

#### Approvals File Handling

10. **Open Approvals File**:
    - `approvals_file_path.open(approvals_file)`: Opens the approvals file.
    - `CHECK_FILE(approvals_file_path, approvals_file)`: Checks if the file opened successfully.

#### Memory Cleanup

11. **Free Allocated Memory**:
    - `FREE_MEMORY(user_file_path)`: Frees the memory allocated for `user_file_path`.
    - `FREE_MEMORY(resources_file_path)`: Frees the memory allocated for `resources_file_path`.
    - `FREE_MEMORY(approvals_file_path)`: Frees the memory allocated for `approvals_file_path`.


# Makefile Explanation

## Default Targets

- **all**: Builds both the client and server executables.
- **generate**: Special target for generating source files from the RPC definition file.

## Rule for Generating Source Files

- **$(TARGETS)**: Generates .cpp and .h files from .x file using `rpcgen`.
  - `rpcgen $(RPCGENFLAGS) $(SOURCES.x)`: Generates RPC code.
  - `mv -f $(SOURCES.x:%.x=%_svc.c) $(SOURCES.x:%.x=%_svc.cpp)`: Renames generated files to .cpp.
  - `mv -f $(SOURCES.x:%.x=%_clnt.c) $(SOURCES.x:%.x=%_clnt.cpp)`: Renames generated files to .cpp.
  - `mv -f $(SOURCES.x:%.x=%_xdr.c) $(SOURCES.x:%.x=%_xdr.cpp)`: Renames generated files to .cpp.
  - `mv -f $(SOURCES.x:%.x=%_client.c) $(SOURCES.x:%.x=%_client.cpp)`: Renames generated files to .cpp.
  - `mv -f $(SOURCES.x:%.x=%_server.c) $(SOURCES.x:%.x=%_server.cpp)`: Renames generated files to .cpp.
  - `sed -i '' '/register /d' $(TARGETS_CLNT_CPP) $(TARGETS_SVC_CPP) $(TARGETS_XDR_CPP)`: Removes deprecated `register` keyword.

## Compilation and Linking Rules

- **$(OBJECTS_CLNT)** and **$(OBJECTS_SVC)**: Compilation rules for client and server objects.
- **$(CLIENT)** and **$(SERVER)**: Linking rules for client and server executables.

## Clean-Up

- **clean**: Removes object files and executables.


# Other changes made

* check.sh
- SERVER_NAME="oauth_server"
- CLIENT_NAME="oauth_client"

* tests
- added a new line at the end of the expected_output files

### Resources

(Other than the SPRC course)

1. https://users.cs.cf.ac.uk/Dave.Marshall/C/node33.html
2. https://docs-archive.freebsd.org/44doc/psd/23.rpc/paper.pdf
3. [Programming with rpcgen](https://web.archive.org/web/20030404112736/http://techpubs.sgi.com/library/tpl/cgi-bin/getdoc.cgi?coll=0650&db=bks&srch=&fname=/SGI_Developer/IRIX_NetPG/sgi_html/ch05.html)
4. [RPC Programming Guide](https://web.archive.org/web/20031013015230/http://techpubs.sgi.com/library/tpl/cgi-bin/getdoc.cgi?coll=0650&db=bks&srch=&fname=/SGI_Developer/IRIX_NetPG/sgi_html/ch06.html)
