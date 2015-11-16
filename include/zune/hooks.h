#ifndef UTILITY_HOOKS_H
#define UTILITY_HOOKS_H

#include <zune/amitypes.h>
#include <zune/amilist.h>

struct Hook
{
    struct Node h_Node;
    ULONG	   (*h_Entry)(); /*(struct Hook *hook, APTR obj, APTR msg);*/
    APTR	       h_Data;		/* owner specific	 */
};

#define h_MinNode h_Node

/* Useful definition for casting function pointers:
 * hook.h_SubEntry = (HOOKFUNC)AFunction
 */
typedef unsigned long (*HOOKFUNC)();

/* Hook calling conventions.
 *
 * The standard C receiving code is:
 *
 *    HookFunc(struct Hook *hook, APTR object, APTR message)
 *
 */


#endif /* UTILITY_HOOKS_H */
