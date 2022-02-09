include "ide/vis.mzn";

enum REGION = { WA, NT, SA, Q, NSW, V, T };
enum COLOR = Color(1..card(REGION));

% Neighboring regions
array [int, int] of REGION: neighbors = [| WA,  NT
                                         | WA,  SA
                                         | NT,  SA
                                         | NT,  Q
                                         | SA,  Q
                                         | SA,  NSW
                                         | SA,  V
                                         | Q,   NSW
                                         | NSW, V|];

% Color of each Region
array [REGION] of var COLOR: color;
% Number of colors used
var 1..card(COLOR): n_colors :: output = card({ c | c in color });

% Neighboring regions have different colours
constraint forall (i in index_set_1of2(neighbors))
                  (color[neighbors[i, 1]] != color[neighbors[i, 2]]);

% Use as few colors as possible
solve minimize n_colors;

% Launch the visualisation page and pass the along the regions and colors as strings
constraint ide_launch_server("vis_aust.html", json_object([|
  "regions", showJSON([show(r) | r in REGION]) |
  "colors", showJSON([show(c) | c in COLOR]) |
|]));

% Pass solution colors as an array of color strings to the visualisation page
output :: "vis_json" [showJSON([show(c) | c in color])];
