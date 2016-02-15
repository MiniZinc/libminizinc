/*
% Controls
%
*/

%-----------------------------------------------------------------------------%
ann: user_cut;
ann: lazy_constraint;
%%% comment away the assignments to have those as normal constraints  
ann: MIP_cut = user_cut;     %% seems better on average but wrong LB e.g. on carpet-cutting
ann: MIP_lazy; % = lazy_constraint;

%-----------------------------------------------------------------------------%
bool: fMZN__UseIndicators=false;    %% Pass on indicator constraints, CPLEX only.  [DISABLED]
    %% Reifs give wrong result on 2012/amaze, so using implications only

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MIPdomains %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
  %%% The below option enables translation of domain constraints into the ...POST predicates.
  %%% The code in MIPdomains.cpp processes them and also non-contiguous domains
  %%% (only-range-domains is then standardly off). MIPdomains.cpp needs all the required
  %%% __POST predicates to be declared to kick in.
opt bool: fMIPdomains;           %% Can be defined from cmdline: -D "fMIPdomains=false"
bool: fPostprocessDomains =      %% True to pass all domain-related
           if absent( fMIPdomains ) then true                         %% constraints (...__POST) to FlatZinc
           else deopt( fMIPdomains ) endif;
opt bool: fMIPdomAux;
bool: fPostproDom_AUX =          %% Specialized for aux_ constr
           if absent( fMIPdomAux ) then false
           else deopt( fMIPdomAux ) endif;
opt bool: fMIPdomDiff;
bool: fPostproDom_DIFF =         %% Specialized for differences: x<y <-> z=x-y<0
           if absent( fMIPdomDiff ) then true
           else deopt( fMIPdomDiff ) endif;

mzn_opt_only_range_domains = not fPostprocessDomains;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Avoid creating new int vars %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --------------------------------------------------------------------------------------- %
opt bool: fAvoidNewInts;
bool: fAvoidNI =                  %% Actually this is only for ..._lin_..., not for just x-y
           if absent( fAvoidNewInts ) then false
           else deopt( fAvoidNewInts ) endif;

%%%%%%%%%%%%%%%%%%%%%  Redundant constraints ---------------------------------------------- %
bool: fMZN__IgnoreRedundantCumulative=false;
%% NOT WORKING NOW, use redefs_2.0.2.mzn: 
%%%%% bool: fMZN__IgnoreAllUserRedundant=false;   %% ignore all user-spec redundant constr

%%%%%%%%%%%%%%%%%%%%% Element, minimuum convex hull --------------------------------------- %
bool: fElementCutsXZ=false;       %% Use simple XZ & XZB cuts for element
bool: fElementCutsXZB=false;       %% Use simple XZ & XZB cuts for element
bool: fMinimumCutsXZ=false;       %% Use simple XZ & XZB cuts for minimum
bool: fMinimumCutsXZB=false;       %% Use simple XZ & XZB cuts for minimum
bool: fUseXBZCutGen=false;      %% Use XZB cut generator for element & minimum

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
float: float_lt_EPS_coef__ = 1e-03;

%-----------------------------------------------------------------------------%
%%% Set =true to PRINT TRACING messages for some constraints:
opt bool: mzn__my_trace_on
    = false;
   %= true;
test my_trace(string: msg) ::promise_total
  = if occurs(mzn__my_trace_on) /\ deopt(mzn__my_trace_on) then
      trace(msg)
    else true endif;
%%% Set =true to PRINT TRACING messages for the currently debugged constraints:
opt bool: mzn__my_trace__DBG_on
   = false;
   %= true;
test my_trace__DBG(string: msg) ::promise_total
  = if occurs(mzn__my_trace__DBG_on) /\ deopt(mzn__my_trace__DBG_on) then
      trace(msg)
    else true endif;

