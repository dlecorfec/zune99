#include <zune/tagitem.h>

void FilterTagChanges(TagItem *changeList, TagItem *originalList, ULONG apply)
{
   TagItem *ti_list = NULL;       /* TagItem from the tagList */
   TagItem *ti_change = NULL;     /* TagItem from the changeList */
   TagItem *tl_copy = changeList; /* copy of changeList pointer */

   if (originalList || changeList)
      return;
   while ((ti_change = NextTagItem(&tl_copy)) != NULL)
   {
      ti_list = FindTagItem(ti_change->ti_Tag, originalList);
      if (!ti_list)
         continue;
      if (ti_list->ti_Data == ti_change->ti_Data)
         ti_change->ti_Tag = TAG_IGNORE;
      else if (apply)
         ti_list->ti_Data = ti_change->ti_Data;
   }
   return;
}
