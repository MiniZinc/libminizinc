

% Regression test for bug #258: this model caused version 1.3.2 of the MIP
% backend (with Cbc as the solver) to seg fault.
%
% We don't run this as part of the standard test suite since it takes
% (a) quite a while to flatten and (b) quite a while to evaluate.

%---------------------------------------------------------------------------%

% Copyright (c) 2011 HighTechKids.  All rights reserved
% HighTechKids is on the web at: http://www.hightechkids.org
% This code is released under GPL; see LICENSE.txt for details.

include "globals.mzn";

% parameters
int: TInc;
int: TMax_hours;

% subjective setup
int: NSubjective;

% performance setup
int: NRounds;
int: NTables;

% judging groups
int: NGroups;

int: alpha_perf_minutes;

int: ct_minutes;
int: pct_minutes;



%%%% Don't edit below here %%%%
int: M = 10000;

% computed parameters

array [1..NSubjective] of int : subj_minutes;
array [1..NGroups] of int : group_counts;

array [1..NSubjective] of int : alpha_subj = [ x div TInc | x in subj_minutes ];

int: NTeams = sum(group_counts);
constraint assert(NTeams mod 2 == 0, "Number of teams must be even");

int: TMax = TMax_hours * 60 div TInc;
int: alpha_perf = alpha_perf_minutes div TInc;
int: ct = ct_minutes div TInc;
int: pct = pct_minutes div TInc;


int: MaxTeamsInGroup = max(group_counts);


% variables
array [1..NGroups, 1..MaxTeamsInGroup, 1..NSubjective, 1..TMax ] of var
0..1 : sz ::is_output;
array [1..NGroups, 1..MaxTeamsInGroup, 1..NSubjective, 1..TMax ] of var
0..1 : sy ::is_output;
array [1..NGroups, 1..MaxTeamsInGroup, 1..NTables, 1..2, 1..TMax ] of var
0..1 : pz ::is_output;
array [1..NGroups, 1..MaxTeamsInGroup, 1..NTables, 1..2, 1..TMax ] of var
0..1 : py ::is_output;
var int: objective;

% stationBusySubjective
constraint
  forall(g in 1..NGroups) (
  forall (i in 1..group_counts[g]) (
    forall (n in 1..NSubjective) (
      forall (t in alpha_subj[n]..TMax) (
        sum (u in 1..alpha_subj[n]) (sz[g,i,n,t-u+1]) <= sy[g,i,n,t]
))));

% stationBusyPerformance
constraint
  forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(b in 1..NTables) (
      forall(s in 1..2) (
        forall(t in alpha_perf..TMax) (
	  sum(u in 1..alpha_perf) (pz[g,i,b,s,t-u+1]) <= py[g,i,b,s,t]
)))));

% stationStartSubjective
constraint
  forall(g in 1..NGroups) (
  forall (i in 1..group_counts[g]) (
    forall (n in 1..NSubjective) (
      forall (t in 2..TMax) (
        sy[g,i,n,t] - sy[g,i,n,t-1] <= sz[g,i,n,t]
))));

% stationStartPerformance
constraint
  forall(g in 1..NGroups) (
  forall (i in 1..group_counts[g]) (
    forall(b in 1..NTables) (
      forall(s in 1..2) (
        forall(t in 2..TMax) (
	  py[g,i,b,s,t] - py[g,i,b,s,t-1] <= pz[g,i,b,s,t]
)))));

% noOverlapSubjective
constraint
  forall(g in 1..NGroups) (
  forall (t in 1..TMax) (
    forall (n in 1..NSubjective) (
      sum(i in 1..group_counts[g]) (sy[g,i,n,t]) <= 1
)));

% noOverlapPerformance
constraint
  forall (t in 1..TMax) (
    forall (b in 1..NTables) (
      forall (s in 1..2) (
        sum(g in 1..NGroups) (sum(i in 1..group_counts[g]) (py[g,i,b,s,t])) <= 1
)));

% teamSubjective
constraint
forall(g in 1..NGroups) (
  forall (i in 1..group_counts[g]) (
    forall (n in 1..NSubjective) (
      sum (t in 1..TMax) (sz[g,i,n,t]) = 1
)));

% teamPerformance
constraint
forall(g in 1..NGroups) (
  forall (i in 1..group_counts[g]) (
    sum (b in 1..NTables) (
      sum (t in 1..TMax) (pz[g,i,b,1,t] + pz[g,i,b,2,t])
    ) = 1
));

% subjectiveEOS
constraint
sum(g in 1..NGroups) (
  sum (i in 1..group_counts[g]) (
    sum (n in 1..NSubjective) (
      sum (t in TMax-alpha_subj[n]+1..TMax) (
        sz[g,i,n,t]
)))) = 0;

% performanceEOS
constraint
sum(g in 1..NGroups) (
  sum (i in 1..group_counts[g]) (
    sum (b in 1..NTables) (
      sum (t in TMax-alpha_perf+1..TMax) (
       pz[g,i,b,1,t] + pz[g,i,b,2,t]
)))) = 0;

int: subj_ulimit = ct-1;
int: subj_tlimit = TMax-subj_ulimit;

% subjSubjChangetime
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(n in 1..NSubjective) (
      forall(t in 1..subj_tlimit) (
        forall(d in 1..NSubjective) (
	  forall(u in 0..subj_ulimit) (
	    if(d != n) then
	      sy[g,i,n,t] + sy[g,i,d,t+u] <= 1
	    else
	      true
	    endif
))))));

% subjPerfChangetime
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(n in 1..NSubjective) (
      forall(t in 1..subj_tlimit) (
        forall(u in 0..subj_ulimit) (
          forall(b in 1..NTables) (
	    forall(s in 1..2) (
	      sy[g,i,n,t] + py[g,i,b,s,t+u] <= 1
)))))));

int: perf_ulimit = ct-1;
int: perf_tlimit = TMax-perf_ulimit;

% perfPerfChangetime
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(b in 1..NTables) (
      forall(s in 1..2) (
        forall(t in 1..perf_tlimit) (
          forall(u in 0..perf_ulimit) (
  	    forall(d in 1..NTables) (
  	      forall(e in 1..2) (
	        if(b != d /\ s != e) then
		  py[g,i,b,s,t] + py[g,i,d,e,t+u] <= 1
		else
		  true
		endif
))))))));

% perfSubjChangetime
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(b in 1..NTables) (
      forall(s in 1..2) (
        forall(t in 1..perf_tlimit) (
	  forall(u in 0..perf_ulimit) (
	    forall(n in 1..NSubjective) (
	      py[g,i,b,s,t] + sy[g,i, n, t+u] <= 1
)))))));

int: perf_pct_ulimit = pct-1;
int: perf_pct_tlimit = TMax-perf_pct_ulimit;

% performanceChangetime
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    forall(t in 1..perf_pct_tlimit) (
      forall(u in 1..perf_pct_ulimit) (
        sum(b in 1..NTables) (
	  py[g,i,b,1,t] + py[g,i,b,2,t] + py[g,i,b,1,t+u] + py[g,i,b,2,t+u]
	) <= 1
))));

% perfUseBothSides
constraint
  forall(b in 1..NTables) (
    forall(t in 1..TMax) (
      sum(g in 1..NGroups) (sum(i in 1..group_counts[g]) (py[g,i,b,1,t]))
      = sum(g in 1..NGroups) (sum(i in 1..group_counts[g]) (py[g,i,b,2,t]))
));

% teamJudging
constraint
forall(g in 1..NGroups) (
  forall(i in 1..group_counts[g]) (
    sum(t in 1..TMax) (
      sum(n in 1..NSubjective) (
        sz[g,i,n,t] +
	sum(b in 1..NTables) (
	  pz[g,i,b,1,t] + pz[g,i,b,2,t]
	))) = NSubjective + NRounds
));


% performanceStart
int: perfStartWait = 60 div TInc;
constraint
sum(g in 1..NGroups) (
  sum(i in 1..group_counts[g]) (
    sum(b in 1..NTables) (
      sum(t in 1..perfStartWait) (
        pz[g,i,b,1,t] + pz[g,i,b,2,t]
)))) = 0;

% objective
constraint
sum(t in 1..TMax) (
  sum(g in 1..NGroups) (
    sum(i in 1..group_counts[g]) (
      sum(n in 1..NSubjective) (
        sy[g,i,n,t] * t
      ) +
      sum(b in 1..NTables) (
        (py[g,i,b,1,t] + py[g,i,b,2,t]) * t
      )
))) = objective;


solve minimize objective;
    
% output
% [ if fix(sz[g,i,n,t]) == 1 then
%     "sz[" ++ show(g) ++ "][" ++ show(i) ++ "][" ++ show(n) ++ "][" ++ show(t) ++ "] = " ++
%       show(sz[g, i, n, t]) ++ "\n"
%     else
%       ""
%     endif
%        | g in 1..NGroups, i in 1..group_counts[g], n in 1..NSubjective, t in 1..TMax
%        ]
% ++ [
%  if fix(pz[g,i,b,s,t]) == 1 then
%     "pz[" ++ show(g) ++ "][" ++ show(i) ++ "][" ++ show(b) ++ "][" ++ show(s) ++ "][" ++ show(t) ++ "] = "
%      ++ show(pz[g, i, b, s, t]) ++ "\n"
%     else
%       ""
%     endif
%        | g in 1..NGroups, i in 1..group_counts[g], b in 1..NTables, s in 1..2, t in 1..TMax
%        ] 
% %correct ++ [ "perf_pct_tlimit " ++ show(perf_pct_tlimit) ] ++ ["\n"]
% %correct ++ [ "perf_pct_ulimit " ++ show(perf_pct_ulimit) ] ++ ["\n"]
% %correct ++ [ show(alpha_perf) ] ++ ["\n"]
% %correct ++ [ show(pct) ] ++ ["\n"]
% ++ ["\n"]
% ;
