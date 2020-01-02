/***
!Test
expected:
- !Result
  solution: !Solution
    network__104:
    - - true
      - true
      - true
      - true
      - false
    - - false
      - false
      - true
      - false
      - true
    objective: 6
- !Result
  solution: !Solution
    network__104:
    - - false
      - false
      - true
      - false
      - true
    - - true
      - true
      - true
      - true
      - false
    objective: 6
***/

% Regression test extracted from examples/zinc/sonet.zinc.
% (as processed by zinc2mzn).
% The change in r7995 broke this, it was fixed in r8005, r8014-5.

array[1 .. 5] of 1 .. 5: Demand_1__107 = [ 1, 3, 3, 2, 4 ];

array[1 .. 5] of 1 .. 5: Demand_2__108 = [ 3, 5, 2, 4, 1 ];

array[1 .. 2, 1 .. 5] of var bool: network__104;

constraint 
  forall(
    [ sum([ bool2int(network__104[ring__0, node]) | node in 1 .. 5 ]) <= 4 | 
      ring__0 in 1 .. 2 ]) /\ 
  forall(
    [ let { var 1 .. 2: ring } 
    in 
      network__104[ring, Demand_1__107[e__110]] /\ 
      network__104[ring, Demand_2__108[e__110]] | e__110 in 1 .. 5 ]);

solve 
  minimize 
    sum(
      [ bool2int(network__104[i__118, i__119]) | 
        i__118 in 1 .. 2, i__119 in 1 .. 5 ]);

output 
  [ "Network of size ", 
    show(
      sum(
        [ bool2int(network__104[i__114, i__115]) | 
          i__114 in 1 .. 2, i__115 in 1 .. 5 ])), ":\n", "{ " ] ++ 
  [ ( if   fix(network__104[i__116, i__117]) 
      then ("(" ++ (((show(i__116) ++ ", ") ++ show(i__117)) ++ ")")) ++ " " 
      else "" endif) | i__116 in 1 .. 2, i__117 in 1 .. 5 ] ++ [ "}" ] ++
  [ "\n" ];
