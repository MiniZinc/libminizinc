%--------------------------------------------------------------------%
% 需要给出一个任务集合，其中起始时间为's', 
% 持续时间'd'以及资源需求量'r'，
% 任何时候需求量都不能超过
% 一个全局资源界限'b'。
% 假设：
% - forall i, d[i] >= 0 and r[i] >= 0
%--------------------------------------------------------------------%
predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
   assert(index_set(s) == index_set(d) /\ 
          index_set(s) == index_set(r),
     "cumulative: the array arguments must have identical index sets",
   assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
     "cumulative: durations and resource usages must be non-negative",
           let { 
               set of int: times =
                 lb_array(s) ..
                 max([ ub(s[i]) + ub(d[i]) | i in index_set(s) ]) 
               } 
           in
              forall( t in times ) (
                 b >= sum( i in index_set(s) ) (
                    bool2int( s[i] <= t /\ t < s[i] + d[i] ) * r[i]
                 )
              )
       )
   );
