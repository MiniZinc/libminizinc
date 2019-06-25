include "fzn_dag.mzn";
include "fzn_dag_reif.mzn";

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a DAG.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate dag(array[int] of $$N: from, array[int] of $$N: to,
                    array[$$N] of var bool: ns, array[int] of var bool: es) =
   assert(index_set(from) = index_set(to),"dreachable: index set of from and to must be identical") /\
   assert(index_set(from) = index_set(es),"dreachable: index set of from and es must be identical") /\
   assert(dom_array(from) subset index_set(ns),"dreachable: nodes in from must be in index set of ns") /\
   assert(dom_array(from) subset index_set(ns),"dreachable: nodes in to must be in index set of ns") /\
   fzn_dag(from,to,ns,es);
