#include <zune/tagitem.h>

void RefreshTagItemClones(TagItem *clone, TagItem *original)
{
   TagItem *tmp_clone = clone;
   TagItem *tmp_list = original;
   TagItem *ti_tmp;

   if (!clone || !original)
      return;
   while ((ti_tmp = NextTagItem(&tmp_list)) != NULL)
   {
      tmp_clone->ti_Tag  = ti_tmp->ti_Tag;
      tmp_clone->ti_Data = ti_tmp->ti_Data;
      tmp_clone++;
   }
   return;
}
