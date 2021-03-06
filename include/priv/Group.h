/****************************************************************************/
/** Group                                                                  **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Group[];
#else
#define MUIC_Group "Group.mui"
#endif

/* Methods */

#define MUIM_Group_ExitChange               0x8042d1cc /* V11 */
#define MUIM_Group_InitChange               0x80420887 /* V11 */
#define MUIM_Group_Sort                     0x80427417 /* V4  */
struct  MUIP_Group_ExitChange               { ULONG MethodID; };
struct  MUIP_Group_InitChange               { ULONG MethodID; };
struct  MUIP_Group_Sort                     { ULONG MethodID; Object *obj[1]; };

/* Attributes */

#define MUIA_Group_ActivePage               0x80424199 /* V5  isg LONG              */
#define MUIA_Group_Child                    0x804226e6 /* V4  i.. Object *          */
#define MUIA_Group_ChildList                0x80424748 /* V4  ..g struct List *     */
#define MUIA_Group_Columns                  0x8042f416 /* V4  is. LONG              */
#define MUIA_Group_Horiz                    0x8042536b /* V4  i.. BOOL              */
#define MUIA_Group_HorizSpacing             0x8042c651 /* V4  isg LONG              */
#define MUIA_Group_LayoutHook               0x8042c3b2 /* V11 i.. struct Hook *     */
#define MUIA_Group_PageMode                 0x80421a5f /* V5  i.. BOOL              */
#define MUIA_Group_Rows                     0x8042b68f /* V4  is. LONG              */
#define MUIA_Group_SameHeight               0x8042037e /* V4  i.. BOOL              */
#define MUIA_Group_SameSize                 0x80420860 /* V4  i.. BOOL              */
#define MUIA_Group_SameWidth                0x8042b3ec /* V4  i.. BOOL              */
#define MUIA_Group_Spacing                  0x8042866d /* V4  is. LONG              */
#define MUIA_Group_VertSpacing              0x8042e1bf /* V4  isg LONG              */

#define MUIV_Group_ActivePage_First 0
#define MUIV_Group_ActivePage_Last -1
#define MUIV_Group_ActivePage_Prev -2
#define MUIV_Group_ActivePage_Next -3
#define MUIV_Group_ActivePage_Advance -4

/* Attributes */

#define MUIA_Group_Forward    0x80421422 /* V11 .s. BOOL */

