/* -*- c++ -*- (enables emacs c++ mode) */
/*========================================================================

 Copyright (C) 2006-2009 Yves Renard, Julien Pommier.

 This file is a part of GETFEM++

 Getfem++ is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation; either version 2.1 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301,
 USA.

 ========================================================================*/

/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "gfi_rpc.h"

bool_t
xdr_gfi_type_id (XDR *xdrs, gfi_type_id *objp)
{
	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_object_id (XDR *xdrs, gfi_object_id *objp)
{
	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->cid))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_sparse (XDR *xdrs, gfi_sparse *objp)
{
	 if (!xdr_array (xdrs, (char **)&objp->ir.ir_val, (u_int *) &objp->ir.ir_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->jc.jc_val, (u_int *) &objp->jc.jc_len, ~0,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->pr.pr_val, (u_int *) &objp->pr.pr_len, ~0,
		sizeof (double), (xdrproc_t) xdr_double))
		 return FALSE;
         if (!xdr_int (xdrs, &objp->is_complex))
                 return FALSE;
	return TRUE;
}

bool_t
xdr_pgfi_array (XDR *xdrs, pgfi_array *objp)
{
	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct gfi_array), (xdrproc_t) xdr_gfi_array))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_storage (XDR *xdrs, gfi_storage *objp)
{
	 if (!xdr_gfi_type_id (xdrs, &objp->type))
		 return FALSE;
	switch (objp->type) {
	case GFI_INT32:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.data_int32.data_int32_val, (u_int *) &objp->gfi_storage_u.data_int32.data_int32_len, ~0,
			sizeof (int), (xdrproc_t) xdr_int))
			 return FALSE;
		break;
	case GFI_UINT32:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.data_uint32.data_uint32_val, (u_int *) &objp->gfi_storage_u.data_uint32.data_uint32_len, ~0,
			sizeof (u_int), (xdrproc_t) xdr_u_int))
			 return FALSE;
		break;
	case GFI_DOUBLE:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.data_double.data_double_val, (u_int *) &objp->gfi_storage_u.data_double.data_double_len, ~0,
			sizeof (double), (xdrproc_t) xdr_double))
			 return FALSE;
                 if (!xdr_int (xdrs, &objp->gfi_storage_u.data_double.is_complex))
                         return FALSE;
		break;
	case GFI_CHAR:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.data_char.data_char_val, (u_int *) &objp->gfi_storage_u.data_char.data_char_len, ~0,
			sizeof (char), (xdrproc_t) xdr_char))
			 return FALSE;
		break;
	case GFI_CELL:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.data_cell.data_cell_val, (u_int *) &objp->gfi_storage_u.data_cell.data_cell_len, ~0,
			sizeof (pgfi_array), (xdrproc_t) xdr_pgfi_array))
			 return FALSE;
		break;
	case GFI_OBJID:
		 if (!xdr_array (xdrs, (char **)&objp->gfi_storage_u.objid.objid_val, (u_int *) &objp->gfi_storage_u.objid.objid_len, ~0,
			sizeof (gfi_object_id), (xdrproc_t) xdr_gfi_object_id))
			 return FALSE;
		break;
	case GFI_SPARSE:
		 if (!xdr_gfi_sparse (xdrs, &objp->gfi_storage_u.sp))
			 return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

bool_t
xdr_gfi_array (XDR *xdrs, gfi_array *objp)
{
	 if (!xdr_array (xdrs, (char **)&objp->dim.dim_val, (u_int *) &objp->dim.dim_len, ~0,
		sizeof (u_int), (xdrproc_t) xdr_u_int))
		 return FALSE;
	 if (!xdr_gfi_storage (xdrs, &objp->storage))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_array_list (XDR *xdrs, gfi_array_list *objp)
{
	 if (!xdr_array (xdrs, (char **)&objp->arg.arg_val, (u_int *) &objp->arg.arg_len, ~0,
		sizeof (gfi_array), (xdrproc_t) xdr_gfi_array))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_status (XDR *xdrs, gfi_status *objp)
{
	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_gfi_output (XDR *xdrs, gfi_output *objp)
{
	 if (!xdr_gfi_status (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->infomsg, ~0))
	   return FALSE;

	switch (objp->status) {
	case GFI_STATUS_OK:
		 if (!xdr_gfi_array_list (xdrs, &objp->gfi_output_u.output))
			 return FALSE;
		break;
	case GFI_STATUS_ERROR:
		 if (!xdr_string (xdrs, &objp->gfi_output_u.errmsg, ~0))
			 return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

bool_t
xdr_gfmrpc_call_1_argument (XDR *xdrs, gfmrpc_call_1_argument *objp)
{
  if (!xdr_int(xdrs, &objp->config_id))
    return FALSE;
  if (!xdr_string (xdrs, &objp->fname, ~0))
    return FALSE;
  if (!xdr_gfi_array_list (xdrs, &objp->in))
    return FALSE;
  if (!xdr_int (xdrs, &objp->nlhs))
    return FALSE;
  return TRUE;
}
