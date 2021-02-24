/*
% Controls
%
*/

%-----------------------------------------------------------------------------%
%---------- USER and LAZY CUTS -----------------------------------------------%
/*
   PLEASE NOTE:
   If you export FZN file with lazy_constraint/user_cut annotations,
   their declarations are not exported currently (as of 7.11.17).
   WORKAROUND: when solving that fzn, add -G linear,
   e.g., as follows: mzn-cplex -G linear model.fzn
 * For Gurobi, the constraints marked as MIP_cut and/or MIP_lazy are added
 * into the overall model and marked with the foll values of Lazy attribute:
 *   ::MIP_lazy             1
 *   ::MIP_cut ::MIP_lazy   2
 *   ::MIP_cut              3
 */
ann: user_cut;
ann: lazy_constraint;
%%% comment away the below assignments (leaving, e.g., ann: MIP_cut;) to have them as normal constraints  
%%% In particular, they may be used by redundant_constraint() and symmetry_breaking_constraint(), see redefs-2.0.2.mzn
ann: MIP_cut = user_cut;         %% MIP_cut: make sure no feasible solutions are cut off
                                 %% -- seems better on average but in CPLEX, wrong LB e.g. on carpet-cutting
ann: MIP_lazy = lazy_constraint;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% "GENERAL" constraints %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
opt bool: fIndConstr;          %% User option, e.g., with -D
                               %% Attention: as of MZN 2.4.3, you also need -DfMIPdomains=false
bool: fMZN__UseIndicators =    %% Pass on indicator constraints
           if absent( fIndConstr ) then false
           else deopt( fIndConstr ) endif;
    %% CPLEX 12.6.2 Concert: reifs give wrong result on 2012/amaze, so using implications only
    
%% MAX/MIN
opt bool: MinMaxGeneral;       %% User option, e.g., with -D
                               %% pass on min/max to the backend as fzn_array_float_minimum
bool: MZN__MinMaxGeneral = if absent(MinMaxGeneral) then false else deopt(MinMaxGeneral) endif;

%% CUMULATIVE
opt bool: CumulativeSolverConfig;               %% As set in share/minizinc/Preferences.json
opt bool: UseCumulative;       %% User option, e.g., with -D
bool: MZN__Cumulative_Fixed_d_r = if occurs(UseCumulative) then deopt(UseCumulative)
                      elseif occurs(CumulativeSolverConfig) then deopt(CumulativeSolverConfig)
                      else false endif;

%% ORBISACK
opt bool: OrbisackSolverConfig;               %% As set in share/minizinc/Preferences.json
opt bool: UseOrbisack;          %% User option, e.g., with -D
bool: MZN__Orbisack = if occurs(UseOrbisack) then deopt(UseOrbisack)
                      elseif occurs(OrbisackSolverConfig) then deopt(OrbisackSolverConfig)
                      else false endif;

%% ORBITOPE
opt bool: OrbitopeSolverConfig;               %% As set in share/minizinc/Preferences.json
opt bool: UseOrbitope;          %% User option, e.g., with -D
bool: MZN__Orbitope = if occurs(UseOrbitope) then deopt(UseOrbitope)
                      elseif occurs(OrbitopeSolverConfig) then deopt(OrbitopeSolverConfig)
                      else false endif;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Quadratic expressions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
%% Forward float_times as fzn_float_times
opt bool: QuadrFloatSolverConfig;             %% As set in share/minizinc/Preferences.json
opt bool: QuadrFloat;                         %% User option, e.g., with -D
bool: MZN__QuadrFloat = if occurs(QuadrFloat) then deopt(QuadrFloat)
                      elseif occurs(QuadrFloatSolverConfig) then deopt(QuadrFloatSolverConfig)
                      else false endif;

%% Forward int_times as fzn_int_times
opt bool: QuadrIntSolverConfig;               %% As set in share/minizinc/Preferences.json
opt bool: QuadrInt;                           %% User option, e.g., with -D
bool: QuadrIntFinal = if occurs(QuadrInt) then deopt(QuadrInt)
                      elseif occurs(QuadrIntSolverConfig) then deopt(QuadrIntSolverConfig)
                      else false endif;
opt int: QuadrIntCard;          %% Convert int_times to fzn_int_times if the minimum
                                %% of x, y's domain cardinalities as at least QuadrIntCard
int: MZN__QuadrIntCard = if occurs(QuadrIntCard) then deopt(QuadrIntCard)
                         elseif QuadrIntFinal then 0 else infinity endif;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Subtour elimination in circuit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
opt int: nSECcuts;       %% 0,1: use MTZ formulation
int: nMZN__fSECcuts =    %% 1,2: pass on circuit constraints to the MIP_solverinstance's cut gen
           if absent( nSECcuts ) then 0
           else deopt( nSECcuts ) endif;

    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MIPdomains %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
%% Paper: %   Belov, Stuckey, Tack, Wallace. Improved Linearization of Constraint Programming Models. CP 2016 Proceedings.
  %%% The below option enables translation of domain constraints into the ...POST predicates.
  %%% The code in MIPdomains.cpp processes them and also non-contiguous domains
  %%% (only-range-domains is then standardly off). MIPdomains.cpp needs all the required
  %%% __POST predicates to be declared to kick in.
opt bool: fMIPDomains;           %% unified decomposition constraints (...__POST) to FlatZinc
opt bool: fMIPdomains;           %% Can be defined from cmdline: -D "fMIPdomains=false"
bool: fPostprocessDomains =      %% True to pass all domain-related
           if absent( fMIPdomains ) /\ absent( fMIPDomains ) then true
           elseif not absent( fMIPdomains ) then deopt( fMIPdomains )
           else deopt( fMIPDomains )
           endif;
opt bool: fMIPdomAux;
bool: fPostproDom_AUX =          %% Specialized for aux_ constr
           if absent( fMIPdomAux ) then false
           else deopt( fMIPdomAux ) endif;
opt bool: fMIPdomDiff;
bool: fPostproDom_DIFF =         %% Specialized for differences: x<y <-> z=x-y<0
           if absent( fMIPdomDiff ) then false                             %% seems best for Gurobi, worse for CBC
           else deopt( fMIPdomDiff ) endif;

mzn_opt_only_range_domains = not fPostprocessDomains;   %% currently unused

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Avoid creating new int vars %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
opt bool: fAvoidNewInts;
bool: fAvoidNI =                  %% Actually this is only for ..._lin_..., not for just x-y
           if absent( fAvoidNewInts ) then false
           else deopt( fAvoidNewInts ) endif;
opt bool: fNewVarsInAuxEq;
bool: fAuxIntEqOLD00 = if absent(fNewVarsInAuxEq) then false else deopt(fNewVarsInAuxEq) endif;
bool: fAuxFloatEqOLD00 = if absent(fNewVarsInAuxEq) then false else deopt(fNewVarsInAuxEq) endif;

%%%%%%%%%%%%%%%%%%%%%  Redundant constraints ---------------------------------------------- %
bool: fMZN__IgnoreRedundantCumulative=false;
%% NOT WORKING NOW, use redefs_2.0.2.mzn: 
%%%%% bool: fMZN__IgnoreAllUserRedundant=false;   %% ignore all user-spec redundant constr

%%%%%%%%%%%%%%%%%%%%% Element, minimuum convex hull --------------------------------------- %
opt bool: fXBZCuts01;  %% orders 0, 1
opt bool: fXBZCutGen;  %% only works if Cuts01
bool: fElementCutsXZ=false;       %% Use simple XZ & XZB cuts for element
bool: fElementCutsXZB = if absent(fXBZCuts01) then false else deopt(fXBZCuts01) endif;
bool: fMinimumCutsXZ=false;       %% Use simple XZ & XZB cuts for minimum
bool: fMinimumCutsXZB = if absent(fXBZCuts01) then false else deopt(fXBZCuts01) endif;
bool: fUseXBZCutGen = if absent(fXBZCutGen) then false else deopt(fXBZCutGen) endif;

% ----------------------------------------------------------------------------------------- %
bool: fIntTimesBool=true;       %% Special handling of multiplication with a boolean(*const)

%-----------------------------------------------------------------------------%
%    If not postprocessing domains: For unary encoding: maximal domain length to invoke it

int: nMZN__UnarySizeMax_intTimes=20;
int: nMZN__UnarySizeMax_cumul=2000;
int: nMZN__UnarySizeMax_1step_regular=20000;   %% network-flow decomp in the regular constraint

int: nMZN__UnaryLenMin__ALL=1;    %% can be used by the indiv. cases
int: nMZN__UnaryLenMax__ALL=2000;    %% can be used by the indiv. cases
%  Some more detailed parameters
int: nMZN__UnaryLenMin_leq       = 1;
int: nMZN__UnaryLenMin_neq       = nMZN__UnaryLenMin__ALL;
int: nMZN__UnaryLenMin_eq        = nMZN__UnaryLenMin__ALL;
int: nMZN__UnaryLenMax_leq       = -1;
int: nMZN__UnaryLenMax_neq       = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_eq        = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_setIn     = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_setInReif = nMZN__UnaryLenMax__ALL;

%-----------------------------------------------------------------------------%
% Strict inequality
% The relative epsilon
%%% Has the problem that when relating to upper bound of various differences,
%%% getting different absolute eps...?
%% float: float_lt_EPS_coef__ = 1e-03; ABANDONED 12.4.18 due to #207
%%% Absolute one, used everywhere
%%% Might make no sense for floats with smaller domains etc.
opt float: float_EPS;
float: float_lt_EPS = if absent( float_EPS ) then 1e-6 else deopt( float_EPS ) endif;

%-----------------------------------------------------------------------------%
%%% Set =true to PRINT TRACING messages for some constraints:
opt bool: fMIPTrace;
bool: mzn__my_trace_on =
    if absent( fMIPTrace ) then false else deopt( fMIPTrace ) endif;
test my_trace(string: msg) ::promise_total
  = if mzn__my_trace_on then trace(msg)
    else true endif;
test my_trace(string: msg, bool: bb) ::promise_total
  = if mzn__my_trace_on then trace(msg, bb)
    else bb endif;
function var bool: my_trace(string: msg, var bool: bb) ::promise_total
  = if mzn__my_trace_on then trace(msg, bb)
    else bb endif;
%%% Set =true to PRINT TRACING messages for the currently debugged constraints:
opt bool: fMIPTraceDBG;
bool: mzn__my_trace__DBG_on =
    if absent( fMIPTraceDBG ) then false else deopt( fMIPTraceDBG ) endif;
test my_trace__DBG(string: msg) ::promise_total
  = if mzn__my_trace__DBG_on then trace(msg)
    else true endif;

