#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "2-Dimensions group",
	    WindowContents, VGroup,
	        MUIA_Group_Columns, 2,
                Child, RectangleObject,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Weight, 60,
	                    MUIA_InnerLeft, 10,
/*  	                    MUIA_InnerRight, 10, */
	                    MUIA_InnerTop, 10,
	                    MUIA_InnerBottom, 10,
/*  	                    MUIA_FixWidth, 30, */
                End,
	        Child, RectangleObject,
	                    MUIA_Background, MUII_FILL,
  	                    MUIA_FixWidth, 42,
	        End,
	        Child, RectangleObject,
	                    MUIA_Background, MUII_SHINE,
                            MUIA_Weight, 40,
	        End,
                Child, RectangleObject,
	                    MUIA_Background, MUII_SHINE,
  	                    MUIA_FixHeight, 30,
   	                    MUIA_FixWidth, 30,
                End,
                Child, RectangleObject,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Weight, 20,
                End,
                Child, RectangleObject,
	                    MUIA_Background, MUII_FILLSHINE,
                            MUIA_Weight, 20,
/*  	                    MUIA_FixWidth, 30, */
	        End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     _U(app), 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

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

	while (DoMethod(app, MUIM_Application_NewInput, _U(&sigs))
	       != MUIV_Application_ReturnID_Quit)
	{
	}
    }
    
    set(mainWin, MUIA_Window_Open, FALSE);
    MUI_DisposeObject(app);

    return 0;
}
