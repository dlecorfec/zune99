#include <zune/amilist.h>

void Remove(struct Node * node)
{
    g_return_if_fail (node != NULL);
    /*
	Unfortunately, there is no (quick) check that the node
	is in a list.
    */

    /*
	Just bend the pointers around the node, ie. we make our
	predecessor point to out successor and vice versa
    */
    node->ln_Pred->ln_Succ = node->ln_Succ;
    node->ln_Succ->ln_Pred = node->ln_Pred;
} /* Remove */

