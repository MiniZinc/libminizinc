include "ide/vis.mzn";

output :: vis_server("australia.html") (
  wa: show(wa),
  nt: show(nt),
  sa: show(sa),
  q: show(q),
  nsw: show(nsw),
  v: show(v),
  t: show(t)
);
