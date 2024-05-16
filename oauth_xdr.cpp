/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "oauth.h"

bool_t
xdr_response_type (XDR *xdrs, response_type *objp)
{
	int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_rpc_method (XDR *xdrs, rpc_method *objp)
{
	int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_token_details (XDR *xdrs, token_details *objp)
{
	int32_t *buf;

	 if (!xdr_string (xdrs, &objp->request_token, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->access_token, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->refresh_token, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->token_validity_seconds))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->token_error_message, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_token_refresh_request (XDR *xdrs, token_refresh_request *objp)
{
	int32_t *buf;

	 if (!xdr_string (xdrs, &objp->user_identifier, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->request_token, ~0))
		 return FALSE;
	 if (!xdr_bool (xdrs, &objp->refresh_token))
		 return FALSE;
	 if (!xdr_bool (xdrs, &objp->initiate_refresh))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_operation_details (XDR *xdrs, operation_details *objp)
{
	int32_t *buf;

	 if (!xdr_string (xdrs, &objp->type_of_operation, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->targeted_resource, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->access_token, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_permissions_detail (XDR *xdrs, permissions_detail *objp)
{
	int32_t *buf;

	 if (!xdr_string (xdrs, &objp->targeted_resource, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->granted_permissions, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_user_credentials (XDR *xdrs, user_credentials *objp)
{
	int32_t *buf;

	 if (!xdr_string (xdrs, &objp->user_identifier, ~0))
		 return FALSE;
	 if (!xdr_token_details (xdrs, &objp->tokens))
		 return FALSE;
	 if (!xdr_bool (xdrs, &objp->is_token_validated))
		 return FALSE;
	return TRUE;
}
