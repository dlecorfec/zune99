#include <zune/amilist.h>

struct Node * RemHead(struct List * list)
{
    struct Node * node;

    g_return_val_if_fail (list != NULL, NULL);
    /*
	Unfortunately, there is no (quick) check that the node
	is in a list
    */

    /* Get the address of the first node or NULL */
    node = list->lh_Head->ln_Succ;
    if (node)
    {
	node->ln_Pred = (struct Node *)list;
	node = list->lh_Head;
	list->lh_Head = node->ln_Succ;
    }

    /* Return the address or NULL */
    return node;
} /* RemHead */

