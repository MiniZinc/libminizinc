/*%-----------------------------------------------------------------------------%
% Domain encodings
%-----------------------------------------------------------------------------%
*/
% Linear equality encoding

    % Single variable:  x = d  <->  x_eq_d[d]
predicate equality_encoding(var int: x, array[int] of var int: x_eq_d) =
    x in index_set(x_eq_d)
    /\
    sum(d in dom(x))(     x_eq_d[d] ) = 1
    /\
    sum(d in dom(x))( d * x_eq_d[d] ) = x
    /\
%    my_trace( "eq_enc: \(x), index_set(pp)=" ++ show(index_set( x_eq_d )) ++ "\n" ) /\
    if fPostprocessDomains then
      equality_encoding__POST(x, x_eq_d)
    else true endif
    ;

    % Two variables:  x = d /\ y = e  <->  x_eq_d[d] /\ y_eq_e[e] /\ xy_eq_de[d, e]
predicate equality_encoding(var int: x, var int: y,
      array[int] of var int: x_eq_d,
      array[int] of var int: y_eq_e,
      array[int, int] of var int: xy_eq_de
      ) =
    x in index_set(x_eq_d) /\
    y in index_set(y_eq_e) /\
    index_set(x_eq_d) == index_set_1of2(xy_eq_de) /\
    index_set(y_eq_e) == index_set_2of2(xy_eq_de) /\
    sum(d in dom(x), e in dom(y))( xy_eq_de[d, e] ) = 1
    /\
    forall(d in dom(x)) (sum(e in dom(y))( xy_eq_de[d, e] ) = x_eq_d[d])
    /\
    forall(e in dom(y)) (sum(d in dom(x))( xy_eq_de[d, e] ) = y_eq_e[e])
    ;

    % Array of variables:  x[i] = d  <->  x_eq_d[i,d]
predicate equality_encoding(array[int] of var int: x,
                            array[int, int] of var int: x_eq_d) =
    forall(i in index_set(x))(
        x[i] in index_set_2of2(x_eq_d)
        /\
        sum(d in index_set_2of2(x_eq_d))(     x_eq_d[i,d] ) = 1
        /\
        sum(d in index_set_2of2(x_eq_d))( d * x_eq_d[i,d] ) = x[i]
    );

function var int: eq_new_var(var int: x, int: i) ::promise_total =
  if i in dom(x) then
  let {
    var 0..1: xi;
  } in xi
  else 0 endif;

function array[int] of var int: eq_encode(var int: x) ::promise_total =
  let {
    array[int] of var int: y = array1d(lb(x)..ub(x),[eq_new_var(x,i) | i in lb(x)..ub(x)]);
    constraint equality_encoding(x,y);
%    constraint
%      if card(dom(x))>0 then
%        my_trace("  eq_encode: dom(\(x)) = " ++ show(dom(x)) ++ ",  card( dom(\(x)) ) = " ++ show(card(dom(x))) ++ "\n")
%      else true endif;
 %%   constraint assert(card(dom(x))>1, "  eq_encode: card(dom(\(x))) == " ++ show(card(dom(x))));
  } in  y;

function array[int] of int: eq_encode(int: x) ::promise_total =
  array1d(lb(x)..ub(x),[ if i=x then 1 else 0 endif | i in lb(x)..ub(x)]);

%%% The same for 2 variables:
function var int: eq_new_var(var int: x, int: i, var int: y, int: j) ::promise_total =
  if i in dom(x) /\ j in dom(y) then
  let {
    var 0..1: xi;
  } in xi
  else 0 endif;

function array[int, int] of var int: eq_encode(var int: x, var int: y) ::promise_total =
  let {
    array[int] of var int: pX = eq_encode(x),
    array[int] of var int: pY = eq_encode(y),
    array[int, int] of var int: pp = array2d(index_set(pX), index_set(pY),
      [eq_new_var(x,i,y,j) | i in index_set(pX), j in index_set(pY)]);
    constraint equality_encoding(x, y, pX, pY, pp);
  } in pp;

function array[int, int] of int: eq_encode(int: x, int: y) ::promise_total =
%  let {
%    constraint       if card(dom(x))*card(dom(y))>200 then
%      my_trace("  eq_encode: dom(\(x)) = " ++ show(dom(x)) ++ ", dom(\(y)) = " ++ show(dom(y)) ++ "\n")
%      else true endif;
%  } in
    array2d(lb(x)..ub(x), lb(y)..ub(y),
      [if i==x /\ j==y then 1 else 0 endif | i in lb(x)..ub(x), j in lb(y)..ub(y)]);

function array[int,int] of var int: eq_encode(array[int] of var int: x) ::promise_total =
  let {
    array[index_set(x),lb_array(x)..ub_array(x)] of var int: y =
      array2d(index_set(x),lb_array(x)..ub_array(x),
      [ let {
          array[int] of var int: xi = eq_encode(x[i])
        } in if j in index_set(xi) then xi[j] else 0 endif
        | i in index_set(x), j in lb_array(x)..ub_array(x)]
      )
  } in y;

function array[int,int] of int: eq_encode(array[int] of int: x) ::promise_total =
  array2d(index_set(x),lb_array(x)..ub_array(x),[ if j=x[i] then 1 else 0 endif | i in index_set(x), j in lb_array(x)..ub_array(x)]);

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
