include "ide/vis.mzn";

int: n_colors;

enum PALETTE = { red, lime, blue, cyan, magenta, yellow, orange };
set of PALETTE: COLOR = to_enum(PALETTE, 1..n_colors);

% Color for each region
var COLOR: wa;
var COLOR: nt;
var COLOR: sa;
var COLOR: q;
var COLOR: nsw;
var COLOR: v;
var COLOR: t;

% Neighboring regions have different colours
constraint wa != nt;
constraint wa != sa;
constraint nt != sa;
constraint nt != q;
constraint sa != q;
constraint sa != nsw;
constraint sa != v;
constraint q != nsw;
constraint nsw != v;

% Data to send during initialisation of the visualisation
any: initial_data = (n: n_colors);

% Data to send on each solution
% Note the use of :: output_only
any: solution_data :: output_only = (
  wa: show(wa), % Get colors as strings
  nt: show(nt),
  sa: show(sa),
  q: show(q),
  nsw: show(nsw),
  v: show(v),
  t: show(t)
);
% Launch the visualisation
output :: vis_server("vis_aust.html", initial_data) solution_data;
