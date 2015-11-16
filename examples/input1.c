#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;
    Object *radio1;
    Object *radio2;

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Input modes",
	    WindowContents, VGroup,
	        Child, MUI_MakeObject(MUIO_BarTitle, _U("MUIV_InputMode_RelVerify")),
                Child, SimpleChainedButton("Hello world, \33u\33iyo\n"
/*  "\33l\33iHello \33bworld\33n, yo\n" */
/*  "\33iHello world, yo\n" */
"_Hello \33uwo\0331r\33bl\33n\33ud\33n, \33iyo\n"
/*  "\33cI \33ilove MUI\n" */
"HelloH \33b\33ihello\33nH"),
	        Child, MUI_MakeObject(MUIO_BarTitle, _U("MUIV_InputMode_Toggle")),
/*                  Child, VSpace(0), */
	        Child, HGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, "The quick brown fox jumps over the lazy dog",
	            MUIA_Background, MUII_GroupBack,
	            Child, ChainedCheckmark("My first checkmark"),
                End,
	        Child, MUI_MakeObject(MUIO_BarTitle, _U("MUIV_InputMode_Immediate")),
	        Child, HGroup,
	            MUIA_Frame, MUIV_Frame_Group,
/*  	            MUIA_FrameTitle, "Radio", */
  	            MUIA_FixHeight, 30,
	            Child, radio1 = RectangleObject,
                            MUIA_CycleChain, TRUE,
	                    MUIA_ControlChar, 'i',
	                    MUIA_InputMode, MUIV_InputMode_Immediate,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Frame, MUIV_Frame_Button,
	            End,
                    Child, radio2 = RectangleObject,
                            MUIA_CycleChain, TRUE,
	                    MUIA_ControlChar, 'o',
	                    MUIA_InputMode, MUIV_InputMode_Immediate,
	                    MUIA_Background, MUII_ButtonBack,
                            MUIA_Frame, MUIV_Frame_Button,
                    End,
                End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     _U(app), 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    set(radio1, MUIA_Selected, TRUE);
    DoMethod(radio1, MUIM_Notify, MUIA_Selected, TRUE,
	     _U(radio2), 3, MUIM_Set, MUIA_Selected, FALSE);
    DoMethod(radio2, MUIM_Notify, MUIA_Selected, TRUE,
	     _U(radio1), 3, MUIM_Set, MUIA_Selected, FALSE);
    /*
     * Open window and ALWAYS check.
     */
    set(mainWin, MUIA_Window_Open, TRUE);
    if (!xget(mainWin, MUIA_Window_Open))
    {
	MUI_DisposeObject(app);
	g_printerr("%s : can't open main window.\n", argv[0]);
	return 5;
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
