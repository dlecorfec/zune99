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
#include <uproperties.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UProperties {
    GTree        *collection;
    GStringChunk *strings;
    ZStringSet   *comments;
};

#define KEYVALUE 1
/* when a ! comment is read and must be kept */
#define COMMENT 2

/* The properties parser is ugly, but can you make it simpler ?
 * Anyway, excepted the parser, it seems correct.
 */

static void
write_escaped_string (STRPTR s, FILE *outfile)
{
    char c;

    while ((c = *s++))
    {
	switch(c)
	{
	case ' ':
	case ':':
	case '!':
	case '#':
	case '=':
	    fprintf(outfile, "\\%c", c);
	    break;
	case '\n':
	    fputs("\\n", outfile);
	    break;
	case '\\':
	    fputs("\\\\", outfile);
	    break;
	case '\r':
	    fputs("\\r", outfile);
	    break;
	case '\t':
	    fputs("\\t", outfile);
	    break;
	default :
	    if (isprint(c))
		fputc(c, outfile);
	    else
		fprintf(outfile, "\\%03ho", c);
	}
    }
}

/* 
 * The value field doesn't need to as escaped as the key,
 * because we will read from the first separator up to end of line
 * without bothering with subsequent separators.
 * To get a value as escaped as a key, replace call to this function
 * with call to write_escaped_string
 */
static void
write_escaped_value (STRPTR s, FILE *outfile)
{
    char c;

    while ((c = *s++))
    {
	switch(c)
	{
	case '\n':
	    fputs("\\n", outfile);
	    break;
	case '\\':
	    fputs("\\\\", outfile);
	    break;
	case '\r':
	    fputs("\\r", outfile);
	    break;
	case '\t':
	    fputs("\\t", outfile);
	    break;
	default :
	    if (isprint(c))
		fputc(c, outfile);
	    else
		fprintf(outfile, "\\%03ho", c);
	}
    }
}

static gint
output_key_value (gpointer keyp,
		  gpointer valuep,
		  gpointer user_data)
{
    FILE *outfile = (FILE *)user_data;
    STRPTR key = (STRPTR)keyp;
    STRPTR value = (STRPTR)valuep;
    write_escaped_string(key, outfile);
    fputc('=', outfile);
    write_escaped_value(value, outfile);
    fputc('\n', outfile);
    return FALSE;
}

/*
 * parser : skip white spaces
 */
static void
skip_white_space(FILE *infile)
{
    int c;

    while ((c = fgetc(infile)) != EOF)
    {
	if (c == ' ' || c == '\t')
	    continue;
	else
	{
	    ungetc(c, infile);
	    break;
	}
    }
}

/*
 * parser : go to beginning of next line
 */
static void
next_line(FILE *infile)
{
    int c;
    BOOL reached = FALSE;

    while ((c = fgetc(infile)) != EOF)
    {
	if (c != '\n' && c != '\r')
	{
	    if (reached)
	    {
		ungetc(c, infile);
		break;
	    }
	    else
		continue;
	}
	else
	{
	    reached = TRUE;
	}
    }
}

/*
 * parser : remove escape sequences from string
 */
static STRPTR
cleanup_string (GString *mystring)
{
    gchar *s = mystring->str;
    GString *new_string = g_string_sized_new(mystring->len);
    int c;

    while ((c = *s++))
    {
	if (c != '\\')
	{
	    g_string_append_c(new_string, c);
	    continue;
	}

	switch((c = *s++))
	{
	case 0:
	    goto end;
	case ' ':
	case ':':
	case '!':
	case '#':
	case '=':
	case '\\':
	    g_string_append_c(new_string, c);
	    break;
	case 'n':
	    g_string_append_c(new_string, '\n');
	    break;
	case 'r':
	    g_string_append_c(new_string, '\r');
	    break;
	case 't':
	    g_string_append_c(new_string, '\t');
	    break;
	case '\n':
	    while ((c = *s++))
	    {
		if (!isspace(c))
		{
		    --s;
		    break;
		}   
	    }
	    if (c == 0)
		goto end;
	    break;
	default :
	    if (isdigit(c))
	    {
		static char num[4];
		gulong res;

		num[0] = c;
		num[1] = *s++;
		if (!num[1])
		    goto end;
		num[2] = *s++;
		if (!num[2])
		    goto end;
		num[3] = 0;
		res = strtoul(num, NULL, 8);
		if (res != ULONG_MAX)
		    g_string_append_c(new_string, res);
	    }
	    else
		g_string_append_c(new_string, c);
	}
    }
 end:
    g_string_free(mystring, TRUE);
    s = new_string->str;
    g_string_free(new_string, FALSE);
    return s;
}


/*
 * parser : break a line into key/value
 */
static int
parse_line (FILE *infile, STRPTR *kp, STRPTR *vp)
{
    char comment[1000];
    int c;
    GString *key;
    GString *value;

    g_return_val_if_fail(infile && kp && vp, FALSE);

    skip_white_space(infile);
    c = fgetc(infile);
    switch (c)
    {
    case EOF:
    case '\n':
    case '\r':
    case '#':
	return FALSE;
    case '!':
	ungetc(c, infile);
	*kp = g_strdup(fgets(comment, 1000, infile));
	if ((*kp)[strlen(*kp) - 1] == '\n')
	{
	    (*kp)[strlen(*kp) - 1] = 0;
	    ungetc('\n', infile);
	}
	return COMMENT;
    default:
	ungetc(c, infile);
    }

    key = g_string_sized_new(16);
    value = g_string_sized_new(16);

    /* read key */
    while ((c = fgetc(infile)) != EOF)
    {
	if (c == '\\')
	{
	    switch (c = fgetc(infile))
	    {
	    case EOF:
		break;
	    default:
		g_string_append_c(key, '\\');
		g_string_append_c(key, c);
		break;
	    }
	}
	else if (c == ':' || c == '=' || isspace(c))
	{
	    ungetc(c, infile);
	    break;
	}
	else
	{
	    g_string_append_c(key, c);
	}
    }

    switch (c)
    {
    case EOF:
	g_string_free(key, TRUE);
	g_string_free(value, TRUE);
	return FALSE;
    case '\n':
    case '\r':
	goto success_exit;
    case ':':
    case '=':
	fgetc(infile);
	break;
    }

    /* skip white space before value */
    skip_white_space(infile);
    c = fgetc(infile);
    if (c == '=' || c == ':')
	skip_white_space(infile);
    else
	ungetc(c, infile);

    /* read value */
    while ((c = fgetc(infile)) != EOF)
    {
	if (c == '\\')
	{
	    switch (c = fgetc(infile))
	    {
	    case EOF:
		break;
	    default:
		g_string_append_c(value, '\\');
		g_string_append_c(value, c);
		break;
	    }
	}
	else if (c == '\n' || c == '\r')
	{
	    ungetc(c, infile);
	    break;
	}
	else
	{
	    g_string_append_c(value, c);
	}
    }

 success_exit:
    *kp = cleanup_string(key);
    *vp = cleanup_string(value);
    return KEYVALUE;
}


/* creation */
/* comments - optional storage for comments */
UProperties *
uprop_new (ZStringSet *comments)
{
    UProperties *prop;
    
    prop = g_new0(UProperties, 1);
    prop->collection = g_tree_new((GCompareFunc)strcmp);
    prop->strings = g_string_chunk_new(64);
    prop->comments = comments;
/*  g_print("comments = %p\n", comments); */
    return prop;
}

/* destroy obj, and free all keys and values */
void 
uprop_destroy (UProperties *prop)
{
    g_tree_destroy(prop->collection);
    g_string_chunk_free(prop->strings);
}

static void
free_parsed_line (STRPTR key, STRPTR value)
{
    if (key)
	g_free(key);
    if (value)
	g_free(value);
}

/* file I/O */
BOOL 
uprop_load (UProperties *prop, FILE *infile)
{
    STRPTR key;
    STRPTR value;

    g_return_val_if_fail(prop && infile, FALSE);

    while (!feof(infile))
    {
	key = NULL;
	value = NULL;
	switch (parse_line(infile, &key, &value))
	{
	    case KEYVALUE:
		uprop_set_property(prop, key, value);
		free_parsed_line(key, value);
		break;
	    case COMMENT:
		if (prop->comments)
		{
/*  		    g_print("adding %s\n", key); */
		    z_string_set_add(prop->comments, key);
		}
		free_parsed_line(key, value);
		break;
	}
	next_line(infile);
    }

    return TRUE;
}

BOOL 
uprop_save (UProperties *prop, FILE *outfile, const STRPTR header)
{
    g_return_val_if_fail(prop && outfile, FALSE);

    if (header)
	fprintf(outfile, "# %s\n\n", header);

    g_tree_traverse (prop->collection, output_key_value, G_IN_ORDER, outfile);
    if (prop->comments)
	z_string_set_dump(prop->comments, outfile);
    return TRUE;
}


/* access */
const STRPTR 
uprop_get_property (UProperties *prop, const STRPTR key)
{
    return g_tree_lookup(prop->collection, key);
}

const STRPTR 
uprop_get_property_with_defaults (UProperties *prop, const STRPTR key,
				  const STRPTR def_val)
{
    const STRPTR val = uprop_get_property(prop, key);
    if (!val)
	return def_val;
    return val;
}

/* note: both key and value are copied */
void 
uprop_set_property (UProperties *prop, const STRPTR key, const STRPTR value)
{
    g_tree_insert(prop->collection,
		  g_string_chunk_insert(prop->strings, key),
		  g_string_chunk_insert(prop->strings, value));
}


/* support (return ptr to static buffers, OK with uprop_set_property) */
static STRPTR 
ulong_to_string (gulong val)
{
    static char buf[60];

    g_snprintf(buf, 60, "%lu", val);
    return buf;
}

static STRPTR 
long_to_string (glong val)
{
    static char buf[60];
    
    g_snprintf(buf, 60, "%ld", val);
    return buf;
}

static STRPTR 
double_to_string (gdouble val)
{
    static char buf[60];

    g_snprintf(buf, 60, "%g", val);
    return buf;
}

static STRPTR 
float_to_string (gfloat val)
{
    static char buf[60];
    
    g_snprintf(buf, 60, "%g", val);
    return buf;
}

/****/

void 
uprop_set_property_uint (UProperties *prop, const STRPTR key, guint value)
{
    uprop_set_property(prop, key, ulong_to_string(value));
}

void 
uprop_set_property_int (UProperties *prop, const STRPTR key, gint value)
{
    uprop_set_property(prop, key, long_to_string(value));
}

void 
uprop_set_property_double (UProperties *prop, const STRPTR key, gdouble value)
{
    uprop_set_property(prop, key, double_to_string(value));
}

void 
uprop_set_property_float (UProperties *prop, const STRPTR key, gfloat value)
{
    uprop_set_property(prop, key, float_to_string(value));
}


/****/


BOOL
uprop_get_property_uint (UProperties *prop, const STRPTR key, guint *res)
{
    const STRPTR s = uprop_get_property(prop, key);
    if (s)
    {
	gulong tmp = (gulong)strtoul(s, NULL, 10);
	*res = tmp;
	return TRUE;
    }
    return FALSE;
}

BOOL
uprop_get_property_int (UProperties *prop, const STRPTR key, gint *res)
{
    const STRPTR s = uprop_get_property(prop, key);
    if (s)
    {
	glong tmp = (glong)strtol(s, NULL, 10);
	*res = tmp;
	return TRUE;
    }
    return FALSE;
}

BOOL
uprop_get_property_double (UProperties *prop, const STRPTR key, gdouble *res)
{
    const STRPTR s = uprop_get_property(prop, key);
    if (s)
    {
	*res = (gdouble)strtod(s, NULL);
	return TRUE;
    }
    return FALSE;
}

BOOL
uprop_get_property_float (UProperties *prop, const STRPTR key, gfloat *res)
{
    const STRPTR s = uprop_get_property(prop, key);
    if (s)
    {
	*res = strtod(s, NULL);
	return TRUE;
    }
    return FALSE;
}
