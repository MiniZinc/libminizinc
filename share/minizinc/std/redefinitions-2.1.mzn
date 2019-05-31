predicate float_in(var float: x, float: a, float: b) =
  x >= a /\ x <= b;

predicate float_dom(var float: x, array[int] of float: as) =
  let {
    array[int] of var bool: b = [float_in(x,as[2*i-1],as[2*i]) | i in 1..length(as) div 2]
  } in exists(b);
