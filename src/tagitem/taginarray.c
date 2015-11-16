#include <zune/tagitem.h>

BOOL TagInArray(Tag tagValue, Tag *tagArray)
{
   BOOL result = FALSE;

   if (!tagArray)
      return FALSE;
   while (*tagArray != TAG_DONE)
   {
      if (*tagArray++ == tagValue)
      {
         result = TRUE;
         break;
      }
   }
   return result;
}
