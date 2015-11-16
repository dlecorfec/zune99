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
#include <stdlib.h>
#include <stdarg.h>

/*
 * Modified from AROS (www.aros.org)
 */

/*
 * Create an object from a class.
 */
Object *
NewObjectA (struct IClass  *classPtr,
	    ClassID         classID,
	    struct TagItem *tagList)
{
    Object * object;
    struct _Object carrier;

    /* No classPtr ? */
    if (!classPtr)
    {
        /* Search for the class */
        if (!(classPtr = GetPublicClass(classID)) )
        {
	    /* I used to have a last-resort builtin class creation here,
	     * but I decided nobody should need it, because you
	     * dont have to call this function with a non-public class -dlc
	     */
	    g_error("NewObjectA: null classPtr, and non public class %s\n"
		    "Perhaps you should try MUI_NewObjectA ?\n",
		    classID);
	    return (NULL); /* Nothing found */
        }
    }

    /* Put the classPtr in our dummy object */
    carrier.o_Class = classPtr;

    /* Try to create a new object */
    if ((object = (Object *) DoMethod (BASEOBJECT(&carrier), OM_NEW, (ULONG)tagList)))
    {
	OCLASS(object) = classPtr;

	/* One more object */
	classPtr->cl_ObjectCount ++;
    }

    return (object);
} /* NewObjectA */


/*
 * Deletes a 'boopsi' object.
 */
void
DisposeObject (Object *object)
{
    ULONG MethodID = OM_DISPOSE;

    OCLASS (object)->cl_ObjectCount --;
    DoMethodA (object, (Msg)&MethodID);
} /* DisposeObject */


#ifndef __GNUC__
/*
 * Varargs stub for NewObjectA().
 */
Object *
NewObject (struct IClass *classPtr, ClassID classID, ULONG tag1, ...)
{
    APTR    object;

    object = NewObjectA (classPtr, classID, (struct TagItem *)&tag1);
    return object;
} /* NewObject */
#endif /* ! __GNUC__ */
