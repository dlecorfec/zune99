/* Zune -- a free Magic User Interface implementation
 * Copyright (C) 1999 David Le Corfec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <zune/boopsi.h>
#include <zunepriv.h>
#include <extclass.h>
#include <builtin.h>
#include <mcc.h>

static GMemChunk *mccMemChunk = NULL;

#define CHECK_INIT_MCC \
if (!mccMemChunk) \
{ \
    init_mcc(); \
}

static void destroy_mcc ()
{
    g_mem_chunk_destroy(mccMemChunk);
}

static void init_mcc ()
{
   mccMemChunk = g_mem_chunk_create(struct MUI_CustomClass, 10, G_ALLOC_AND_FREE);    
   g_atexit(destroy_mcc);
}


/* An obsolete function in the API, but very helpful for internals !
 */
struct IClass *
MUI_GetClass(char *classID)
{
    Class *cl = NULL;

    g_return_val_if_fail(classID != NULL, NULL);

    cl = GetPublicClass(classID);
    if (!cl)
    {
	cl = _zune_builtin_class_create(classID);
	if (!cl)
	{
	    cl = _zune_class_load(classID);
	}
    }
    return cl;
}

/* To create a class, it may be needed to create the superclass,
 * thus possible indirect recursive calls via MUI_GetClass(supername).
 * So avoid using static storage in all public, builtin and dynamic
 * class creation (yes I was trapped :)
 */
struct MUI_CustomClass *
MUI_CreateCustomClass(
    struct Library         *base,       /* for public classes */
    char                   *supername,  /* to inherit from public mcc */
    struct MUI_CustomClass *supermcc,   /* to inherit from private mcc */
    int                     datasize,   /* class data size */
    APTR                    dispatcher) /* class dispatcher */
{
    struct MUI_CustomClass *mcc;
    Class                  *super;
    char                   *id = NULL;

    g_return_val_if_fail((supername != NULL) || (supermcc != NULL), NULL);

    if (!supermcc)
    {
	super = MUI_GetClass(supername);
	if (!super)
	    return NULL;
    }
    else
	super = supermcc->mcc_Class;

    CHECK_INIT_MCC;
    mcc = g_chunk_new0(struct MUI_CustomClass, mccMemChunk);
    if (!mcc)
	return NULL;

    if (base)
	id = ((struct Node *)base)->ln_Name;

    mcc->mcc_Class = MakeClass(id, NULL, super, datasize, 0);
    if (!mcc->mcc_Class)
    {
	g_chunk_free(mcc, mccMemChunk);
	return NULL;
    }

    mcc->mcc_Super = super;
    mcc->mcc_Class->cl_Dispatcher.h_Entry = dispatcher;
    if (id)
    {
/*  	g_print("MUI_CreateCustomClass : Addclass %s\n", id); */
	AddClass(mcc->mcc_Class);
    }
/*      else */
/*      { */
/*  	g_print("MUI_CreateCustomClass : do not AddClass %p\n", mcc->mcc_Class); */
/*      } */
/*  g_print("MUI_CreateCustomClass returns %p\n\n", mcc); */
    return mcc;
}


BOOL
MUI_DeleteCustomClass(struct MUI_CustomClass *mcc)
{
    if (!mcc
	|| (mcc->mcc_Class->cl_SubclassCount > 0)
	|| (mcc->mcc_Class->cl_ObjectCount > 0)
	|| (!FreeClass(mcc->mcc_Class)))
    {
	return FALSE;
    }
    g_chunk_free(mcc, mccMemChunk);
    return TRUE;
}
