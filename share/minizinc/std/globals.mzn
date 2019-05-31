/***
  @groupdef globals Global constraints
  
  These constraints represent high-level modelling abstractions, for which
  many solvers implement special, efficient inference algorithms.
*/

/***
  @groupdef globals.alldifferent All-Different and related constraints

  @groupdef globals.lexicographic Lexicographic constraints
  
  @groupdef globals.sort Sorting constraints
  
  @groupdef globals.channeling Channeling constraints
  
  @groupdef globals.counting Counting constraints
  
  @groupdef globals.packing Packing constraints
  
  @groupdef globals.scheduling Scheduling constraints
  
  @groupdef globals.graph Graph constraints
  
  @groupdef globals.extensional Extensional constraints (table, regular etc.)
*/

include "all_different.mzn";
include "alldifferent_except_0.mzn";
include "all_disjoint.mzn";
include "all_equal.mzn";
include "alternative.mzn";
include "among.mzn";
include "arg_sort.mzn";
include "arg_min.mzn";
include "arg_max.mzn";
include "at_least.mzn";
include "at_most.mzn";
include "at_most1.mzn";
include "bin_packing.mzn";
include "bin_packing_capa.mzn";
include "bin_packing_load.mzn";
include "bounded_path.mzn";   
include "circuit.mzn";
include "connected.mzn";   
include "cost_mdd.mzn";   
include "cost_regular.mzn";
include "count.mzn";
include "count_eq.mzn";
include "count_neq.mzn";
include "count_geq.mzn";
include "count_gt.mzn";
include "count_leq.mzn";
include "count_lt.mzn";
include "cumulative.mzn";
include "cumulative_opt.mzn";
include "dag.mzn";   
include "decreasing.mzn";
include "diffn.mzn";
include "diffn_nonstrict.mzn";
include "diffn_k.mzn";
include "diffn_nonstrict_k.mzn";
include "disjoint.mzn";
include "disjunctive.mzn";
include "disjunctive_strict.mzn";
include "disjunctive_opt.mzn";
include "disjunctive_strict_opt.mzn";
include "distribute.mzn";
include "element.mzn";
include "exactly.mzn";
include "geost.mzn";
include "global_cardinality.mzn";
include "global_cardinality_closed.mzn";
include "global_cardinality_low_up.mzn";
include "global_cardinality_low_up_closed.mzn";
include "increasing.mzn";
include "int_set_channel.mzn";
include "inverse.mzn";
include "inverse_set.mzn";
include "knapsack.mzn";
include "lex_greater.mzn";
include "lex_greatereq.mzn";
include "lex_lesseq.mzn";
include "lex_less.mzn";
include "lex2.mzn";
include "link_set_to_booleans.mzn";
include "maximum.mzn";
include "mdd.mzn";
include "member.mzn";
include "minimum.mzn";
include "network_flow.mzn";
include "neural_net.mzn";
include "nvalue.mzn";
include "partition_set.mzn";
include "range.mzn";
include "reachable.mzn";   
include "regular.mzn";
include "regular_nfa.mzn";
include "roots.mzn";
include "sliding_sum.mzn";
include "sort.mzn";
include "span.mzn";
include "steiner.mzn";
include "strict_decreasing.mzn";
include "string_increasing.mzn";
include "strict_lex2.mzn";
include "subcircuit.mzn";
include "subgraph.mzn";   
include "sum_pred.mzn";
include "sum_set.mzn";
include "symmetric_all_different.mzn";
include "table.mzn";
include "tree.mzn";   
include "value_precede.mzn";
include "value_precede_chain.mzn";
include "wst.mzn";
