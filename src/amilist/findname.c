#include <zune/amilist.h>

struct Node * FindName(struct List *list, UBYTE *name)
{
    struct Node * node;

    g_assert (list);
    g_return_val_if_fail(name != NULL, NULL);

    /* Look through the list */
    for (node=GetHead(list); node; node=GetSucc(node))
    {
	/* check the node. If we found it, stop */
	if (!strcmp (node->ln_Name, name))
	    break;
    }

    /*
	If we found a node, this will contain the pointer to it. If we
	didn't, this will be NULL (either because the list was
	empty or because we tried all nodes in the list)
    */
    return node;
} /* FindName */

