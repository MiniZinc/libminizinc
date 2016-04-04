/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov
*/

%-----------------------------------------------------------------------------%
%
% Logic operations
% Use indicators for reifs (CPLEX)?  Seems weak.
%
%-----------------------------------------------------------------------------%

predicate bool_not(var bool: p, var bool: q) =
    bool2int(p) + bool2int(q) = 1;

predicate bool_and(var bool: p, var bool: q, var bool: r) =
%  my_trace("  bool_and: \(p) /\\ \(q) <-> \(r) \n") /\
  if false then
      int_lin_le_reif__IND( [-1, -1], [p, q], -2, r)
  else
    bool_and__INT(bool2int(p), bool2int(q), bool2int(r))
  endif;

predicate bool_and__INT(var int: x, var int: y, var int: z) =
%  my_trace("  bool_and__INT: \(x) /\\ \(y) <-> \(z) \n") /\
    x + y <= z + 1 /\
    %% x + y >= z * 2;         % weak
    x >= z /\ y >= z;     % strong


predicate bool_or(var bool: p, var bool: q, var bool: r) =
%  my_trace("  bool_or: \(p) \\/ \(q) <-> \(r) \n") /\
  if fMZN__UseIndicators then
%      my_trace__DBG("  bool_or  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
      int_lin_le_reif__IND( [-1, -1], [p, q], -1, r)
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
%    my_trace("              bool_or:  0..1: \(x) \\/ \(y) <-> \(z) \n") /\
    x + y >= z /\
    % x + y <= z * 2;       % weak
    x <= z /\ y <= z   % strong
  endif;


predicate bool_xor(var bool: p, var bool: q, var bool: r) =
  if false then
%      my_trace__DBG("  bool_xor  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
%      int_lin_eq_reif__IND( [1, 1], [p, q], 1, r) /\
    true
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
    x <= y + z /\
    y <= x + z /\
    z <= x + y /\
    x + y + z <= 2
  endif;


predicate bool_eq_reif(var bool: p, var bool: q, var bool: r) =
    if is_fixed(r) then   % frequent case
        if fix(r) = true then p = q else bool_not(p,q) endif
    elseif false then
%      my_trace__DBG("  bool_eq_reif  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
%      int_lin_eq_reif__IND( [1, -1], [p, q], 0, r) /\
      true
    else
      let { var int: x = bool2int(p),
            var int: y = bool2int(q),
            var int: z = bool2int(r) }
      in
      x + y <= z + 1 /\
      x + z <= y + 1 /\
      y + z <= x + 1 /\
      x + y + z >= 1
    endif;


predicate bool_ne_reif(var bool: p, var bool: q, var bool: r) =
    bool_xor(p, q, r);


predicate bool_le(var bool: p, var bool: q) =
    let { var int: x = bool2int(p),
          var int: y = bool2int(q) }
    in
%  my_trace("  bool_le: \(p) <= \(q), , 0..1: \(x) <= \(y) \n") /\
    x <= y;


predicate bool_le_reif(var bool: p, var bool: q, var bool: r) =
  if false then
%      my_trace__DBG("  bool_le_reif  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
%      int_lin_le_reif__IND( [1, -1], [p, q], 0, r) /\
    true
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
%    my_trace("  bool_le_reif: \(p) le \(q) equiv \(r), integers: \(x) le \(y) equiv \(z) \n") /\
    1 - x + y >= z /\
      %% /\ 1 - x + y <= z * 2      not needed
    1 - x <= z /\ y <= z % strong
  endif;


predicate bool_lt(var bool: p, var bool: q) =
    not p /\ q;


predicate bool_lt_reif(var bool: p, var bool: q, var bool: r) =
%  my_trace("  bool_lt_reif: \(p) lt \(q) equiv \(r) \n") /\
    (not p /\ q) <-> r;

%-----------------------------------------------------------------------------%

predicate array_bool_or(array[int] of var bool: a, var bool: b) =
    if is_fixed(b) then   % frequent case
  %%%% TODO: "Cannot partially evaluate let expression" ???????????
  %      my_trace( "  array_bool_or: \(a), \(b) \n") /\
%        my_trace( "  array_bool_or FIXED: " ++ show(index_set(a)) ++ ", val=" ++ show(fix(b)) ++ "\n") /\
        if fix(b) = true then
            sum(i in index_set(a))( bool2int(a[i]) ) >= 1
        else
            forall(i in index_set(a))( not a[i] )
        endif
    else
    let { var int: x = bool2int(b),
          array[1..length(a)] of var int: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
%  my_trace( "  array_bool_or: \(a), \(b), " ++
%      " array_bool_or(" ++ show(length(a)) ++ "): vars 0..1: \(c), \(x) \n") /\
  if false then
%      my_trace__DBG("  array_bool_or  INDIC: " ++ show(length(a)) ++ " elements. \n") /\
%      int_lin_le_reif__IND( [ -1 | i in index_set(a) ], c, -1, b) /\
    true
  else
    sum(c) >= x /\
    %  sum(c) <= x * length(a)                     % weak
    forall (i in index_set(a)) (x >= c[i])        % strong 
  endif
    endif;

predicate array_bool_and(array[int] of var bool: a, var bool: b) =
%%    if false /*is_fixed(b)*/ then   % frequent case. Does not work -- BUG TODO
    if is_fixed(b) then            % frequent case. Does not work -- BUG TODO
%%        my_trace( "  array_bool_and: \(a), \(b) \n") /\   TODO
%        my_trace( "  array_bool_and FIXED: " ++ show(index_set(a)) ++ ", val=" ++ show(fix(b)) ++ "\n") /\
        if fix(b) = false then
            sum(i in index_set(a))( bool2int(a[i]) ) <= length(a)-1
        else
            forall(i in index_set(a))( a[i] )
        endif
    else
    let { var int: x = bool2int(b),
          array[1..length(a)] of var int: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
%  my_trace( % "  array_bool_and: \(a), \(b), " ++
%      " array_bool_and(" ++ show(length(a)) ++ "): vars 0..1: \(c), \(x) \n") /\
  if false then
%      my_trace__DBG("  array_bool_and  INDIC: " ++ show(length(a)) ++ " elements. \n") /\
%      int_lin_le_reif__IND( [ -1 | i in index_set(a) ], c, -length(a), b) /\
    true
  else
    length(a) - sum(c) >=  1 - x /\
    % length(a) - sum(c) <= (1 - x) * length(a);     % weak
    forall (i in index_set(a)) (x <= c[i])        % strong
  endif
    endif;

% predicate array_bool_xor(array[int] of var bool: a) = .. sum(a) is odd ..
predicate array_bool_xor(array[int] of var bool: a) =
     let { var 0..(length(a)-1) div 2: m,
           var 1..((length(a)-1) div 2)*2+1: ss = sum(i in index_set(a))( bool2int(a[i]) ) }
     in
       ss == 1 + 2 * m;

predicate bool_clause(array[int] of var bool: p, array[int] of var bool: n) =
%   my_trace("  bool_clause: \(p), \(n) \n") /\
 %       my_trace( "  bool_clause: " ++ show(index_set(p)) ++ ", " ++ show(index_set(n)) ++ "\n") /\
      sum(i in index_set(p))( bool2int(p[i]) )
    - sum(i in index_set(n))( bool2int(n[i]) )
    + length(n)
    >= 1;

predicate bool_lin_eq(array[int] of int: c, array[int] of var bool: x, var int: d) :: promise_total =
  sum(i in index_set(x))( c[i]*bool2int(x[i]) ) == d;

predicate bool_lin_eq_reif(array[int] of int: c, array[int] of var bool: x,
                          int: d, var bool: b) =           %% No var int d, sorry
%    my_trace("  bool_lin_eq_reif: \(c), \(x), \(d), \(b) \n") /\
    aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*bool2int(x[i]) ), d, bool2int(b));

    