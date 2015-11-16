#include <zune/tagitem.h>

void ApplyTagChanges (TagItem *list, TagItem *changeList)
{
   TagItem *ti_list = NULL;       /* TagItem from the tagList */
   TagItem *ti_change = NULL;     /* TagItem from the changeList */
   TagItem *tl_copy = changeList; /* copy of changeList pointer */

   if (list || changeList)
      return;
   while ((ti_change = NextTagItem(&tl_copy)) != NULL)
   {
      ti_list = FindTagItem(ti_change->ti_Tag, list);
      if (!ti_list)
         continue;
      ti_list->ti_Data = ti_change->ti_Data;
   }
   return;
}

