include "globals.mzn";
include "ide/vis.mzn";

% Graph definition
enum NODE = {A, B, C, D, E, F, G, H, I, J};
array [int] of NODE: from = [A, A, A, B, B, B, B, C, C, D, D, E,  E, F, F, F, G,  G, H, H, I];
array [int] of NODE:   to = [B, C, E, C, D, E, G, D, F, F, G, G,  J, G, H, I, I,  J, I, J, J];
array [int] of int:     w = [3, 6, 9, 4, 2, 9, 9, 2, 9, 9, 8, 8, 18, 7, 4, 5, 9, 10, 1, 4, 3];

% Find the minimum spanning tree of the graph
var 0..sum(w): total_weight;
array [index_set(from)] of var bool: es;
constraint weighted_spanning_tree(
  card(NODE),   % Number of nodes
  length(from), % Number of edges
  from,         % Edge from node
  to,           % Edge to node
  w,            % Weight of edge
  es,           % Whether edge is in spanningtree
  total_weight  % Total weight of spanning tree
);
solve minimize total_weight;

% Graph visualisation
array [int] of string: edge_labels = [i: show(w[i]) | i in index_set(w)];
output vis_graph_highlight(
  from,                  % Edge from node
  to,                    % Edge to node
  edge_labels,           % Edges are labelled with their weights
  [i: true | i in NODE], % All nodes are present in a spanning tree
  es                     % Whether edge is in spanningtree
);

% Objective visualisation
output vis_line(total_weight, "Spanning tree weight");
