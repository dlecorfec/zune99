#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;

    g_print("use '--gdk-debug all --sync' to debug events\n");

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Another layout demo",
	    WindowContents, VGroup,
                Child, VGroup,
                    MUIA_Weight, 300,
	            MUIA_Group_Columns, 2,
	            MUIA_Group_SameHeight, TRUE,
                    Child, RectangleObject,
	                    MUIA_Background, "5:/usr/share/themes/gradient/entry1.png",
                            MUIA_Frame, MUIV_Frame_String,
                            MUIA_HorizWeight, 80,
	                    MUIA_InnerLeft, 10,
	                    MUIA_InnerRight, 10,
	                    MUIA_InnerTop, 10,
	                    MUIA_InnerBottom, 10,
/*  	                    MUIA_FixWidth, 30, */
                    End,
	            Child, ImageObject,
	                    MUIA_Image_Spec, "4:gradient/RadioButton.mf0",
	                    MUIA_ShowSelState, FALSE,
	                    MUIA_InputMode, MUIV_InputMode_Toggle,
/*  	                    MUIA_Background, "c:/usr/share/themes/gradient/_box_out_dgrad1.png", */
                            MUIA_Frame, MUIV_Frame_None,
/*  	                    MUIA_Image_FreeVert, TRUE, */
/*  	                    MUIA_Image_FreeHoriz, TRUE, */
                            MUIA_HorizWeight, 10,
	            End,
                    Child, RectangleObject,
	                    MUIA_Background, MUII_FILL,
                            MUIA_Frame, MUIV_Frame_PopUp,
	                    MUIA_FixHeight, 30,
                    End,
                    Child, RectangleObject,
	                    MUIA_Background, MUII_FILL,
/*                              MUIA_Frame, MUIV_Frame_Button, */
/*  	                    MUIA_FixWidth, 30, */
	                    MUIA_FixHeight, 10,
	            End,
                End,
	        Child, HGroup,
	            MUIA_Background, MUII_BACKGROUND,
	            MUIA_Frame, MUIV_Frame_Group,
	            MUIA_Group_SameWidth, TRUE,
/*  	            MUIA_FixHeight, 50, */
	            Child, RectangleObject,
	                    MUIA_Background, MUII_FILLBACK,
                            MUIA_Frame, MUIV_Frame_ImageButton,
                            MUIA_InnerLeft, 20,
	                    MUIA_FixHeight, 30,
	            End,
                    Child, RectangleObject,
	                    MUIA_Background, MUII_SHADOWSHINE,
                            MUIA_Frame, MUIV_Frame_Button,
	                    MUIA_FixHeight, 10,
                    End,
                    Child, RectangleObject,
	                    MUIA_Background, MUII_SHINEBACK,
                            MUIA_Frame, MUIV_Frame_Virtual,
   	                    MUIA_InnerTop, 19,
                    End,
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
