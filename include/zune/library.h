#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include <zune/amitypes.h>
#include <zune/amilist.h>
#include <gmodule.h>

struct Library {
/*---- traditional stuff ----*/
    struct Node  lib_Node;
/*---- zune stuff ----*/
    GModule     *lib_GModule;
};

#endif
