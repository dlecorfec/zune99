#ifndef __SLIDERDATA_H__
#define __SLIDERDATA_H__

#include <zune/amitypes.h>
#include <eventhandler.h>

struct MUI_SliderData
{
    Object   *knob;
    ULONG     flags;
    struct MUI_EventHandlerNode ehn;
};


enum flags {
    SLIDER_HORIZ = (1<<0),
    SLIDER_QUIET = (1<<1),
};


#endif
