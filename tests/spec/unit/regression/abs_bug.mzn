/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: |
      [4, 10]
      {4,10,13}
      {1,3,4}
      depot(a,4).
      depot(c,10).

***/

% Regression test.
% The bounds inferred for absolute values expressions where incorrect in r13710
% and before.  

int: nr; % number of restaurants
set of int: Restaurant = 1..nr;
array[Restaurant] of string: name :: add_to_output;

array[Restaurant] of 0..910: k; % kilometre position
set of int: ks :: add_to_output = { k[r] | r in Restaurant };

set of Restaurant: first :: add_to_output = { min(r in Restaurant where k[r] == pos)(r) | pos in ks };

int: number_of_depots;
set of int: Depot = 1..number_of_depots;
array[Depot] of var 0..910: p :: add_to_output; % position of depot

constraint forall(d in Depot)(p[d] in ks);
constraint forall(d in 1..number_of_depots-1)(p[d] < p[d+1]);

solve minimize sum(r in Restaurant)(min(d in Depot)(abs(p[d] - k[r])));
      %satisfy;

output [ show(p), "\n", show(ks), "\n", show(first), "\n"]++
       [ if (fix(p[d]) == k[r]) then
       	    "depot(" ++ name[r] ++ "," ++ show(p[d]) ++ ").\n"
         else "" endif | d in Depot, r in first ];

nr = 4;                                                                                                                                                                                                                                                        
name = ["a", "b", "c", "d"];                                                                                                                                                                                                                                   
k = [4,4,10,13];                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                               
number_of_depots = 2;                                                                                                                                                                                                                                          
