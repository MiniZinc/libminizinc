/***
!Test
expected: !Result
  solution: !Solution
    evedays:
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    flatroster: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    morndays:
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    restdays:
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1]
    roster:
    - [1, 1, 1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1, 1, 1]
    - [1, 1, 1, 1, 1, 1, 1]
***/

% Prior to r8850 mzn2fzn would abort when attempting to flatten the
% seq_search annotation here.

include "globals.mzn" ;

int: weeks = 5;
int: flatsize = 7 * weeks ;

array [1..weeks,1..7] of var 1..5: roster;
array [1..flatsize] of var 1..5: flatroster ;

array [1..7,1..weeks] of var 1..weeks: restdays ;
array [1..7,1..weeks] of var 1..flatsize: morndays ;
array [1..7,1..weeks] of var 1..flatsize: evedays ;

solve
     :: seq_search([
     	int_search([restdays[d,w]|d in 1..7,w in 1..weeks], input_order, indomain_min, complete),
        int_search([morndays[d,w]|d in 1..7,w in 1..weeks], input_order, indomain_min, complete),
        int_search([evedays[d,w]|d in 1..7,w in 1..weeks], input_order, indomain_min, complete),
        int_search(flatroster, first_fail, indomain_min, complete)])
    satisfy;

output ["roster = ", show(roster), ";\n"];
