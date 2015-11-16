#ifndef __Z_STRING_SET_H__
#define __Z_STRING_SET_H__

#include <glib.h>
#include <stdio.h>

struct ZStringSet;
typedef struct ZStringSet ZStringSet;

ZStringSet *z_string_set_new(void);
void z_string_set_destroy(ZStringSet *set);
void z_string_set_add(ZStringSet *set, gchar *str);
void z_string_set_dump(ZStringSet *set, FILE *out);

#endif
