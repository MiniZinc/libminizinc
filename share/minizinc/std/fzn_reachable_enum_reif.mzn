predicate fzn_reachable_reif(
  array [int] of int: from,
  array [int] of int: to,
  var int: r,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) =
  let {
    int: E = length(es);
    set of int: NODE = index_set(ns);
    array [1..2 * E] of NODE: dfrom = from ++ to;
    array [1..2 * E] of NODE: dto = to ++ from;
    array [1..2 * E] of var bool: des = es ++ es;
    array [NODE] of var bool: dns = array1d(NODE, ns);
    var NODE: dr = r;
  } in /* duplicate the edges so that we can use directed graph reachability */
  b <-> fzn_dreachable(dfrom, dto, dr, dns, des);

%-----------------------------------------------------------------------------%
