#ifndef __UPROPERTIES_H__
#define __UPROPERTIES_H__

#include <zune/amitypes.h>
#include <stdio.h>
#include <stringset.h>

/* Smell like java.util.Properties.
 * Store/retrieve key/value pairs, with file I/O
 */

struct UProperties;

typedef struct UProperties UProperties;

/* creation */
UProperties *uprop_new (ZStringSet *comments);
/* destroy obj, and free all keys and values */
void uprop_destroy (UProperties *prop);

/* file I/O */
BOOL uprop_load (UProperties *prop, FILE *infile);
BOOL uprop_save (UProperties *prop, FILE *outfile, const STRPTR header);

/* access */
const STRPTR uprop_get_property (UProperties *prop, const STRPTR key);
const STRPTR uprop_get_property_with_defaults (UProperties *prop,
					       const STRPTR key,
					       const STRPTR def_val);
/* note: both key and value are copied */
void uprop_set_property (UProperties *prop, const STRPTR key,
			 const STRPTR value);

/* note: try to init *res with your default value */
BOOL uprop_get_property_uint (UProperties *prop, const STRPTR key, guint *res);
BOOL uprop_get_property_int (UProperties *prop, const STRPTR key, gint *res);
BOOL uprop_get_property_double (UProperties *prop, const STRPTR key, gdouble *res);
BOOL uprop_get_property_float (UProperties *prop, const STRPTR key, gfloat *res);


void uprop_set_property_uint (UProperties *prop, const STRPTR key, guint value);
void uprop_set_property_int (UProperties *prop, const STRPTR key, gint value);
void uprop_set_property_double (UProperties *prop, const STRPTR key, gdouble value);
void uprop_set_property_float (UProperties *prop, const STRPTR key, gfloat value);

#endif
