#include <zune/zune.h>
#include <tictactoe_mcc.h>

ULONG
EndGameCB(struct Hook *h, Object *app, ULONG *msg)
{
    Object *ttt = (Object *)msg[0];
    g_print ("Yay!\n");
    DoMethod(ttt, MUIM_TicTacToe_Clear);
    return(0);
}

int main (int argc, char **argv)
{
    static struct Hook EndGameHook  = { {0,0}, EndGameCB, NULL};
    Object *app;
    Object *mainWin;
    Object *ttt;

    MUI_Init(&argc, &argv);

    app = ApplicationObject,
	SubWindow, mainWin = WindowObject,
	    MUIA_Window_Width, MUIV_Window_Width_Screen(20),
	    MUIA_Window_Height, MUIV_Window_Height_Scaled,
	    MUIA_Window_Title, "TicTacToe demo",
	    WindowContents, VGroup,
	        Child, CLabel("A program using an external class,\nloaded at runtime."),
                Child, ttt = TicTacToeObject,
	            GroupFrame,
                End,
            End,
        End,
    End;

    if (!app)
	g_error("can't create application object.\n");

    DoMethod(mainWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     _U(app), 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ttt, MUIM_Notify, MUIA_TicTacToe_Done, TRUE,
	     _U(app), 3, MUIM_CallHook, _U(&EndGameHook), _U(ttt));

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
