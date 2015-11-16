#include <zune/tagitem.h>

TagItem *NextTagItem(TagItem **tagItemPtr)
{
   if (!tagItemPtr || !*tagItemPtr)
      return NULL;

   while (1)
   {
      ULONG i, j;

      switch ((*tagItemPtr)->ti_Tag)
      {
         case TAG_DONE   :
            return NULL;

         case TAG_IGNORE :
            (*tagItemPtr)++;
            break;

         case TAG_SKIP   :
            j = (ULONG)(*tagItemPtr)->ti_Data;
            for (i = 0; i < j ; i++)
               (*tagItemPtr)++;
            break;

         case TAG_MORE   :
            *tagItemPtr = (TagItem *)((*tagItemPtr)->ti_Data);
            break;

         default         :
            (*tagItemPtr)++;
            return (*tagItemPtr - 1);
      }
   }
   return NULL;
}

