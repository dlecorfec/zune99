#ifndef UTILITY_TAGITEM_H
#define UTILITY_TAGITEM_H

#include <zune/amitypes.h>

/* Tags are a general mechanism of extensible data arrays for parameter
 * specification and property inquiry. In practice, tags are used in arrays,
 * or chain of arrays, or variable arglists.
 *
 * eg. 3 tagitems given as a list :
 *  foo(FixedArg1 a, FixedArg2 b, Tag t, ...);
 *  foo(argfoo, argbar, Tag1, val1, Tag2, val2, Tag3, val3, TAG_DONE, 0);
 *
 * DON'T FORGET TAG_DONE !
 *
 *
 * Programmers defining their own tags:
 * eg. #define MyTag_Width (TAG_USER | 0x54321)
 * Always use TAG_USER for your tags !
 * see below for control tags
 */

/*****************************************************************************/


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*****************************************************************************/


typedef ULONG Tag;

typedef struct TagItem
{
    Tag	   ti_Tag;	/* identifies the type of data */
    ULONG  ti_Data;	/* type-specific data, either pointer or integer */
} TagItem;

/* constants for Tag.ti_Tag, control tag values */
#define TAG_DONE   (0L)	  /* terminates array of TagItems. ti_Data unused */
#define TAG_END	   (0L)   /* synonym for TAG_DONE			  */
#define	TAG_IGNORE (1L)	  /* ignore this item, not end of array		  */
#define	TAG_MORE   (2L)	  /* ti_Data is pointer to another array of TagItems
			   * note that this tag terminates the current array
			   */
#define	TAG_SKIP   (3L)	  /* skip this and the next ti_Data items	  */

/* differentiates user tags from control tags */
#define TAG_USER   ((ULONG)(1L<<31))

/* If the TAG_USER bit is set in a tag number, it tells utility.library that
 * the tag is not a control tag (like TAG_DONE, TAG_IGNORE, TAG_MORE) and is
 * instead an application tag. "USER" means a client of utility.library in
 * general, including system code like Intuition or ASL, it has nothing to do
 * with user code.
 * eg. #define MyTag_Width (TAG_USER | 0x54321)
 */

/*****************************************************************************/

/* Tag filter logic specifiers for use with FilterTagItems() */
#define TAGFILTER_AND 0		/* exclude everything but filter hits	*/
#define TAGFILTER_NOT 1		/* exclude only filter hits		*/

/*****************************************************************************/

/* Mapping types for use with MapTags() */
#define MAP_REMOVE_NOT_FOUND 0	/* remove tags that aren't in mapList */
#define MAP_KEEP_NOT_FOUND   1	/* keep tags that aren't in mapList   */

/*****************************************************************************/

/* Only NextTagItem, FindTagItem and GetTagData are commonly used
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate memory for an array */
TagItem*   AllocateTagItems    (ULONG numTags);

/* Reallocate memory from AllocateTagItems */
TagItem*   ReallocateTagItems  (TagItem *, ULONG numTags);

void       ApplyTagChanges     (TagItem *, TagItem *);

TagItem*   CloneTagItems       (TagItem *, ULONG *);

void       FilterTagChanges    (TagItem *, TagItem *, ULONG);

ULONG      FilterTagItems      (TagItem *,Tag *,ULONG);

/* Find the TagItem with the given Tag in an array */
TagItem*   FindTagItem         (Tag, TagItem *);

/* Free memory from AllocateTagItems */
void       FreeTagItems        (TagItem *);

/* Get value of a given tag if found, or return the default value */
IPTR       GetTagData          (Tag, IPTR, TagItem *);

void       MapTags             (TagItem *,TagItem *,ULONG);

/* TagItem iterator */
TagItem*   NextTagItem         (TagItem **);

/* ??? */
ULONG      PackBoolTags        (ULONG, TagItem *,TagItem *);

/* Update a cloned array with new values */
void       RefreshTagItemClones(TagItem *,TagItem *);

/* Test quickly if given tag is in array (no handling of control tag) */
BOOL       TagInArray          (Tag,Tag *);

#ifdef __cplusplus
}
#endif

#endif /* UTILITY_TAGITEM_H */
