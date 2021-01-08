predicate fzn_piecewise_linear_reif(var float: x, var float: y,
                                    array[int] of float: xi1, array[int] of float: xi2,
                                    array[int] of float: vi1, array[int] of float: vi2,
                                    var bool: b) = 
  abort("Reified piecewise_linear (non_continuous) constraint is not supported");

%-----------------------------------------------------------------------------%
