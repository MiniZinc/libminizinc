/***
--- !Test
expected: !Result
  status: UNSATISFIABLE
***/

include "cluster.mzn";

include "reachable.mzn";
include "subgraph.mzn";

include "cluster.naive.mzn.model";

%-------------------------------------------------------------------------------

array[GRAPH_NODES] of var bool: GraphNodes; % free variable
array[GRAPH_EDGES] of var bool: GraphEdges; % free variable
array[GRAPH_NODES,GRAPH_NODES] of var bool: ReachabilityMatrixNaive; % free variable
array[GRAPH_NODES,GRAPH_NODES] of var bool: ReachabilityMatrix; % free variable

%===============================================================================

int: NUM_GRAPH_NODES = 5; % par
int: NUM_GRAPH_EDGES = length(j,i in GRAPH_NODES where i > j)(1);
array[GRAPH_EDGES] of tuple(GRAPH_NODES,GRAPH_NODES): GRAPH_NODE_PAIRS = [ (j,i) | j,i in GRAPH_NODES where i > j ];

%-------------------------------------------------------------------------------

set of int: GRAPH_NODES = 1..NUM_GRAPH_NODES;
set of int: GRAPH_EDGES = 1..NUM_GRAPH_EDGES;

array[GRAPH_EDGES] of GRAPH_NODES: GRAPH_LEAVING_NODE  = [ p.1 | p in GRAPH_NODE_PAIRS ];
array[GRAPH_EDGES] of GRAPH_NODES: GRAPH_ENTERING_NODE = [ p.2 | p in GRAPH_NODE_PAIRS ];

constraint subgraph(
  GRAPH_LEAVING_NODE,
  GRAPH_ENTERING_NODE,
  GraphNodes,
  GraphEdges
);

%-------------------------------------------------------------------------------

constraint cluster_naive(GRAPH_LEAVING_NODE, GRAPH_ENTERING_NODE, ReachabilityMatrixNaive, GraphNodes, GraphEdges);
constraint cluster      (NUM_GRAPH_NODES, NUM_GRAPH_EDGES, GRAPH_LEAVING_NODE, GRAPH_ENTERING_NODE, ReachabilityMatrix,      GraphNodes, GraphEdges);

% Look for solutions where matrices *DO NOT* match.
% There should *NOT* be any such solutions.
constraint not(forall (j,i in GRAPH_NODES)(
  ReachabilityMatrix[j,i] == ReachabilityMatrixNaive[j,i]
));
