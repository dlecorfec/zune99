#ifndef __CLASSES_H__
#define __CLASSES_H__

#include <zune/amitypes.h>
#include <zune/amilist.h>
#include <zune/hooks.h>


typedef STRPTR ClassID;
typedef ULONG Object;

typedef struct {
    ULONG MethodID;
} *Msg;

#define ROOTCLASS	"rootclass"


/*******************************************/
/*** "White box" access to struct IClass ***/
/*******************************************/

typedef struct IClass {
/*    struct Node cl_Node;*/
    struct Hook		cl_Dispatcher;  /* hook has a node */
    ULONG		cl_Reserved;	/* must be 0  */
    struct IClass	*cl_Super;
    ClassID		cl_ID;

    /* where within an object is the instance data for this class? */
    UWORD		cl_InstOffset;
    UWORD		cl_InstSize;

    ULONG		cl_UserData;	/* per-class data of your choice */
    ULONG		cl_SubclassCount;
					/* how many direct subclasses?	*/
    ULONG		cl_ObjectCount;
				/* how many objects created of this class? */
    ULONG		cl_Flags;
#define	CLF_INLIST	0x00000001	/* class is in public class list */

    GMemChunk          *cl_MemChunk;    /* glib stuff for object allocation */
} Class;

/* add offset for instance data to an object handle */
#define INST_DATA( cl, o )	((VOID *) (((UBYTE *)o)+cl->cl_InstOffset))

/* sizeof the instance data for a given class */
#define SIZEOF_INSTANCE( cl )	((cl)->cl_InstOffset + (cl)->cl_InstSize \
			+ sizeof (struct _Object ))

/**************************************************/
/*** "White box" access to struct _Object	***/
/**************************************************/

/*
 * We have this, the instance data of the root class, PRECEDING
 * the "object".  This is so that Gadget objects are Gadget pointers,
 * and so on.  If this structure grows, it will always have o_Class
 * at the end, so the macro OCLASS(o) will always have the same
 * offset back from the pointer returned from NewObject().
 *
 * This data structure is subject to change.  Do not use the o_Node
 * embedded structure.
 */
struct _Object {
    struct MinNode	o_Node;
    struct IClass	*o_Class;
};

/* convenient typecast	*/
#define _OBJ( o )	((struct _Object *)(o))

/* get "public" handle on baseclass instance from real beginning of obj data */
#define BASEOBJECT( _obj )	( (Object *) (_OBJ(_obj)+1) )

/* get back to object data struct from public handle */
#define _OBJECT( o )		(_OBJ(o) - 1)

/* get class pointer from an object handle	*/
#define OCLASS( o )	( (_OBJECT(o))->o_Class )

#endif
