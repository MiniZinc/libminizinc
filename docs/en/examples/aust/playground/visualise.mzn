include "ide/vis.mzn";

constraint ide_launch_server("australia.html");

output :: "vis_json" [showJSON((
  wa: wa,
  nt: nt,
  sa: sa,
  q: q,
  nsw: nsw,
  v: v,
  t: t
))];