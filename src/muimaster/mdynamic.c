#include <config.h>
#include <zunepriv.h>
#include <gmodule.h>
#include <file.h>
#include <extclass.h>
#include <mcc.h>

#define QUERY_MCC 0
#define QUERY_MCP 1

/* Classes are searched first in user dir, then in system dir.
 * After all, if they want to force you to use system classes first,
 * they can create a forbidden (chmod 000) user libs dir ?
 * Anyway ...
 * Other search paths are welcome.
 */
Class *
_zune_class_load(ClassID classID)
{
    GModule *module;
    STRPTR modpath;
    QueryFunc query;
    struct MUI_CustomClass *mcc;

    if (!g_module_supported())
	return NULL;

    
    modpath = g_module_build_path(__zune_file_get_user_libs_dir(), classID);
g_print("loading %s in %s\n", classID, modpath);
    module = g_module_open (modpath, 0);
    g_free(modpath);
    if (!module)
    {
	modpath = g_module_build_path(__zune_file_get_lib_dir(), classID);
g_print("loading %s in %s\n", classID, modpath);
	module = g_module_open (modpath, 0);
	g_free(modpath);
	if (!module)
	{
	    g_warning("_zune_class_load : can't load %s\n", classID);
	    return NULL;
	}
    }
/*  g_print("module opened !"); */
    if (!g_module_symbol(module, "MCC_Query", (gpointer *)&query))
    {
	g_warning("_zune_class_load : %s\n", g_module_error());
	g_module_close(module);
	return NULL;
    }

    mcc = (struct MUI_CustomClass *)(*query)(QUERY_MCC);
    if (!mcc)
    {
	g_warning("_zune_class_load : dynamic creation of class %s failed\n", classID);
	g_module_close(module);
	return NULL;
    }
    mcc->mcc_Module = module;
    return mcc->mcc_Class;
}
