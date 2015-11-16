#include <zune/tagitem.h>

ULONG FilterTagItems(TagItem *tagList, Tag *filterArray, ULONG logic)
{
   ULONG numValid = 0;
   TagItem *ti_tmp;
   TagItem *tmp_list = tagList;

   if (!tagList)
      return 0;
   while ((ti_tmp = NextTagItem(&tmp_list)) != NULL)
      numValid++;
   switch (logic)
   {
    case TAGFILTER_AND:
      tmp_list = tagList;
      while ((ti_tmp = NextTagItem(&tmp_list)) != NULL)
      {
         if (!TagInArray(ti_tmp->ti_Tag, filterArray))
         {
            ti_tmp->ti_Tag = TAG_IGNORE;
            numValid--;
         }
      }
      break;

    case TAGFILTER_NOT:
      if (!filterArray)
         break;
      while (*filterArray != TAG_DONE)
      {
         ti_tmp = FindTagItem(*filterArray++, tagList);
         if (ti_tmp)
            ti_tmp->ti_Tag = TAG_IGNORE;
      }
   }
   return numValid;
}


