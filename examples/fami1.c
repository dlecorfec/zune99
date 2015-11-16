#include <zune/zune.h>

#define NUMCHILDS 5

int main (void)
{
    Object *obj;
    TagItem tags[NUMCHILDS];
    int i;

    for (i = 0; i < NUMCHILDS; i++)
    {
	tags[i].ti_Tag = MUIA_Family_Child;
	tags[i].ti_Data = (ULONG)MUI_NewObjectA(MUIC_Notify, NULL);
    }
    
    obj = MUI_NewObject(MUIC_Family,
			TAG_MORE, tags,
			TAG_DONE);
    g_print("family obj = %p\n", obj);

    if (obj)
	MUI_DisposeObject(obj);
    exit(0);
}
