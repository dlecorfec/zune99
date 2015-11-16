#ifndef __PROTOS_ADAPTATOR_H__
#define __PROTOS_ADAPTATOR_H__

#ifndef EXEC_TYPES_H
#define EXEC_TYPES_H 1
#endif
#ifndef INTUITION_CLASSES_H
#define INTUITION_CLASSES_H 1
#endif
#ifndef UTILITY_TAGITEM_H
#include <zune/tagitem.h>
#endif

#include <zune/library.h>
#include <zune/classes.h>

struct MUI_RenderInfo;
struct Region;
struct MUI_PenSpec;

#include <zune/muimaster_protos.h>

void MUI_Init (int *argc, char ***argv);

#ifdef __GNUC__

#if 0
#define MUI_NewObject(clid, tags...) \
({ ULONG _tags[] = { tags }; MUI_NewObjectA(clid, (struct TagItem *)_tags); })
#endif

#define MUI_MakeObject(type, tags...) \
({ ULONG _tags[] = { tags }; MUI_MakeObjectA(type, _tags); })

#endif /* __GNUC__ */

#endif /* __PROTOS_ADAPTATOR_H__ */

