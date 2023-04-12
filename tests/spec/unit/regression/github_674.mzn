/***
!Test
solvers: [gecode, chuffed, highs]
expected: !Result
  solution: !Solution
    arr: [true, false]
    ns: [false, false, false, false, false, false, false, false, false, true, false]
    es: [false, false, false, false, false, false, false, false, false, false, false]
***/
include "fzn_dtree_enum.mzn";

array[1..11] of int: from = [1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10];
array[1..11] of int: to   = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0];

array[1..2] of var bool: arr;
array[0..10] of var bool: ns;
array[1..11] of var bool: es;

constraint not ns[0];
constraint count([ns[1]]) = 0;
constraint ns[1] <-> not arr[1];
constraint ns[9] <-> not arr[2];
constraint es[3] <-> ns[1] /\ ns[9];

tuple(int, array[int] of var bool, array[int] of var bool): t = (0, ns, es);

constraint fzn_tree2(from, to, 9, t.2, t.3);

predicate fzn_tree2(array[int] of int: from, array[int] of int: to,
                   var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        int: E = length(es);
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des;
        var bool: x;
    } in
        /* ensure that the directed edges selected agree with undirected edges */
        %     forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E])) /\
        (x = forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E]))) /\ x /\
        /* duplicate the edges so that the we can use directed graph reachability */
        fzn_dtree(dfrom,dto,r,ns,des);

solve satisfy;