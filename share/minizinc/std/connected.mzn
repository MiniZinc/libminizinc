
/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be connected.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate dconnected(array[int] of $$N: from, array[int] of $$N: to,
                    array[$$N] of var bool: ns, array[int] of var bool: es) =
   assert(index_set(from) = index_set(to),"dconnected: index set of from and to must be identical") /\
   assert(index_set(from) = index_set(es),"dconnected: index set of from and es must be identical") /\
   assert(dom_array(from) subset index_set(ns),"dconnected: nodes in from must be in index set of ns") /\
   assert(dom_array(from) subset index_set(ns),"dconnected: nodes in to must be in index set of ns") /\
   fzn_dconnected(from,to,ns,es);
   
include "fzn_connected.mzn";
   
%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be connected.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate connected(array[int] of $$N: from, array[int] of $$N: to,
                    array[$$N] of var bool: ns, array[int] of var bool: es) =
   assert(index_set(from) = index_set(to),"connected: index set of from and to must be identical") /\
   assert(index_set(from) = index_set(es),"connected: index set of from and es must be identical") /\
   assert(dom_array(from) subset index_set(ns),"connected: nodes in from must be in index set of ns") /\
   assert(dom_array(from) subset index_set(ns),"connected: nodes in to must be in index set of ns") /\
   fzn_connected(from,to,ns,es);
      