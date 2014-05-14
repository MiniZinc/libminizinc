%-----------------------------------------------------------------------------%
% Domain encodings
%-----------------------------------------------------------------------------%

% Linear equality encoding

    % Single variable:  x = d  <->  x_eq_d[d]
predicate equality_encoding(var int: x, array[int] of var 0..1: x_eq_d) =
    x in index_set(x_eq_d)
    /\
    sum(d in index_set(x_eq_d))(     x_eq_d[d] ) = 1
    /\
    sum(d in index_set(x_eq_d))( d * x_eq_d[d] ) = x;


    % Array of variables:  x[i] = d  <->  x_eq_d[i,d]
predicate equality_encoding(array[int] of var int: x,
                            array[int, int] of var 0..1: x_eq_d) =
    forall(i in index_set(x))(
        x[i] in index_set_2of2(x_eq_d)
        /\
        sum(d in index_set_2of2(x_eq_d))(     x_eq_d[i,d] ) = 1
        /\
        sum(d in index_set_2of2(x_eq_d))( d * x_eq_d[i,d] ) = x[i]
    );

function array[int] of var 0..1: eq_encode(var int: x) ::promise_total =
  let {
    array[lb(x)..ub(x)] of var 0..1: y;
    constraint equality_encoding(x,y);
  } in y;

function array[int] of int: eq_encode(int: x) ::promise_total =
  array1d(lb(x)..ub(x),[ if i=x then 1 else 0 endif | i in lb(x)..ub(x)]);

function array[int,int] of var int: eq_encode(array[int] of var int: x) ::promise_total =
  let {
    array[index_set(x),lb_array(x)..ub_array(x)] of var 0..1: y =    
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
