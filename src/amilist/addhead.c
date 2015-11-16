#include <zune/amilist.h>

void AddHead(struct List *list, struct Node * node)
{
    g_return_if_fail (node != NULL);
    g_return_if_fail (list != NULL);

    /*
	Make the node point to the old first node in the list and to the
	head of the list.
    */
    node->ln_Succ	   = list->lh_Head;
    node->ln_Pred	   = (struct Node *)&list->lh_Head;

    /*
	New we come before the old first node which must now point to us
	and the same applies to the pointer to-the-first-node in the
	head of the list.
    */
    list->lh_Head->ln_Pred = node;
    list->lh_Head	   = node;
} /* AddHead */

