#include <zune/tagitem.h>

void FreeTagItems (TagItem *tagList)
{
   g_free(tagList);
}

