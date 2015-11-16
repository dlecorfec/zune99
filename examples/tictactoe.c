/* External Class for Zune, to be installed in Zune external classes path.
 * Should look like a mix between GTK TicTacToe example from the GTK Tutorial
 * (Ian Main and others),
 * and Tron.mcc, (c) Copyright 1995-96 by kmel, Klaus Melchior
 */

#include <tictactoe_mcc.h>

#define CLASS       MUIC_TicTacToe
#define SUPERCLASS  MUIC_Group

#define VERSION 0
#define REVISION 0

/*
 * Instance data
 */
struct Data
{
    Object *buttons[3][3];
};

/*
 * TicTacToe private methods
 */

enum TicTacToe_private_methods {
    MUIM_TicTacToe_Toggle = (TAG_USER | 1),
};

struct MUIP_TicTacToe_Toggle { ULONG MethodID; Object *button; };


/*
 * Static functions
 */
static ULONG _Dispatcher(struct IClass *cl, Object *obj, Msg msg);
static BOOL ClassInit (void);
static void ClassExit (void);

/*
 * Static variables
 */
static const char UserLibName[] = CLASS;


#include <zune/mccheader.c>

/******************************************************************************/
/* Class Functions                                                            */
/******************************************************************************/
static BOOL 
ClassInit (void)
{
    return TRUE;
}

static void 
ClassExit (void)
{
}

/******************************************************************************/
/* MCC Functions                                                              */
/******************************************************************************/

static ULONG 
_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Data *data;
    int i;
    int j;

    obj = (Object *)DoSuperNew(cl, obj,
                               MUIA_Group_Columns, 3,
			       TAG_MORE, _U(msg->ops_AttrList),
			       TAG_DONE);
			       
    if (!obj)
	return(0);

    /*** init data ***/
    data = INST_DATA(cl, obj);

    for (i = 0; i < 3; i++)
    {
	for (j = 0; j < 3; j++)
	{
	    data->buttons[i][j] = RectangleObject,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_InputMode, MUIV_InputMode_Toggle,
		End;
	    if (!data->buttons[i][j])
	    {
		CoerceMethod(cl, obj, OM_DISPOSE);
		return 0;
	    }
	    DoMethod(obj, OM_ADDMEMBER, _U(data->buttons[i][j]));
	    DoMethod(data->buttons[i][j], MUIM_Notify,
		     MUIA_Selected, MUIV_EveryTime,
		     _U(obj), 2, MUIM_TicTacToe_Toggle, _U(data->buttons[i][j]));
	}
    }

    return((ULONG)obj);
}

static ULONG
_Dispose(struct IClass *cl, Object *obj, Msg msg)
{
    return(DoSuperMethodA(cl,obj,msg));
}

static ULONG
_Get(struct IClass *cl, Object *obj, struct opGet *msg)
{
    ULONG       *store = msg->opg_Storage;

    switch(msg->opg_AttrID)
    {
	case MUIA_Version:
	    *store = VERSION;
	    return(TRUE);
	case MUIA_Revision:
	    *store = REVISION;
	    return(TRUE);
	case MUIA_TicTacToe_Done:
	    /* Game will set() it to TRUE to trigger notification. */
	    *store = FALSE;
	    return(TRUE);
    }
    return(DoSuperMethodA(cl, obj, (Msg) msg));
}


static ULONG
_TicTacToe_Clear(struct IClass *cl, Object *obj, Msg msg)
{
    struct Data *data = INST_DATA(cl, obj);
    int i,j;

    for (i=0;i<3;i++)
	for (j=0;j<3;j++)
	{
	    nnset(data->buttons[i][j], MUIA_Selected, FALSE);
	}
    nnset(obj, MUIA_TicTacToe_Done, FALSE);
    return TRUE;
}

static ULONG
_TicTacToe_Toggle(struct IClass *cl, Object *obj,
		  struct MUIP_TicTacToe_Toggle *msg)
{
    struct Data *data = INST_DATA(cl, obj);
    int i, k;

    static int rwins[8][3] = { { 0, 0, 0 }, { 1, 1, 1 }, { 2, 2, 2 },
			       { 0, 1, 2 }, { 0, 1, 2 }, { 0, 1, 2 },
			       { 0, 1, 2 }, { 0, 1, 2 } };
    static int cwins[8][3] = { { 0, 1, 2 }, { 0, 1, 2 }, { 0, 1, 2 },
			       { 0, 0, 0 }, { 1, 1, 1 }, { 2, 2, 2 },
			       { 0, 1, 2 }, { 2, 1, 0 } };
    int success, found;

    for (k=0; k<8; k++)
    {
	success = TRUE;
	found = FALSE;

	for (i=0;i<3;i++)
	{
	    success = success &&
		xget(data->buttons[rwins[k][i]][cwins[k][i]], MUIA_Selected);
	    found = found ||
		data->buttons[rwins[k][i]][cwins[k][i]] == msg->button;
	}
	if (success && found)
	{
	    set(obj, MUIA_TicTacToe_Done, TRUE);
	    break;
	}
    }
    return TRUE;
}

static ULONG _Dispatcher(struct IClass *cl, Object *obj, Msg msg)
{
    switch(msg->MethodID)
    {
	case OM_NEW :
	    return _New(cl, obj, (APTR)msg);
	case OM_DISPOSE :
	    return _Dispose(cl, obj, (APTR)msg);
	case OM_GET : 
	    return _Get(cl, obj, (APTR)msg);
	case MUIM_TicTacToe_Clear :
	    return _TicTacToe_Clear(cl, obj, (APTR)msg);
	case MUIM_TicTacToe_Toggle :
	    return _TicTacToe_Toggle(cl, obj, (APTR)msg);
    }

    return(DoSuperMethodA(cl,obj,msg));
}
