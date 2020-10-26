% 位 x 的的总和 = 二元表示的 s。
%            s[0], s[1], ..., s[k] 其中 2^k >= length(x) > 2^(k-1)
predicate binary_sum(array[int] of var bool:x, 
                     array[int] of var bool:s)=
     let { int: l = length(x) } in 
     if l == 1 then s[0] = x[1]
     elseif l == 2 then 
             s[0] = (x[1] xor x[2]) /\ s[1] = (x[1] /\ x[2])
     else let { int: ll = (l div 2),
                array[1..ll] of var bool: f = [ x[i] | i in 1..ll ], 
                array[1..ll] of var bool: t = [x[i]| i in ll+1..2*ll],
                var bool: b = if ll*2 == l then false else x[l] endif,
                int: cp = floor(log2(int2float(ll))),
                array[0..cp] of var bool: fs,
                array[0..cp] of var bool: ts } in
                binary_sum(f, fs) /\ binary_sum(t, ts) /\
                binary_add(fs, ts, b, s)
     endif;

% 把两个二元数值 x 和 y 加起来，位 ci 用来表示进位，来得到二元 s 
predicate binary_add(array[int] of var bool: x,
                     array[int] of var bool: y,
                     var bool: ci,
                     array[int] of var bool: s) =
    let { int:l = length(x), 
          int:n = length(s), } in  
    assert(l == length(y),
          "length of binary_add input args must be same",
    assert(n == l \/ n == l+1, "length of binary_add output " ++
                         "must be equal or one more than inputs",
    let { array[0..l] of var bool: c } in
    full_adder(x[0], y[0], ci, s[0], c[0]) /\
    forall(i in 1..l)(full_adder(x[i], y[i], c[i-1], s[i], c[i])) /\
    if n > l then s[n] = c[l] else c[l] == false endif ));
    
predicate full_adder(var bool: x, var bool: y, var bool: ci,
                     var bool: s, var bool: co) =
          let { var bool: xy = x xor y } in 
          s = (xy xor ci) /\ co = ((x /\ y) \/ (ci /\ xy));
