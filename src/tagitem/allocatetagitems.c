#include <zune/tagitem.h>

TagItem *AllocateTagItems (ULONG numTags)
{
   TagItem *tags = NULL;

   if (numTags)
   {
      tags = (TagItem *)g_malloc0(numTags * sizeof(TagItem));
   }
   return tags;
}

