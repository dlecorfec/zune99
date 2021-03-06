#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    /*
     * Zune needed this new call, for underlying libs.
     */
    MUI_Init(&argc, &argv);

    /*
     * This big call uses macros from mui.h
     * You may also create intermediate objects.
     * Usually you will only create one window, which will be
     * an instance of a custom Window subclass (remember to use
     * subclasses as much as possible !)
     */
    app = ApplicationObject,
        MUIA_Application_Author, "Reez",
	MUIA_Application_Title, "Application1",
        MUIA_Application_Description, "A very simple test program",
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "My first MUI app !",
	    WindowContents, VGroup,
                Child, HGroup,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Frame, MUIV_Frame_Button,
                            MUIA_Weight, 60,
	                    MUIA_InnerLeft, 10,
	                    MUIA_InnerRight, 10,
	                    MUIA_InnerTop, 10,
	                    MUIA_InnerBottom, 10,
/*  	                    MUIA_FixWidth, 30, */
                        End,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_FILL,
                            MUIA_Frame, MUIV_Frame_Button,
  	                    MUIA_FixWidth, 42,
                        End,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_FILLSHINE,
                            MUIA_Frame, MUIV_Frame_Button,
                            MUIA_Weight, 40,
                        End,
                End,
                Child, HGroup,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_SHADOWBACK,
                            MUIA_Frame, MUIV_Frame_Button,
  	                    MUIA_FixHeight, 30,
                        End,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_TextBack,
                            MUIA_Frame, MUIV_Frame_Button,
                        End,
	        End,
                Child, HGroup,
	                MUIA_Weight, 50,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_FILLSHINE,
                            MUIA_Frame, MUIV_Frame_Button,
/*  	                    MUIA_FixWidth, 30, */
                        End,
                        Child, RectangleObject,
	                    MUIA_Background, MUII_FILLBACK,
                            MUIA_Frame, MUIV_Frame_Button,
                            MUIA_Weight, 50,
                        End,
	        End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");
    g_print("created Application object %p\n", app);

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    /*
     * Open window and ALWAYS check.
     */
    set(mainWin, MUIA_Window_Open, TRUE);
    if (!xget(mainWin, MUIA_Window_Open))
    {
	MUI_DisposeObject(app);
	g_error("can't open main window.\n");
    }

/*
** This is the ideal input loop for an object oriented MUI application.
** Everything is encapsulated in classes, no return ids need to be used,
** we just check if the program shall terminate.
** Note that MUIM_Application_NewInput expects sigs to contain the result
** from Wait() (or 0). This makes the input loop significantly faster.
*/
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










#ifdef FOO
void
foo ()
{
    /*
     * Dynamic window adding
     */
    {
	Object *dynWin;

	dynWin = WindowObject,
	    MUIA_Window_Title, "A dynamic window",
	    WindowContents, RectangleObject,
	        MUIA_Background, MUII_SHINE, /* a MUI-fixed color */
	        End,
            End,
	End;

        if (!dynWin)
	{
	    set(mainWin, MUIA_Window_Open, FALSE);
	    MUI_DisposeObject(appli);
	    g_error("can't create dynamic window\n");
	}

	DoMethod(app, OM_ADDMEMBER, dynWin);
	/* Window added to Application window list.
	 * Then you should open it ...
	 */
/*
  set(dynWin, MUIA_Window_Open, TRUE);
  if (!xget(dynWin, MUIA_Window_Open))
  {
  MUI_DisposeObject(appli);
  set(mainWin, MUIA_Window_Open, FALSE);
  g_error("can't open dynamic window.\n");
  }
       
  set(dynWin, MUIA_Window_Open, FALSE);
*/
	/*
	 * Remove window from application
	 */
	DoMethod(app, OM_REMMEMBER, dynWin);
	/*
	 * If you intend to close a window from one of its methods
	 * (because you subclassed it), use MUIM_Application_PushMethod
	 * to ask another object to destroy you.
	 * Never EVER dispose yourself !!!
	 */
	MUI_DisposeObject(dynWin);
    }
}
#endif
