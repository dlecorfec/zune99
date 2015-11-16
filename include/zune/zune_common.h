#ifndef __ZUNE_COMMON_H__
#define __ZUNE_COMMON_H__

#include <zune/classes.h>
#include <gdk/gdktypes.h>

#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright (obj)) \
                          && _between(_mtop(obj) ,(y),_mbottom(obj)))

/* add mask in flags if tag is true, else sub mask */
#define _handle_bool_tag(flags, tag, mask) \
((tag) ? ((flags) |= (mask)) : ((flags) &= ~(mask)))



#ifdef __cplusplus
extern "C" {
#endif

/* Set foreground color */
void SetAPen (GdkGC *gc, gulong pixel);

/* Set background color */
void SetBPen (GdkGC *gc, gulong pixel);


/* call SimpleButton macro and set CycleChain */
Object *SimpleChainedButton (STRPTR label);

/* create MUIO_Checkmark and set CycleChain */
Object *ChainedCheckmark (STRPTR label);

#ifdef __cplusplus
}
#endif


#define SIGBREAKF_CTRL_C (1<<0)

#endif /* __ZUNE_COMMON_H__ */
