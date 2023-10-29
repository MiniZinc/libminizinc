include "ide/vis.mzn";

constraint ide_launch_server("australia.html");

output :: "vis_json" [showJSON((
  wa: show(wa),
  nt: show(nt),
  sa: show(sa),
  q: show(q),
  nsw: show(nsw),
  v: show(v),
  t: show(t)
))];