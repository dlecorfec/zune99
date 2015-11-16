#include <zune/zune.h>


/* using callbacks is bad MUI programming
 * (use classes and methods instead)
 * but it's ok for a quick hack
 */
ULONG
SavePrefsCB(struct Hook *h, Object *app, ULONG *dummy)
{
    /*
     * Temporary - just to obtain written prefs, waiting for the editor
     */
    __zune_prefs_user_global_write_current();
    return(0);
}

int main (int argc, char **argv)
{
    static struct Hook SavePrefsHook  = { {0,0}, SavePrefsCB, NULL};
    Object *app;
    Object *mainWin;
    Object *BT_Save;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
        MUIA_Application_Title, "ImagesDemo",
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Images",
	    WindowContents, HGroup,
	            Child, ImageObject,
	                    MUIA_Image_Spec, MUII_CheckMark,
	                    MUIA_InputMode, MUIV_InputMode_Toggle,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Frame, MUIV_Frame_None,
	                    MUIA_ShowSelState, FALSE,
	            End,
	            Child, RectangleObject,
	            End,
	            Child, BT_Save = RectangleObject,
	                    MUIA_InputMode, MUIV_InputMode_RelVerify,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Frame, MUIV_Frame_Button,
                    End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(BT_Save, MUIM_Notify, MUIA_Pressed, FALSE,
	     app, 2, MUIM_CallHook, &SavePrefsHook);
    /*
     * Open window and ALWAYS check.
     */
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
    return 0;
}
