#include <zune/tagitem.h>

TagItem *ReallocateTagItems (TagItem *tags, ULONG numTags)
{
   return((TagItem *)g_realloc(tags, numTags * sizeof(TagItem)));
}

