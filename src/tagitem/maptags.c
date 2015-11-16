#include <zune/tagitem.h>

void MapTags(TagItem *tagList, TagItem *mapList, ULONG mapType)
{
   TagItem *ti_list;
   TagItem *ti_map;
   TagItem *tmp_list = tagList;

   if (!tagList)
      return;

   while ((ti_list = NextTagItem(&tmp_list)) != NULL)
   {
      ti_map = FindTagItem(ti_list->ti_Tag, mapList);
      if (ti_map)
         ti_list->ti_Tag = (ti_map->ti_Data != TAG_DONE) ? (Tag)ti_map->ti_Data
                                                         : TAG_IGNORE;
      else if (mapType == MAP_REMOVE_NOT_FOUND)
         ti_list->ti_Tag = TAG_IGNORE;
   }
   return;
}
