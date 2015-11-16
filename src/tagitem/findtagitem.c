#include <zune/tagitem.h>

TagItem *FindTagItem(Tag tagValue, TagItem *tagList)
{
   TagItem *ti = NULL;
   TagItem *tl_copy = tagList;

   if (!tagList)
      return NULL;

   while ((ti = NextTagItem(&tl_copy)) != NULL)
   {
      if (ti->ti_Tag == tagValue)
         break;
   }
   return ti;
}

