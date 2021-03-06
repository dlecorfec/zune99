/****************************************************************************/
/** Area                                                                   **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Area[];
#else
#define MUIC_Area "Area.mui"
#endif

/* Methods */
enum {
    MUIM_AskMinMax         = 0x80423874, /* Custom Class */ /* V4  */
    MUIM_Cleanup           = 0x8042d985, /* Custom Class */ /* V4  */
    MUIM_ContextMenuBuild  = 0x80429d2e, /* V11 */
    MUIM_ContextMenuChoice = 0x80420f0e, /* V11 */
    MUIM_CreateBubble	   = 0x80421c41, /* V18 */
    MUIM_CreateShortHelp   = 0x80428e93, /* V11 */
    MUIM_DeleteBubble	   = 0x804211af, /* V18 */
    MUIM_DeleteShortHelp   = 0x8042d35a, /* V11 */
    MUIM_DragBegin	   = 0x8042c03a, /* V11 */
    MUIM_DragDrop	   = 0x8042c555, /* V11 */
    MUIM_DragFinish	   = 0x804251f0, /* V11 */
    MUIM_DragQuery	   = 0x80420261, /* V11 */
    MUIM_DragReport	   = 0x8042edad, /* V11 */
    MUIM_Draw		   = 0x80426f3f, /* Custom Class */ /* V4  */
    MUIM_DrawBackground	   = 0x804238ca, /* V11 */
    MUIM_HandleEvent	   = 0x80426d66, /* Custom Class */ /* V16 */
    MUIM_HandleInput	   = 0x80422a1a, /* Custom Class */ /* V4  */
    MUIM_Hide		   = 0x8042f20f, /* Custom Class */ /* V4  */
    MUIM_Setup		   = 0x80428354, /* Custom Class */ /* V4  */
    MUIM_Show		   = 0x8042cc84, /* Custom Class */ /* V4  */
};

struct  MUIP_AskMinMax                      { ULONG MethodID; struct MUI_MinMax *MinMaxInfo; }; /* Custom Class */
struct  MUIP_Cleanup                        { ULONG MethodID; }; /* Custom Class */
struct  MUIP_ContextMenuBuild               { ULONG MethodID; LONG mx; LONG my; };
struct  MUIP_ContextMenuChoice              { ULONG MethodID; Object *item; };
struct  MUIP_CreateBubble                   { ULONG MethodID; LONG x; LONG y; char *txt; ULONG flags; };
struct  MUIP_CreateShortHelp                { ULONG MethodID; LONG mx; LONG my; };
struct  MUIP_DeleteBubble                   { ULONG MethodID; APTR bubble; };
struct  MUIP_DeleteShortHelp                { ULONG MethodID; STRPTR help; };
struct  MUIP_DragBegin                      { ULONG MethodID; Object *obj; };
struct  MUIP_DragDrop                       { ULONG MethodID; Object *obj; LONG x; LONG y; };
struct  MUIP_DragFinish                     { ULONG MethodID; Object *obj; };
struct  MUIP_DragQuery                      { ULONG MethodID; Object *obj; };
struct  MUIP_DragReport                     { ULONG MethodID; Object *obj; LONG x; LONG y; LONG update; };
struct  MUIP_Draw                           { ULONG MethodID; ULONG flags; }; /* Custom Class */
struct  MUIP_DrawBackground                 { ULONG MethodID; LONG left; LONG top; LONG width; LONG height; LONG xoffset; LONG yoffset; LONG flags; };
struct  MUIP_HandleEvent                    { ULONG MethodID; GdkEvent *imsg; LONG muikey; }; /* Custom Class */
struct  MUIP_HandleInput                    { ULONG MethodID; struct IntuiMessage *imsg; LONG muikey; }; /* Custom Class */
struct  MUIP_Hide                           { ULONG MethodID; }; /* Custom Class */
struct  MUIP_Setup                          { ULONG MethodID; struct MUI_RenderInfo *RenderInfo; }; /* Custom Class */
struct  MUIP_Show                           { ULONG MethodID; }; /* Custom Class */

/* Attributes */

enum {
    MUIA_Background         = 0x8042545b, /* V4  is. LONG              */
    MUIA_BottomEdge         = 0x8042e552, /* V4  ..g LONG              */
    MUIA_ContextMenu        = 0x8042b704, /* V11 isg Object *          */
    MUIA_ContextMenuTrigger = 0x8042a2c1, /* V11 ..g Object *          */
    MUIA_ControlChar        = 0x8042120b, /* V4  isg char              */
    MUIA_CycleChain         = 0x80421ce7, /* V11 isg LONG              */
    MUIA_Disabled           = 0x80423661, /* V4  isg BOOL              */
    MUIA_Draggable          = 0x80420b6e, /* V11 isg BOOL              */
    MUIA_Dropable           = 0x8042fbce, /* V11 isg BOOL              */
#ifdef MUI_OBSOLETE	 		
    MUIA_ExportID           = 0x8042d76e, /* V4  isg ULONG             */
#endif /* MUI_OBSOLETE */		
    MUIA_FillArea           = 0x804294a3, /* V4  is. BOOL              */
    MUIA_FixHeight          = 0x8042a92b, /* V4  i.. LONG              */
    MUIA_FixHeightTxt       = 0x804276f2, /* V4  i.. STRPTR            */
    MUIA_FixWidth           = 0x8042a3f1, /* V4  i.. LONG              */
    MUIA_FixWidthTxt        = 0x8042d044, /* V4  i.. STRPTR            */
    MUIA_Font               = 0x8042be50, /* V4  i.g struct TextFont * */
    MUIA_Frame              = 0x8042ac64, /* V4  i.. LONG              */
    MUIA_FramePhantomHoriz  = 0x8042ed76, /* V4  i.. BOOL              */
    MUIA_FrameTitle         = 0x8042d1c7, /* V4  i.. STRPTR            */
    MUIA_Height             = 0x80423237, /* V4  ..g LONG              */
    MUIA_HorizDisappear     = 0x80429615, /* V11 isg LONG              */
    MUIA_HorizWeight        = 0x80426db9, /* V4  isg WORD              */
    MUIA_InnerBottom        = 0x8042f2c0, /* V4  i.g LONG              */
    MUIA_InnerLeft          = 0x804228f8, /* V4  i.g LONG              */
    MUIA_InnerRight         = 0x804297ff, /* V4  i.g LONG              */
    MUIA_InnerTop           = 0x80421eb6, /* V4  i.g LONG              */
    MUIA_InputMode          = 0x8042fb04, /* V4  i.. LONG              */
    MUIA_LeftEdge           = 0x8042bec6, /* V4  ..g LONG              */
    MUIA_MaxHeight          = 0x804293e4, /* V11 i.. LONG              */
    MUIA_MaxWidth           = 0x8042f112, /* V11 i.. LONG              */
    MUIA_Pressed            = 0x80423535, /* V4  ..g BOOL              */
    MUIA_RightEdge          = 0x8042ba82, /* V4  ..g LONG              */
    MUIA_Selected           = 0x8042654b, /* V4  isg BOOL              */
    MUIA_ShortHelp          = 0x80428fe3, /* V11 isg STRPTR            */
    MUIA_ShowMe             = 0x80429ba8, /* V4  isg BOOL              */
    MUIA_ShowSelState       = 0x8042caac, /* V4  i.. BOOL              */
    MUIA_Timer              = 0x80426435, /* V4  ..g LONG              */
    MUIA_TopEdge            = 0x8042509b, /* V4  ..g LONG              */
    MUIA_VertDisappear      = 0x8042d12f, /* V11 isg LONG              */
    MUIA_VertWeight         = 0x804298d0, /* V4  isg WORD              */
    MUIA_Weight             = 0x80421d1f, /* V4  i.. WORD              */
    MUIA_Width              = 0x8042b59c, /* V4  ..g LONG              */
    MUIA_Window             = 0x80421591, /* V4  ..g struct Window *   */
    MUIA_WindowObject       = 0x8042669e, /* V4  ..g Object *          */
};

enum {
    MUIV_Font_Inherit = 0,
    MUIV_Font_Normal = -1,
    MUIV_Font_List = -2,
    MUIV_Font_Tiny = -3,
    MUIV_Font_Fixed = -4,
    MUIV_Font_Title = -5,
    MUIV_Font_Big = -6,
    MUIV_Font_Button = -7,
    MUIV_Font_Knob = -8,
    MUIV_Font_NegCount = -9,
};

enum {
    MUIV_Frame_None = 0,
    MUIV_Frame_Button,
    MUIV_Frame_ImageButton,
    MUIV_Frame_Text,
    MUIV_Frame_String,
    MUIV_Frame_ReadList,
    MUIV_Frame_InputList,
    MUIV_Frame_Prop,
    MUIV_Frame_Gauge,
    MUIV_Frame_Group,
    MUIV_Frame_PopUp,
    MUIV_Frame_Virtual,
    MUIV_Frame_Slider,
    MUIV_Frame_Knob,
    MUIV_Frame_Drag,
    MUIV_Frame_Count,
};

enum {
    MUIV_InputMode_None = 0,
    MUIV_InputMode_RelVerify,
    MUIV_InputMode_Immediate,
    MUIV_InputMode_Toggle,
};

/*** Private ***/

/* Methods */

enum {
    MUIM_DoDrag          = 0x804216bb, /* V18 */ /* Custom Class */
    MUIM_CreateDragImage = 0x8042eb6f, /* V18 */ /* Custom Class */
    MUIM_DeleteDragImage = 0x80423037, /* V18 */ /* Custom Class */
    MUIM_GoActive        = 0x8042491a,
    MUIM_GoInactive      = 0x80422c0c,
    MUIM_CustomBackfill  = 0x80428d73,
};

struct  MUIP_CustomBackfill  { ULONG MethodID; LONG left; LONG top; LONG right; LONG bottom; LONG xoffset; LONG yoffset; };
struct  MUIP_DeleteDragImage { ULONG MethodID; struct MUI_DragImage *di; };              /* Custom Class */
struct  MUIP_CreateDragImage { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; }; /* Custom Class */
struct  MUIP_DoDrag          { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; }; /* Custom Class */

/* Attributes */

enum {
    MUIA_CustomBackfill = 0x80420a63,
};

enum {
    MUIV_CreateBubble_DontHidePointer = (1<<0),
};

struct MUI_DragImage
{
	struct BitMap *bm;
	WORD width;  /* exact width and height of bitmap */
	WORD height;
	WORD touchx; /* position of pointer click relative to bitmap */
	WORD touchy;
	ULONG flags; /* must be set to 0 */
};

