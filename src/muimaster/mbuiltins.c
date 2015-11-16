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
#include <zunepriv.h>
#include <builtin.h>
#include <mcc.h>

static const struct __MUIBuiltinClass *builtins[] =
{
    &_MUI_Notify_desc,
    &_MUI_Family_desc,
    &_MUI_Application_desc,
    &_MUI_Window_desc,
    &_MUI_Area_desc,
    &_MUI_Rectangle_desc,
    &_MUI_Group_desc,
    &_MUI_Image_desc,
    &_MUI_Dataspace_desc,
    &_MUI_Configdata_desc,
    &_MUI_Text_desc,
    &_MUI_Numeric_desc,
    &_MUI_Slider_desc,
};

static int num_builtins = sizeof(builtins) / sizeof(struct __MUIBuiltinClass *);

/*
 * Given the index in builtin classes, construct the
 * class and make it public (because of the fake lib base).
 */
static Class *
builtin_to_public_class(ULONG idx)
{
    const struct __MUIBuiltinClass *desc = builtins[idx];
    Class *cl;
    Class *super;

    super = MUI_GetClass(desc->supername);
    if (!super)
	return NULL;
    cl = MakeClass(desc->name, NULL, super, desc->datasize, 0);
    if (!cl)
	return NULL;
    cl->cl_Dispatcher.h_Entry = desc->dispatcher;
    AddClass(cl);
    return cl;
}


/*
 * Create a builtin class and all its superclasses.
 */
Class *
_zune_builtin_class_create (char *classname)
{
    int i;

    for (i = 0 ; i < num_builtins ; i++)
    {
	const struct __MUIBuiltinClass *builtin = builtins[i];

	/* found the class to create */
	if (!strcmp(builtin->name, classname))
	{
	    /* rootclass is our parent */
	    if (!strcmp(builtin->supername, ROOTCLASS))
		_zune_add_root_class();

	    return builtin_to_public_class(i);
	}
    }
    return NULL;
}
