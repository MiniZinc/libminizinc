/***
!Test
solvers: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    aCostSupport: 0
    mdl8_Z: 0
***/

%%%%%% Regression test for the "missing builtin mzn_in_root_context" resolved @227ce089

int: nOBJCOEFDIVISOR__MIP = 10000;	 %%% Objective coefficient divisor to adapt magnitude in MIP
int: nOBJCOEFDIVISOR__CP = 1000;	 %%% Objective coefficient divisor to adapt magnitude in CP
int: nOBJLENGTHDISCR = 1000;	 %%% Length measure discretizer to improve objective precision (in CP)




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONSTRAINTS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  

function var int: CostScaledByDiscrLength(var int: L, float: unit_cost, float: costDivisor, float: lengthDiscr) =
    CostScaledByDiscrLength__useDIV(L, unit_cost, costDivisor, lengthDiscr);

function var int: CostScaledByDiscrLength__useDIV(var int: L, float: unit_cost, float: costDivisor, float: lengthDiscr) =
  let {
    int: lengthDiscrCeil = ceil(lengthDiscr);
  } in
     ((L + lengthDiscrCeil - 1) div lengthDiscrCeil) * ceil(unit_cost * lengthDiscrCeil / costDivisor);

function var int: CostScaledByDiscrLength__useMULT(var int: L, float: unit_cost, float: costDivisor, float: lengthDiscr) =
  let {
    var int: L_scaled;                          %% Manual decomp: OR-TOOLS 8.0.8 says "TIMEOUT, unknown"
    int: lengthDiscrCeil = ceil(lengthDiscr);   %% But it could be better, as suggested by Gurobi
    constraint L <=                             %% Assume L is minimized
      L_scaled * lengthDiscrCeil
    ;
  } in
    L_scaled * ceil(unit_cost * lengthDiscrCeil / costDivisor);


%%%%%%%%%%%%%%%% SUPPORT COSTS %%%%%%%%%%%%%%%%%%%%%%%%%%%
var 0..1000000000: aCostSupport;
var 0..0: mdl8_Z;
constraint
          aCostSupport >= CostScaledByDiscrLength( mdl8_Z,
                                                        42240, nOBJCOEFDIVISOR__CP, nOBJLENGTHDISCR )
;




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SOLVE + OBJECTIVE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


solve
  minimize
    aCostSupport;


