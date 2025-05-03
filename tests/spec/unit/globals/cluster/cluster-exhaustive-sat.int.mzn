/***
--- !Test
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    GraphEdges:
    - false
    - true
    - true
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - true
      - true
    - - true
      - true
      - true
    - - true
      - true
      - true
  - !Solution
    GraphEdges:
    - true
    - true
    - true
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - true
      - true
    - - true
      - true
      - true
    - - true
      - true
      - true
  - !Solution
    GraphEdges:
    - true
    - false
    - true
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - true
      - true
    - - true
      - true
      - true
    - - true
      - true
      - true
  - !Solution
    GraphEdges:
    - true
    - true
    - false
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - true
      - true
    - - true
      - true
      - true
    - - true
      - true
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - true
    GraphNodes:
    - false
    - true
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - true
    - - false
      - true
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - true
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - true
    - - false
      - true
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - false
    - false
    - false
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - false
    - false
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - false
    - true
    - false
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - false
    - true
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - true
    - false
    - false
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - true
    - true
    - false
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - true
    - false
    - false
    GraphNodes:
    - true
    - true
    - false
    ReachabilityMatrix:
    - - true
      - true
      - false
    - - true
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - true
    - false
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - false
    - false
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - false
      - false
    - - false
      - true
      - false
    - - false
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - true
    - false
    GraphNodes:
    - true
    - false
    - true
    ReachabilityMatrix:
    - - true
      - false
      - true
    - - false
      - true
      - false
    - - true
      - false
      - true
  - !Solution
    GraphEdges:
    - false
    - true
    - false
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - false
      - true
    - - false
      - true
      - false
    - - true
      - false
      - true
  - !Solution
    GraphEdges:
    - true
    - false
    - false
    GraphNodes:
    - true
    - true
    - true
    ReachabilityMatrix:
    - - true
      - true
      - false
    - - true
      - true
      - false
    - - false
      - false
      - true
***/

include "cluster.mzn";

include "reachable.mzn";
include "subgraph.mzn";

%-------------------------------------------------------------------------------

array[GRAPH_NODES] of var bool: GraphNodes; % free variable
array[GRAPH_EDGES] of var bool: GraphEdges; % free variable
array[GRAPH_NODES,GRAPH_NODES] of var bool: ReachabilityMatrix;

%===============================================================================

int: NUM_GRAPH_NODES = 3; % par
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

constraint cluster(NUM_GRAPH_NODES, NUM_GRAPH_EDGES, GRAPH_LEAVING_NODE, GRAPH_ENTERING_NODE, ReachabilityMatrix, GraphNodes, GraphEdges);

% List all computed matrices.
