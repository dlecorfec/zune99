#ifndef __ROOTCLASS_H__
#define __ROOTCLASS_H__

#include <zune/classes.h>

/* Dispatched method ID's
 * NOTE: Applications should use Intuition entry points, not direct
 * DoMethod() calls, for NewObject, DisposeObject, SetAttrs,
 * SetGadgetAttrs, and GetAttr.
 */

#define OM_Dummy	(0x100)
#define OM_NEW		(0x101)	/* 'object' parameter is "true class"	*/
#define OM_DISPOSE	(0x102)	/* delete self (no parameters)		*/
#define OM_SET		(0x103)	/* set attributes (in tag list)		*/
#define OM_GET		(0x104)	/* return single attribute value	*/
#define OM_ADDTAIL	(0x105)	/* add self to a List (let root do it)	*/
#define OM_REMOVE	(0x106)	/* remove self from list		*/
#define OM_NOTIFY	(0x107)	/* send to self: notify dependents	*/
#define OM_UPDATE	(0x108)	/* notification message from somebody	*/
#define OM_ADDMEMBER	(0x109)	/* used by various classes with lists	*/
#define OM_REMMEMBER	(0x10A)	/* used by various classes with lists	*/

/* Parameter "Messages" passed to methods	*/

/* OM_NEW and OM_SET	*/
struct opSet {
    ULONG		MethodID;
    struct TagItem	*ops_AttrList;	/* new attributes	*/
};

/* OM_NOTIFY, and OM_UPDATE	*/
struct opUpdate {
    ULONG		MethodID;
    struct TagItem	*opu_AttrList;	/* new attributes	*/
};

/* OM_GET	*/
struct opGet {
    ULONG		MethodID;
    ULONG		opg_AttrID;
    ULONG		*opg_Storage;	/* may be other types, but "int"
					 * types are all ULONG
					 */
};

/* OM_ADDTAIL	*/
struct opAddTail {
    ULONG		MethodID;
    struct List		*opat_List;
};

/* OM_ADDMEMBER, OM_REMMEMBER	*/
#define  opAddMember opMember
struct opMember {
    ULONG		MethodID;
    Object		*opam_Object;
};


#endif
