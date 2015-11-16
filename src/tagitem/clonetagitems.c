#include <zune/tagitem.h>

/*  original : array to copy
 *  nTags : number of copied tags in new array
 */
TagItem *CloneTagItems(TagItem *original, ULONG *nTags)
{
   TagItem *clone = NULL;
   TagItem *ti_tmp;
   ULONG   NumTags = 0;
   TagItem *tmp_list = original;

   while ((ti_tmp = NextTagItem(&tmp_list)) != NULL)
      NumTags++;
   clone = AllocateTagItems(NumTags);
   if (!clone)
      return NULL;
   RefreshTagItemClones(clone, original);
	if (nTags)
		*nTags = NumTags;
   return clone;
}

