%-----------------------------------------------------------------------------%
% A table constraint table(x, t) represents the constraint x in t where we
% consider each row in t to be a tuple and t as a set of tuples.
%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
% Reified version
%
% We only support special cases of a few variables.
%
% The approach is to add the Boolean variable to the list of variables and
% create an extended table.  The extended table covers all combinations of
% assignments to the original variables, and every entry in it is padded with a
% value that depends on whether that entry occurs in the original table.
%
% For example, the original table constraint
%
%   x y
%   ---
%   2 3
%   5 8
%   4 1
%
% reified with a Boolean b is turned into a table constraint of the form
%
%   x y  b
%   ---------
%   2 3 true
%   5 8 true
%   4 1 true
%   ... false
%   ... false  % for all other pairs (x,y)
%   ... false
%

predicate fzn_table_int_reif(array[int] of var int: x, 
			     array[int, int] of int: t,
                             var bool: b) =

    let { int: n_vars = length(x) }
    in

    assert(n_vars in 1..5,
        "'table' constraints in a reified context " ++
        "are only supported for 1..5 variables.",

    if n_vars = 1 then

        x[1] in { t[it,1] | it in index_set_1of2(t) } <-> b

    else

        let { set of int: ix  = index_set(x),
              set of int: full_size = 1..product(i in ix)( dom_size(x[i]) ),
              array[full_size, 1..n_vars + 1] of int: t_b =
                    array2d(full_size, 1..n_vars + 1,

                    if n_vars = 2 then

                        [ let { array[ix] of int: tpl = [i1,i2] } in
                            (tpl ++ [bool2int(aux_is_in_table(tpl,t))])[p]
                          | i1 in dom(x[1]),
                            i2 in dom(x[2]),
                            p in 1..n_vars + 1 ]

                    else if n_vars = 3 then

                        [ let { array[ix] of int: tpl = [i1,i2,i3] } in
                            (tpl ++ [bool2int(aux_is_in_table(tpl,t))])[p]
                          | i1 in dom(x[1]),
                            i2 in dom(x[2]),
                            i3 in dom(x[3]),
                            p in 1..n_vars + 1 ]

                    else if n_vars = 4 then

                        [ let { array[ix] of int: tpl = [i1,i2,i3,i4] }
                          in
                            (tpl ++ [bool2int(aux_is_in_table(tpl,t))])[p]
                          | i1 in dom(x[1]),
                            i2 in dom(x[2]),
                            i3 in dom(x[3]),
                            i4 in dom(x[4]),
                            p in 1..n_vars + 1 ]

                    else % if n_vars = 5 then

                        [ let { array[ix] of int: tpl = [i1,i2,i3,i4,i5] } in
                            (tpl ++ [bool2int(aux_is_in_table(tpl,t))])[p]
                          | i1 in dom(x[1]),
                            i2 in dom(x[2]),
                            i3 in dom(x[3]),
                            i4 in dom(x[4]),
                            i5 in dom(x[5]),
                            p in 1..n_vars + 1 ]

                    endif endif endif ) }
        in
        fzn_table_int(x ++ [bool2int(b)], t_b)

    endif
    );

test aux_is_in_table(array[int] of int: e, array[int, int] of int: t) =
    exists(i in index_set_1of2(t))(
        forall(j in index_set(e))( t[i,j] = e[j] )
    );

%-----------------------------------------------------------------------------%
