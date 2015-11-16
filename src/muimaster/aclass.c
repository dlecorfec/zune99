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

/*
 * Some parts come from AROS (www.aros.org)
 * Because of multiple FindName lookups, changed class storage
 * to a hashtable.
 */

static GHashTable *publicClasses = NULL;
static GMemChunk  *iclassMemChunk = NULL;

/* chunk size to devote for a class' object allocator */
#define DEFAULT_PUDDLE_SIZE 2048


#define CHECK_INIT_ICLASS \
if (!iclassMemChunk) \
    { \
	init_iclasses(); \
    }

static void
destroy_iclasses ()
{
    g_hash_table_destroy(publicClasses);
    g_mem_chunk_destroy(iclassMemChunk);
}

static void
init_iclasses ()
{
    publicClasses = g_hash_table_new(g_str_hash, g_str_equal);
    iclassMemChunk = g_mem_chunk_create(Class, 10, G_ALLOC_AND_FREE);
    g_atexit(destroy_iclasses);
}

/*
 * Return a class pointer for a given public class
 */
Class *GetPublicClass (const STRPTR className)
{
    CHECK_INIT_ICLASS;
    return (Class *)g_hash_table_lookup(publicClasses, className);
}


/*
 * Setup an IClass structure.
 */
struct IClass *
MakeClass (ClassID        classID,
	   ClassID        superClassID,
	   struct IClass *superClassPtr,
	   ULONG          instanceSize,
	   ULONG          flags)
{
    Class * iclass;

    g_return_val_if_fail((superClassID != NULL) || (superClassPtr != NULL), NULL);
    CHECK_INIT_ICLASS;
    if (classID)
    {
	/* Does this PUBLIC class already exist ? */
	if (g_hash_table_lookup(publicClasses, classID))
	    return (NULL);
    }

    if (!superClassPtr)
    {
	/* Search for the class ... */
	superClassPtr = GetPublicClass(superClassID);
        if (!superClassPtr)
        {
	    g_warning("MakeClass: cannot create %s : no super class %s\n",
		      classID, superClassID);
	    return (NULL);  /* nothing found */
        }
    }

    /* Get some memory */
    if ((iclass = g_chunk_new0(Class, iclassMemChunk)) != NULL)
    {
	int object_size;
	int puddle_size;

	/* Felder init */
	iclass->cl_Super      = superClassPtr;
	iclass->cl_ID	      = classID;
	iclass->cl_InstOffset = superClassPtr->cl_InstOffset +
	    superClassPtr->cl_InstSize;
	iclass->cl_InstSize   = instanceSize;
	iclass->cl_Flags      = flags;
	object_size = iclass->cl_InstOffset + iclass->cl_InstSize
	    + sizeof (struct _Object);
	puddle_size = MAX(DEFAULT_PUDDLE_SIZE, object_size);
	iclass->cl_MemChunk   = g_mem_chunk_new(classID,
						object_size,
						puddle_size,
						G_ALLOC_AND_FREE);
	iclass->cl_Dispatcher.h_Node.ln_Type = 0;
	iclass->cl_Dispatcher.h_Node.ln_Pri = 0;
	iclass->cl_Dispatcher.h_Node.ln_Name = iclass->cl_ID;

	/* SuperClass is used one more time now */
	superClassPtr->cl_SubclassCount ++;
    }
    return (iclass);
} /* MakeClass */


void
AddClass(struct IClass *classPtr)
{
    g_return_if_fail(classPtr != NULL);
    CHECK_INIT_ICLASS;

    if (!(classPtr->cl_Flags & CLF_INLIST))
    {
	g_hash_table_insert(publicClasses,
			    ((struct Node *)classPtr)->ln_Name, classPtr);

	classPtr->cl_Flags |= CLF_INLIST;
    }
} /* AddClass */


void
RemoveClass (struct IClass *classPtr)
{
    CHECK_INIT_ICLASS;
    if (classPtr && (classPtr->cl_Flags & CLF_INLIST))
    {
	/*Forbid ();*/
	g_hash_table_remove(publicClasses, classPtr->cl_ID);
	/*Permit ();*/

	classPtr->cl_Flags &= ~CLF_INLIST;
    }
} /* RemoveClass */


BOOL
FreeClass (struct IClass *classPtr)
{
   /* Make sure no one creates another object from this class. For private
      classes, this call does nothing. */
   RemoveClass (classPtr);

   if (!classPtr->cl_SubclassCount && !classPtr->cl_ObjectCount)
   {
      classPtr->cl_Super->cl_SubclassCount --;
      g_mem_chunk_destroy(classPtr->cl_MemChunk);
      g_chunk_free(classPtr, iclassMemChunk);

      return (TRUE);
   }

   return (FALSE);
} /* FreeClass */
