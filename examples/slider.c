#include <zune/zune.h>

int main (int argc, char **argv)
{
    Object *app;
    Object *mainWin;

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Title, "Input modes",
	    WindowContents, VGroup,
	        Child, SliderObject,
                   MUIA_Numeric_Value, 50,
                End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     _U(app), 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

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
