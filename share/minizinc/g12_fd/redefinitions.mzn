% FlatZinc built-in redefintions for G12/FD.
% There are currently annotations related to the new search annotations:
%  - Limits 
%  - Restarts
%  - Multiple Searches
% XXX NOT YET ACTIVATED
% annotation limit(ann: measure, int: value, ann: search);
% annotation restart_geometric(float: factor, float: value, ann: search);
% annotation multi_search(array[ann]: multisearch);

predicate bool_clause_reif(array[int] of var bool: as,
                           array[int] of var bool: bs,
                           var bool: b) =
  clause(as,bs++[b]) /\
  forall (i in index_set(as)) (as[i] -> b) /\
  forall (i in index_set(bs)) (bs[i] \/ b);
