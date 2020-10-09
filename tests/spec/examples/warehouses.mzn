/***
!Test
expected:
- !Result
  solution: !Solution
    Total: 383
    cost: [30, 27, 70, 2, 4, 22, 5, 13, 35, 55]
    objective: 383
    open: [true, true, true, false, true]
    supplier: [5, 2, 5, 1, 5, 2, 2, 3, 2, 3]
***/

%-----------------------------------------------------------------------------
% Warehouse allocation
% (Problem 034 in CSPLib)
% vim: ft=zinc ts=2 sw=2 et tw=0
%
% Guido Tack, tack@gecode.org
% 2007-02-22
%
% Ported from the Gecode example
%-----------------------------------------------------------------------------
% A company needs to construct warehouses to supply stores with goods.  Each
% warehouse possibly to be constructed has a certain capacity defining how many
% stores it can supply.  Constructing a warehouse incurs a fixed cost.  Costs
% for transportation from warehouses to stores depend on the locations of
% warehouses and stores.
%
% Determine which warehouses should be constructed and which warehouse should
% supply which store such that overall cost (transportation cost plus
% construction cost) is smallest.
%-----------------------------------------------------------------------------

include "globals.mzn";

%-----------------------------------------------------------------------------
% Instance

n_suppliers = 5;
n_stores = 10;
building_cost = 30;

capacity = [1,4,2,1,3];

cost_matrix =
 [|20, 24, 11, 25, 30
  |28, 27, 82, 83, 74
  |74, 97, 71, 96, 70
  | 2, 55, 73, 69, 61
  |46, 96, 59, 83,  4
  |42, 22, 29, 67, 59
  | 1,  5, 73, 59, 56
  |10, 73, 13, 43, 96
  |93, 35, 63, 85, 46
  |47, 65, 55, 71, 95|]; 

%-----------------------------------------------------------------------------
% Model

int: n_suppliers;
int: n_stores;
int: building_cost;
array[1..n_suppliers] of int: capacity;
array[1..n_stores,1..n_suppliers] of int: cost_matrix;

int: MaxCost = max(i in 1..n_stores, j in 1..n_suppliers)(cost_matrix[i,j]);
int: MaxTotal =   (n_suppliers * building_cost) 
                + sum(i in 1..n_stores, j in 1..n_suppliers)(cost_matrix[i,j]); 

array[1..n_stores] of var 1..n_suppliers: supplier;
array[1..n_suppliers] of var bool: open;
array[1..n_stores] of var 1..MaxCost: cost;
var 1..MaxTotal: Total;

constraint
  sum (i in 1..n_suppliers) (building_cost * bool2int(open[i])) +
  sum (i in 1..n_stores) (cost[i])
  = Total;

constraint
  forall (i in 1..n_stores) (
    cost_matrix[i,supplier[i]] = cost[i]
  );

constraint
  forall (i in 1..n_suppliers) (
    let {
      var int: use
     } in
    count(supplier,i,use) /\ use <= capacity[i]
  );

constraint
  forall (i in 1..n_suppliers) (
    (exists (j in 1..n_stores) (supplier[j] == i)) == open[i]
  );

solve
  :: int_search(
    supplier ++ cost ++ [bool2int(open[i]) | i in 1..n_suppliers],
    first_fail,
    indomain_split,
    complete
  )
  minimize Total;

output
  [ "warehouses:" ]
  ++
  [ "\nTotal = ", show(Total) ]
  ++
  [ "\nsupplier = [\n" ]
  ++
  [ "\t" ++ show(supplier[i]) ++
    if i = n_stores then "\n]"
    elseif i mod 5 = 0 then ",\n"
    else ","
    endif
  | i in 1..n_stores
  ]
  ++
  [ "\ncost = [\n" ]
  ++
  [ "\t" ++ show(cost[i]) ++
    if i = n_stores then "\n]"
    elseif i mod 5 = 0 then ",\n"
    else ","
    endif
  | i in 1..n_stores
  ]
  ++
  [ "\nopen = [\n" ]
  ++
  [ "\t" ++ show(open[i]) ++
    if i = n_suppliers then "\n]\n"
    elseif i mod 5 = 0 then ",\n"
    else ","
    endif
  | i in 1..n_suppliers
  ]

%-----------------------------------------------------------------------------
%-----------------------------------------------------------------------------
