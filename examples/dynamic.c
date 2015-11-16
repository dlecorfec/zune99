#include <zune/zune.h>

#ifndef TAGBASE
#define TAGBASE (TAG_USER | (7007 << 16))
#endif

struct DynTestData {
    Object *rect[3];
};

#define MUIM_DynTest_ToggleChild (TAGBASE+1)

struct MUIP_DynTest_ToggleChild { ULONG MsgID; int num;};

static ULONG
mNew (struct IClass *cl, Object *obj, struct opSet *msg)
{
    obj = (Object *)DoSuperNew(cl, obj,
			       InputListFrame,
			       Child, RectangleObject,
			           MUIA_MaxHeight, 5,
	                           MUIA_Background, MUII_SHADOW,
			       End,
			       TAG_DONE);
    if (!obj)
	return 0;
    return (ULONG)obj;
}

static ULONG 
mToggleChild(struct IClass *cl, Object *obj, struct MUIP_DynTest_ToggleChild *msg)
{
    struct DynTestData *data = INST_DATA(cl, obj);

    DoMethod(obj, MUIM_Group_InitChange);
    if (!data->rect[msg->num])
    {
	data->rect[msg->num] = RectangleObject,
	    MUIA_CycleChain, TRUE,
	    MUIA_InputMode, MUIV_InputMode_RelVerify,
	    MUIA_InnerTop, 20,
	    MUIA_InnerLeft, 20,
	    MUIA_Background, MUII_FILL,
	    End;
	if (data->rect[msg->num])
	{
/*  	    g_print("add %p\n", data->rect[msg->num]); */
	    DoMethod(obj, OM_ADDMEMBER, data->rect[msg->num]);
	}
    }
    else
    {
	DoMethod(obj, OM_REMMEMBER, data->rect[msg->num]);
	MUI_DisposeObject(data->rect[msg->num]);
	data->rect[msg->num] = NULL;
    }
    DoMethod(obj, MUIM_Group_ExitChange);

    return TRUE;
}


static ULONG 
MyDispatcher(struct IClass *cl, Object *obj, Msg msg)
{
    switch (msg->MethodID)
    {
	case OM_NEW:
	    return(mNew(cl, obj, (struct opSet *) msg));
	case MUIM_DynTest_ToggleChild:
	    return(mToggleChild(cl, obj, (APTR)msg));
    }
    return(DoSuperMethodA(cl, obj, msg));
}

struct MUI_CustomClass *
CreateDynTestClass (void)
{
    return MUI_CreateCustomClass(NULL, MUIC_Group, NULL,
				 sizeof(struct DynTestData),
				 MyDispatcher);
}

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;
    Object *dgroup;
    Object *b[3];
    struct MUI_CustomClass *cl_dyngroup;
    int i;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    MUI_Init(&argc, &argv);

    cl_dyngroup = CreateDynTestClass();

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Dynamic add/remove",
	    WindowContents, VGroup,
                Child, HGroup,
   		    GroupFrame,
                    Child, b[0] = RectangleObject,
	                    MUIA_CycleChain, TRUE,
                            MUIA_InputMode, MUIV_InputMode_Toggle,
                            MUIA_FixHeight, 20,
                            MUIA_Background, MUII_ButtonBack,
                            ButtonFrame,
                    End,
                    Child, b[1] = RectangleObject,
	                    MUIA_CycleChain, TRUE,
                            MUIA_InputMode, MUIV_InputMode_Toggle,
                            MUIA_FixHeight, 20,
                            MUIA_Background, MUII_ButtonBack,
                            ButtonFrame,
                    End,
                    Child, b[2] = RectangleObject,
	                    MUIA_CycleChain, TRUE,
                            MUIA_InputMode, MUIV_InputMode_Toggle,
                            MUIA_MaxWidth, 20,
                            MUIA_Background, MUII_ButtonBack,
                            ButtonFrame,
                    End,
                End,
/* ---  --- */
	        Child, dgroup = NewObject(cl_dyngroup->mcc_Class, NULL,
					  TAG_DONE),
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     app, 2,
	     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    for (i = 0; i < 3; i++)
    {
	DoMethod(b[i], MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
		 dgroup, 2, MUIM_DynTest_ToggleChild, i);
    }

    set(mainWin, MUIA_Window_Open, TRUE);
    if (!xget(mainWin, MUIA_Window_Open))
    {
	MUI_DisposeObject(app);
	g_error("can't open main window.\n");
    }

    {
	ULONG sigs = 0;

	while (DoMethod(app, MUIM_Application_NewInput, &sigs)
	       != MUIV_Application_ReturnID_Quit)
	{
	}
    }
    
    set(mainWin, MUIA_Window_Open, FALSE);
    MUI_DisposeObject(app);

    if (cl_dyngroup)
	MUI_DeleteCustomClass(cl_dyngroup);
    return 0;
}
