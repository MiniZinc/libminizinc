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

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
