#ifndef __MINMAX_H__
#define __MINMAX_H__

#include <zune/amitypes.h>

struct MUI_MinMax
{
	WORD MinWidth;
	WORD MinHeight;
	WORD MaxWidth;
	WORD MaxHeight;
	WORD DefWidth;
	WORD DefHeight;
};

#define MUI_MAXMAX 10000 /* use this if a dimension is not limited. */

#endif

