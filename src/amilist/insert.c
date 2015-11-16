#include <zune/amilist.h>

void Insert( struct List * list, struct Node * node, struct Node * pred)
{
    g_return_if_fail (node != NULL);
    g_return_if_fail (list != NULL);

    /* If we have a node to insert behind... */
    if (pred)
    {
	/*
	    Our successor is the successor of the node we add ourselves
	    behind and our predecessor is just the node itself.
	*/
	node->ln_Succ = pred->ln_Succ;
	node->ln_Pred = pred;

	/*
	    We are the predecessor of the successor of our predecessor
	    (What ? blblblb... ;) and of out predecessor itself.
	    Note that here the sequence is quite important since
	    we need ln_Succ in the first expression and change it in
	    the second.
	*/
	pred->ln_Succ->ln_Pred = node;
	pred->ln_Succ = node;
    }
    else
    {
	/*
	    add at the top of the list. I do not use AddHead() here but
	    write the code twice for two reasons: 1. The code is small and
	    quite prone to errors and 2. If I would call AddHead(), it
	    would take almost as long to call the function as the execution
	    would take yielding 100% overhead.
	*/
	node->ln_Succ	       = list->lh_Head;
	node->ln_Pred	       = (struct Node *)&list->lh_Head;
	list->lh_Head->ln_Pred = node;
	list->lh_Head	       = node;
    }
} /* Insert */

