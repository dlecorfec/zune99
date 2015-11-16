#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;
    Object *pages;
    Object *stepb[2];
    Object *b[3];
    int i;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Pages",
	    WindowContents, VGroup,
                Child, VGroup,
   		    GroupFrame,
		    Child, HGroup,
			Child, stepb[0] = RectangleObject,
	                        MUIA_CycleChain, TRUE,
				MUIA_InnerLeft, 20,
				MUIA_FixHeight, 20,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_Background, MUII_ButtonBack,
				ButtonFrame,
			End,
			Child, stepb[1] = RectangleObject,
	                        MUIA_CycleChain, TRUE,
				MUIA_InnerLeft, 20,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_Background, MUII_ButtonBack,
				ButtonFrame,
			End,
		    End,

	            Child, MUI_MakeObject(MUIO_HBar, 4),
		    Child, HGroup,
			Child, b[0] = RectangleObject,
	                        MUIA_CycleChain, TRUE,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_FixHeight, 20,
				MUIA_Background, MUII_ButtonBack,
				ButtonFrame,
			End,
			Child, b[1] = RectangleObject,
	                        MUIA_CycleChain, TRUE,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_FixHeight, 20,
				MUIA_Background, MUII_ButtonBack,
				ButtonFrame,
			End,
			Child, b[2] = RectangleObject,
	                        MUIA_CycleChain, TRUE,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_FixHeight, 20,
				MUIA_Background, MUII_ButtonBack,
				ButtonFrame,
			End,
		    End,
                End,
	        Child, pages = HGroup,
	            MUIA_Group_PageMode, TRUE,
	            InputListFrame,
	            MUIA_Background, MUII_PageBack,
	            Child, RectangleObject,
	                    MUIA_FixWidth, 50,
	                    MUIA_Background, MUII_SHADOW,
	            End,
                    Child, RectangleObject,
	                    MUIA_FixWidth, 30,
                            MUIA_FixHeight, 30,
	                    MUIA_Background, MUII_SHINE,
                    End,
                    Child, RectangleObject,
                            MUIA_FixHeight, 50,
	                    MUIA_Background, MUII_FILL,
                    End,
                End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     app, 2,
	     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(stepb[0], MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
	     pages, 3, MUIM_Set,
	     MUIA_Group_ActivePage, MUIV_Group_ActivePage_Prev);

    DoMethod(stepb[1], MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
	     pages, 3, MUIM_Set,
	     MUIA_Group_ActivePage, MUIV_Group_ActivePage_Next);

    for (i = 0; i < 3; i++)
    {
	DoMethod(b[i], MUIM_Notify, MUIA_Pressed, FALSE,
		 pages, 3, MUIM_Set, MUIA_Group_ActivePage, i);
    }
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
