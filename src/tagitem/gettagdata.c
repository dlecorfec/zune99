#include <zune/tagitem.h>

IPTR GetTagData(Tag tagValue, IPTR defaultVal, TagItem *tagList)
{
   TagItem *ti = NULL;
   IPTR     value = defaultVal;

   ti = FindTagItem(tagValue, tagList);
   if (ti)
      value = ti->ti_Data;
   return value;
}
