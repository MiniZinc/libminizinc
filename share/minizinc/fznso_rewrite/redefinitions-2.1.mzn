% Shadows `std`'s file of the same name. `float_in` is a registry constraint;
% `float_dom` is not, so it keeps `std`'s definition — which reaches the registry
% constraint anyway, through the `float_in` calls in its body.

include "fzn_float_in.mzn";

predicate float_in(var float: x, float: l, float: u) = fzn_float_in(x, l, u);

predicate float_dom(var float: x, array [int] of float: as) =
  let {
    array [int] of var bool: b = [
      float_in(x, as[2 * i - 1], as[2 * i]) |
        i in 1..length(as) div 2,
    ];
  } in exists(b);
