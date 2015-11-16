#ifndef __BOOPSI_H__
#define __BOOPSI_H__

#include <zune/classes.h>
#include <zune/rootclass.h>
#include <zune/tagitem.h>

#define _U(p) GPOINTER_TO_UINT(p)

#ifdef __cplusplus
extern "C" {
#endif

Object *NewObjectA (struct IClass *classPtr, ClassID classID, struct TagItem *tagList);
void DisposeObject (Object *object);
ULONG SetAttrsA (Object *object, struct TagItem *tagList);

ULONG GetAttr (ULONG attrID, Object *object, ULONG *storagePtr);

Object *NextObject (Object **objectPtrPtr);
struct IClass *MakeClass (ClassID classID, ClassID superClassID,
			  struct IClass *superClassPtr, ULONG instanceSize,
			  ULONG flags);
void AddClass (struct IClass *classPtr);
BOOL FreeClass (struct IClass *classPtr);
void RemoveClass (struct IClass *classPtr);
Class *GetPublicClass (const STRPTR className); /* Zune v1 */

ULONG DoMethodA (Object *obj, Msg message);
ULONG DoSuperMethodA (struct IClass *cl, Object *obj, Msg message);
ULONG CoerceMethodA (struct IClass *cl, Object *obj, Msg message);
ULONG xget (Object *obj, ULONG attr);

#ifdef __GNUC__

typedef ULONG arg_type; /* to store a message member */
typedef ULONG tag_type; /* to store a taglist member */

#define DoSuperMethod(cl, obj, tags...) \
({arg_type _args[] = { tags }; DoSuperMethodA(cl, obj, (Msg)_args); })
#define DoMethod(obj, tags...) \
({arg_type _args[] = { tags }; DoMethodA(obj, (Msg)_args); })
#define CoerceMethod(cl, obj, tags...) \
({arg_type _args[] = { tags }; CoerceMethodA (cl, obj, (Msg)_args); })
#define DoSuperNew(cl, obj, tags...) \
({tag_type _tags_[] = { tags }; arg_type _args[] = { OM_NEW, (arg_type)_tags_ };\
 DoSuperMethodA(cl, obj, (Msg)_args); })

#define NewObject(clptr, clid, tags...) \
({tag_type _tags[] = { tags }; NewObjectA(clptr, clid, (struct TagItem *)_tags); })
#define SetAttrs(obj, tags...) \
({tag_type _tags[] = { tags }; SetAttrsA(obj, (struct TagItem *)_tags); })

#else /* ! __GNUC__ */
ULONG DoMethod (Object *obj, ULONG MethodID, ...);
ULONG DoSuperMethod (struct IClass *cl, Object *obj, ULONG MethodID, ...);
ULONG CoerceMethod (struct IClass *cl, Object *obj, ULONG MethodID, ...);
Object *NewObject (struct IClass *classPtr, ClassID classID, ULONG tag1, ...);
ULONG SetAttrs (Object *object, ULONG tag1, ...);
ULONG DoSuperNew (struct IClass *cl, Object *obj, ULONG tag1, ...);
#endif

#ifdef __cplusplus
}
#endif


#endif /* __BOOPSI_H__ */
