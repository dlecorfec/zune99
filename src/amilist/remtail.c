#include <zune/amilist.h>

struct Node * RemTail(struct List * list)
{
    struct Node * node;

    g_return_val_if_fail (list != NULL, NULL);
    /*
	Unfortunately, there is no (quick) check that the node
	is in a list.
    */

    /* Get the last node of the list */
    node = list->lh_TailPred->ln_Pred;
    if (node)
    {
	node->ln_Succ = (struct Node *)&list->lh_Tail;
	node = list->lh_TailPred;
	list->lh_TailPred = node->ln_Pred;
    }

    /* return it's address or NULL if there was no node */
    return node;
} /* RemTail */

