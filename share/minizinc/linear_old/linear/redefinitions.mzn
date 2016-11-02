%-----------------------------------------------------------------------------%
% FlatZinc built-in redefinitions for linear solvers.
%
% Sebastian Brand
% Gleb Belov     Corrected array_var_float_element and float_lin_lt_reif
%-----------------------------------------------------------------------------%

function var bool: reverse_map(var int: x) = (x==1);
function bool: reverse_map(int: x) = (x==1);

function var int: bool2int(var bool: x) :: promise_total =
 let { var 0..1: b2i;
       constraint (x = reverse_map(b2i)) ::is_reverse_map ;
      } in b2i;

predicate bool_eq(var bool: x, var bool: y) =
  bool2int(x)==bool2int(y);

%-----------------------------------------------------------------------------%
% Strict inequality
%
% Uncomment the following redefinition for FlatZinc MIP solver interfaces that
% do not support strict inequality.  Note that it does not preserve equivalence
% (some solutions of the original problem may become invalid).

% predicate float_lt(var float: x, var float: y) = x + 1e-06 <= y;

%-----------------------------------------------------------------------------%
%
% Logic operations
%
%-----------------------------------------------------------------------------%

predicate bool_not(var bool: p, var bool: q) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q) }
    in
    x + y = 1;


predicate bool_and(var bool: p, var bool: q, var bool: r) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q),
          var 0..1: z = bool2int(r) }
    in
    x + y <= z + 1 /\
    x + y >= z * 2;
    % x >= z /\ y >= z;     % alternative


predicate bool_or(var bool: p, var bool: q, var bool: r) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q),
          var 0..1: z = bool2int(r) }
    in
    x + y >= z /\
    x + y <= z * 2;
    % x <= z /\ y <= z;     % alternative


predicate bool_xor(var bool: p, var bool: q, var bool: r) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q),
          var 0..1: z = bool2int(r) }
    in
    x <= y + z /\
    y <= x + z /\
    z <= x + y /\
    x + y + z <= 2;


predicate bool_eq_reif(var bool: p, var bool: q, var bool: r) =
    if is_fixed(q) then   % frequent case
        if fix(q) = true then p = r else bool_not(p,r) endif
    else
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q),
          var 0..1: z = bool2int(r) }
    in
    x + y <= z + 1 /\
    x + z <= y + 1 /\
    y + z <= x + 1 /\
    x + y + z >= 1
    endif;


predicate bool_ne_reif(var bool: p, var bool: q, var bool: r) =
    bool_xor(p, q, r);


predicate bool_le(var bool: p, var bool: q) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q) }
    in
    x <= y;


predicate bool_le_reif(var bool: p, var bool: q, var bool: r) =
    let { var 0..1: x = bool2int(p),
          var 0..1: y = bool2int(q),
          var 0..1: z = bool2int(r) }
    in
    1 - x + y >= z /\
    1 - x + y <= z * 2;
    % 1 - x <= z /\ y <= z; % alternative


predicate bool_lt(var bool: p, var bool: q) =
    not p /\ q;


predicate bool_lt_reif(var bool: p, var bool: q, var bool: r) =
    (not p /\ q) <-> r;

%-----------------------------------------------------------------------------%

predicate array_bool_or(array[int] of var bool: a, var bool: b) =
    if is_fixed(b) then   % frequent case
        if fix(b) = true then
            sum(i in index_set(a))( bool2int(a[i]) ) >= 1
        else
            forall(i in index_set(a))( not a[i] )
        endif
    else
    let { var 0..1: x = bool2int(b),
          array[1..length(a)] of var 0..1: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
    sum(c) >= x /\
    sum(c) <= x * length(a)
    endif;

predicate array_bool_and(array[int] of var bool: a, var bool: b) =
    let { var 0..1: x = bool2int(b),
          array[1..length(a)] of var 0..1: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
    length(a) - sum(c) >=  1 - x /\
    length(a) - sum(c) <= (1 - x) * length(a);

predicate array_bool_xor(array[int] of var bool: a) =
     let { var 0..length(a): m }
     in
     sum(i in 1..length(a))( bool2int(a[i]) ) = 1 + 2 * m;

predicate bool_clause(array[int] of var bool: p, array[int] of var bool: n) =
      sum(i in index_set(p))( bool2int(p[i]) )
    - sum(i in index_set(n))( bool2int(n[i]) )
    + length(n)
    >= 1;

% predicate array_bool_xor(array[int] of var bool: a) = .. sum(a) is odd ..

%-----------------------------------------------------------------------------%
%
% Linear equations and inequations
%
%-----------------------------------------------------------------------------%

predicate int_le_reif(var int: x, var int: y, var bool: b) =
    let { var 0..1: p = bool2int(b) }
    in
    aux_int_le_if_1(x, y, p) /\
    aux_int_gt_if_0(x, y, p);


predicate int_lt_reif(var int: x, var int: y, var bool: b) =
    int_le_reif(x, y - 1, b);


predicate int_ne(var int: x, var int: y) =
    let { var 0..1: p }
    in
    aux_int_lt_if_1(x, y, p) /\
    aux_int_gt_if_0(x, y, p);

predicate int_lin_ne(array[int] of int: c, array[int] of var int: x, int: d) =
    int_ne(sum(i in index_set(x))( c[i]*x[i] ),d);

predicate int_eq_reif(var int: x, var int: y, var bool: b) =
    aux_int_eq_iff_1(x, y, bool2int(b));


predicate int_ne_reif(var int: x, var int: y, var bool: b) =
    aux_int_eq_iff_1(x, y, 1 - bool2int(b));

%-----------------------------------------------------------------------------%

predicate int_lin_eq_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, bool2int(b));


predicate int_lin_ne_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, 1 - bool2int(b));


predicate int_lin_le_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    let { var 0..1: p = bool2int(b) }
    in
    aux_int_le_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
    aux_int_gt_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p);


predicate int_lin_lt_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    int_lin_le_reif(c, x, d - 1, b);

%-----------------------------------------------------------------------------%

predicate float_le_reif(var float: x, var float: y, var bool: b) =
    let { var 0..1: p = bool2int(b) }
    in
    aux_float_le_if_1(x, y, int2float(p)) /\
    aux_float_gt_if_0(x, y, int2float(p));


predicate float_lt_reif(var float: x, var float: y, var bool: b) =
    let { var 0..1: p = bool2int(b) }
    in
    aux_float_lt_if_1(x, y, int2float(p)) /\
    aux_float_ge_if_0(x, y, int2float(p));


predicate float_ne(var float: x, var float: y) =
    let { var 0..1: p }
    in
    aux_float_lt_if_1(x, y, int2float(p)) /\
    aux_float_gt_if_0(x, y, int2float(p));


predicate float_eq_reif(var float: x, var float: y, var bool: b) =
    aux_float_eq_iff_1(x, y, int2float(bool2int(b)));


predicate float_ne_reif(var float: x, var float: y, var bool: b) =
    aux_float_eq_iff_1(x, y, 1.0 - int2float(bool2int(b)));

%-----------------------------------------------------------------------------%

predicate float_lin_eq_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d,
        int2float(bool2int(b)));


predicate float_lin_ne_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d,
        1.0 - int2float(bool2int(b)));


predicate float_lin_le_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    let { var 0.0..1.0: p = int2float(bool2int(b)) }
    in
    aux_float_le_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
    aux_float_gt_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p);


predicate float_lin_lt_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    let { var 0.0..1.0: p = int2float(bool2int(b)) }
    in
    aux_float_lt_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
    aux_float_ge_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p);

%-----------------------------------------------------------------------------%
% Minimum, maximum, absolute value

predicate int_abs(var int: x, var int: z) =
    let { var 0..1: p }
    in
    % z <= x \/ z <= -x
    aux_int_le_if_1(z,  x, p) /\
    aux_int_le_if_0(z, -x, p) /\
    z >=  x /\
    z >= -x /\
    z >= 0;


predicate int_min(var int: x, var int: y, var int: z) =
    let { var 0..1: p }
    in
    % z >= x \/ z >= y
    aux_int_ge_if_1(z, x, p) /\
    aux_int_ge_if_0(z, y, p) /\
    z <= x /\
    z <= y;


predicate int_max(var int: x, var int: y, var int: z) =
    let { var 0..1: p }
    in
    % z <= x \/ z <= y
    aux_int_le_if_1(z, x, p) /\
    aux_int_le_if_0(z, y, p) /\
    z >= x /\
    z >= y;


predicate float_abs(var float: x, var float: z) =
    let { var 0..1: p }
    in
    % z <= x \/ z <= -x
    aux_float_le_if_1(z,  x, int2float(p)) /\
    aux_float_le_if_0(z, -x, int2float(p)) /\
    z >=  x /\
    z >= -x /\
    z >= 0.0;


predicate float_min(var float: x, var float: y, var float: z) =
    let { var 0..1: p }
    in
    % z >= x \/ z >= y
    aux_float_ge_if_1(z, x, int2float(p)) /\
    aux_float_ge_if_0(z, y, int2float(p)) /\
    z <= x /\
    z <= y;


predicate float_max(var float: x, var float: y, var float: z) =
    let { var 0..1: p }
    in
    % z <= x \/ z <= y
    aux_float_le_if_1(z, x, int2float(p)) /\
    aux_float_le_if_0(z, y, int2float(p)) /\
    z >= x /\
    z >= y;

%-----------------------------------------------------------------------------%
% Multiplication and division

predicate int_div(var int: x, var int: y, var int: q) =
    let { var 0..max(abs(lb(y)), abs(ub(y))) - 1: r }
    in
    aux_int_division_modulo(x,y,q,r);


predicate int_mod(var int: x, var int: y, var int: r) =
    let { 
      int: bx = max(abs(lb(x)), abs(ub(x)));
      var -bx..bx: q;
      int: by = max(abs(lb(y)), abs(ub(y)));
      constraint r in -by..by;
    }
    in
    aux_int_division_modulo(x,y,q,r);


predicate aux_int_division_modulo(var int: x, var int: y, var int: q,
        var int: r) =
    x = y * q + r /\
    let { array[1..2] of var 0..1: p }
    in
    % 0 < x -> 0 <= r    which is    0 >= x \/ 0 <= r
    aux_int_le_if_1(x, 0, p[1]) /\
    aux_int_ge_if_0(r, 0, p[1]) /\
    % x < 0 -> r <= 0    which is    x >= 0 \/ r <= 0
    aux_int_ge_if_1(x, 0, p[2]) /\
    aux_int_le_if_0(r, 0, p[2]) /\
    % abs(r) < abs(y)
    let { var 1.. max(abs(lb(y)), abs(ub(y))): w = abs(y) }
    in
    w >  r /\
    w > -r;


predicate int_times(var int: x, var int: y, var int: z) =
    if card(dom(x)) > card(dom(y)) then int_times(y,x,z)
    else
    let { set of int: s = lb(x)..ub(x),
          set of int: r = {lb(x)*lb(y), lb(x)*ub(y), ub(x)*lb(y), ub(x)*ub(y)},
          array[s] of var min(r)..max(r): ady = array1d(s, [ d*y | d in s ]) }
    in
    ady[x] = z
    endif;

%-----------------------------------------------------------------------------%
% Array 'element' constraints

predicate array_bool_element(var int: x, array[int] of bool: a, var bool: z) =
    x in index_set(a) /\
    forall(d in index_set(a))( x = d -> a[d] = z );


predicate array_var_bool_element(var int: x, array[int] of var bool: a,
                                 var bool: z) =
    x in index_set(a) /\
    forall(d in index_set(a))( x = d -> a[d] = z );


predicate array_int_element(var int: x, array[int] of int: a, var int: z) =
    x in index_set(a) /\
    forall(d in index_set(a))( x = d -> a[d] = z );

predicate array_var_int_element(var int: x, array[int] of var int: a,
                                var int: z) =
    x in index_set(a) /\
    forall(d in index_set(a))( x = d -> a[d] = z );


predicate array_float_element(var int: x, array[int] of float: a,
                              var float: z) =
    let { set of int: ix = index_set(a),
          array[ix] of var 0..1: x_eq_d }
    in
    sum(i in ix)(     x_eq_d[i] ) = 1
    /\
    sum(i in ix)( i * x_eq_d[i] ) = x
    /\
    sum(i in ix)( a[i] * int2float(x_eq_d[i]) ) = z;


predicate array_var_float_element(var int: x, array[int] of var float: a,
                                  var float: z) =
    let { set of int: ix = index_set(a),
          array[ix] of var 0..1: x_eq_d }
    in
    sum(i in ix)(     x_eq_d[i] ) = 1
    /\
    sum(i in ix)( i * x_eq_d[i] ) = x
    /\
    forall(i in ix)(
        % x_eq_d[i] -> a[i] = a2[i]
        a[i] - z >= (lb(a[i])-ub(z))*int2float(1-x_eq_d[i])
        /\
        z - a[i] >= (lb(z)-ub(a[i]))*int2float(1-x_eq_d[i])
    );

%-----------------------------------------------------------------------------%
% Domain constraints

% XXX  only for a fixed set

predicate set_in(var int: x, set of int: s) =
    if s = min(s)..max(s) then
        min(s) <= x /\ x <= max(s)
    else
        exists(e in s)( x = e )
    endif;

% XXX  only for a fixed set
predicate set_in_reif(var int: x, set of int: s, var bool: b) =
    b <->
        exists(i in 1..length([ 0 | e in s where not (e - 1 in s) ]))(
            let { int: l = [ e | e in s where not (e - 1 in s) ][i],
                  int: r = [ e | e in s where not (e + 1 in s) ][i] }
            in
            l <= x /\ x <= r
        );

    % Alternative
predicate alt_set_in_reif(var int: x, set of int: s, var bool: b) =
    b <->
        if s = min(s)..max(s) then
            min(s) <= x /\ x <= max(s)
        else
            exists(e in s)( x = e )
        endif;

%-----------------------------------------------------------------------------%
% Auxiliary: equality reified onto a 0/1 variable

predicate aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
    let { array[1..2] of var 0..1: q_458 }
    in
    aux_int_lt_if_0(x - p, y, q_458[1]) /\
    aux_int_gt_if_0(x + p, y, q_458[2]) /\
    sum(q_458) <= 2 - 2*p /\
    sum(q_458) <= 1 + p;

    % Alternative 1
predicate alt_1_aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
    let { array[1..2] of var 0..1: q }
    in
    aux_int_lt_if_0(x - p, y, q[1]) /\
    aux_int_gt_if_0(x + p, y, q[2]) /\
    q[1] <= 1 - p /\
    q[2] <= 1 - p /\
    sum(q) <= 1 + p;

    % Alternative 2
predicate alt_2_aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
    let { array[1..2] of var 0..1: q }
    in
    aux_int_le_if_1(x, y, p) /\
    aux_int_ge_if_1(x, y, p) /\
    aux_int_lt_if_0(x, y, q[1]) /\
    aux_int_gt_if_0(x, y, q[2]) /\
    sum(q) <= p + 1;


predicate aux_float_eq_iff_1(var float: x, var float: y, var float: p) =
    let { array[1..2] of var 0..1: q }
    in
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p) /\
    aux_float_lt_if_0(x, y, int2float(q[1])) /\
    aux_float_gt_if_0(x, y, int2float(q[2])) /\
    int2float(sum(q)) <= 1.0 + p;

%-----------------------------------------------------------------------------%
% Auxiliary: indicator constraints
%   p -> x # 0  where p is a 0/1 variable and # is a comparison

% Base cases

predicate aux_int_le_zero_if_0(var int: x, var int: p) =
    x <= ub(x) * p;

predicate aux_float_le_zero_if_0(var float: x, var float: p) =
    x <= ub(x) * p;

predicate aux_float_lt_zero_if_0(var float: x, var float: p) =
    let { float: rho = 1e-02 * abs(ub(x)) }  % same order of magnitude as ub(x)
    in
    x < (ub(x) + rho) * p;


% Derived cases

predicate aux_int_le_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y, p);

predicate aux_int_ge_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x, p);

predicate aux_int_le_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y, 1 - p);

predicate aux_int_ge_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x, 1 - p);

predicate aux_int_lt_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y + 1, p);

predicate aux_int_gt_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x + 1, p);

predicate aux_int_lt_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y + 1, 1 - p);


predicate aux_float_le_if_0(var float: x, var float: y, var float: p) =
    aux_float_le_zero_if_0(x - y, p);

predicate aux_float_ge_if_0(var float: x, var float: y, var float: p) =
    aux_float_le_zero_if_0(y - x, p);

predicate aux_float_le_if_1(var float: x, var float: y, var float: p) =
    aux_float_le_zero_if_0(x - y, 1.0 - p);

predicate aux_float_ge_if_1(var float: x, var float: y, var float: p) =
    aux_float_le_zero_if_0(y - x, 1.0 - p);

predicate aux_float_lt_if_0(var float: x, var float: y, var float: p) =
    aux_float_lt_zero_if_0(x - y, p);

predicate aux_float_gt_if_0(var float: x, var float: y, var float: p) =
    aux_float_lt_zero_if_0(y - x, p);

predicate aux_float_lt_if_1(var float: x, var float: y, var float: p) =
    aux_float_lt_zero_if_0(x - y, 1.0 - p);

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

annotation bool_search(array[int] of var bool: x, ann:a1, ann:a2, ann:a3) =
  int_search([bool2int(x[i]) | i in index_set(x)],a1,a2,a3);

predicate array_int_maximum(var int: m, array[int] of var int: x) =
    let { int: l = min(index_set(x)),
          int: u = max(index_set(x)),
          int: ly = lb_array(x),
          int: uy = ub_array(x),
          array[l..u] of var ly..uy: y } in
    y[l] = x[l] /\
    m = y[u] /\
    forall (i in l+1 .. u) ( y[i] == max(x[i],y[i-1]) );

predicate array_float_maximum(var float: m, array[int] of var float: x) =
    let { int: l = min(index_set(x)),
          int: u = max(index_set(x)),
          float: ly = lb_array(x),
          float: uy = ub_array(x),
          array[l..u] of var ly..uy: y } in
    y[l] = x[l] /\
    m = y[u] /\
    forall (i in l+1 .. u) ( y[i] == max(x[i],y[i-1]) );

predicate array_int_minimum(var int: m, array[int] of var int: x) =
    let { int: l = min(index_set(x)),
          int: u = max(index_set(x)),
          int: ly = lb_array(x),
          int: uy = ub_array(x),
          array[l..u] of var ly..uy: y } in
    y[l] = x[l] /\
    m = y[u] /\
    forall (i in l+1 .. u) ( y[i] == min(x[i],y[i-1]) );

predicate array_float_minimum(var float: m, array[int] of var float: x) =
    let { int: l = min(index_set(x)),
          int: u = max(index_set(x)),
          float: ly = lb_array(x),
          float: uy = ub_array(x),
          array[l..u] of var ly..uy: y } in
    y[l] = x[l] /\
    m = y[u] /\
    forall (i in l+1 .. u) ( y[i] == min(x[i],y[i-1]) );

mzn_opt_only_range_domains = true;
