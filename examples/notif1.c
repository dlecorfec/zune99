#include <zune/zune.h>

int main (void)
{
    Object *obj;

    obj = MUI_NewObjectA(MUIC_Notify, NULL);
    printf("new Notify object = %p\n", obj);
    set(obj, MUIA_UserData, 21);
    printf("object UserData = %ld\n", xget(obj, MUIA_UserData));

    DoMethod(obj, MUIM_Notify, MUIA_UserData, MUIV_EveryTime, MUIV_Notify_Self,
	     3, MUIM_Set, MUIA_ObjectID, 19);
    set(obj, MUIA_ObjectID, 12);
    printf("id = %ld\n", xget(obj, MUIA_ObjectID));
    set(obj, MUIA_UserData, 22);
    printf("id after notification = %ld\n", xget(obj, MUIA_ObjectID));

    if (obj)
	DisposeObject(obj);
    exit(0);
}
