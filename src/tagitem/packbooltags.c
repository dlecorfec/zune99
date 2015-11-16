#include <zune/tagitem.h>

ULONG PackBoolTags(ULONG initialFlags, TagItem *tagList, TagItem *boolMap)
{
   ULONG flags = initialFlags;
   TagItem *ti_list = NULL;     /* TagItem from the tagList */
   TagItem *ti_map = NULL;      /* TagItem from the boolMap */
   TagItem *tl_copy = tagList;
   
   if (!tagList)
      return 0;

   while ((ti_list = NextTagItem(&tl_copy)) != NULL)
   {
      ti_map = FindTagItem(ti_list->ti_Tag, boolMap);
      if (ti_map)
         flags = (ti_list->ti_Data == (IPTR)TRUE) ?
                 (flags | (ULONG)ti_map->ti_Data) : (flags & ~(ULONG)ti_map->ti_Data);
   }
   return flags;
}
