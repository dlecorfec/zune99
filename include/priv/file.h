#ifndef __ZUNEFILE_H__
#define __ZUNEFILE_H__

#include <zune/amitypes.h>

/*
 * find the image file corresponding to the given relative path,
 * by searching first into $(HOME)/.zune/images/, then in
 * ZUNEDATADIR/images/
 */
STRPTR
__zune_file_find_image(STRPTR relpath);

STRPTR
__zune_file_get_data_dir(void);

STRPTR
__zune_file_get_lib_dir(void);

STRPTR
__zune_file_get_user_dir(void);

STRPTR
__zune_file_get_user_image_dir(void);

STRPTR
__zune_file_get_user_libs_dir(void);

STRPTR
__zune_file_get_sys_image_dir(void);

BOOL
__zune_file_exists_regular(STRPTR path);

STRPTR
__zune_file_get_sys_prefs_global_file(void);

STRPTR
__zune_file_get_sys_prefs_app_file(STRPTR app_title);

STRPTR
__zune_file_get_user_prefs_global_file(void);

STRPTR
__zune_file_get_user_prefs_app_file(STRPTR app_title);

#endif
