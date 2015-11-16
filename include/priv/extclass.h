#ifndef __EXTCLASS_H__
#define __EXTCLASS_H__

#include <zune/classes.h>

/* About loading Zune classes in external files ...
 */

typedef ULONG (*QueryFunc) (LONG which);

Class *_zune_class_load(ClassID classID);



#endif
