/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    xi: [1, 2, 1, 2]
    yi: 1
    xoi: [1, null, 1, 2]
    yoi: 1
    xb: [true, false, true, false]
    yb: false
    xf: [1.5, 2.0, 1.5, 2.0]
    yf: 1.5
    xs: [!!set {1,2}, !!set {2}, !!set {1,2}, !!set {1}]
    ys: !!set {1,2}
    eq_vi: 2
    eq_vb: 2
    eq_vf: 2
    eq_vs: 2
    eq_vo: 2
    geq_vi: 3
    geq_vb: 3
    geq_vf: 3
    geq_vs: 3
    geq_vo: 3
    gt_vi: 3
    gt_vb: 3
    gt_vf: 3
    gt_vs: 3
    gt_vo: 3
    leq_vi: 1
    leq_vb: 1
    leq_vf: 1
    leq_vs: 1
    leq_vo: 1
    lt_vi: 1
    lt_vb: 1
    lt_vf: 1
    lt_vs: 1
    lt_vo: 1
    neq_vi: 1
    neq_vb: 1
    neq_vf: 1
    neq_vs: 1
    neq_vo: 1
***/

array[1..4] of var 1..3: xi = [1, 2, 1, _];
var 1..3: yi;

array[1..4] of var opt 1..2: xoi = [1, <>, 1, _];
var 1..3: yoi;

array[1..4] of var bool: xb = [true, false, true, _];
var bool: yb;

array[1..4] of var {1.5, 2.0, 3.0}: xf = [1.5, 2.0, 1.5, _];
var {1.0, 1.5, 2.0, 3.0}: yf;

array[1..4] of var set of 1..3: xs = [{1, 2}, {2}, {1, 2}, _];
var set of 1..3: ys;

var 1..3: eq_vi;
var 1..3: eq_vb;
var 1..3: eq_vf;
var 1..3: eq_vs;
var 1..3: eq_vo;

var 2..4: geq_vi;
var 2..4: geq_vb;
var 2..4: geq_vf;
var 2..4: geq_vs;
var 2..4: geq_vo;

var 2..4: gt_vi;
var 2..4: gt_vb;
var 2..4: gt_vf;
var 2..4: gt_vs;
var 2..4: gt_vo;

var 0..2: leq_vi;
var 0..2: leq_vb;
var 0..2: leq_vf;
var 0..2: leq_vs;
var 0..2: leq_vo;

var 0..2: lt_vi;
var 0..2: lt_vb;
var 0..2: lt_vf;
var 0..2: lt_vs;
var 0..2: lt_vo;

var 0..2: neq_vi;
var 0..2: neq_vb;
var 0..2: neq_vf;
var 0..2: neq_vs;
var 0..2: neq_vo;

% count_eq overloads
constraint count_eq(xi, yi, eq_vi);
constraint count_eq(xb, yb, eq_vb);
constraint count_eq(xf, yf, eq_vf);
constraint count_eq(xs, ys, eq_vs);
constraint count_eq(xoi, yoi, eq_vo);
constraint count_eq(xi, 1, 2);
constraint count_eq(xb, true, 2);
constraint count_eq(xf, 1.5, 2);
constraint count_eq(xs, {1, 2}, 2);

% count_geq overloads
constraint count_geq(xi, yi, geq_vi);
constraint count_geq(xb, yb, geq_vb);
constraint count_geq(xf, yf, geq_vf);
constraint count_geq(xs, ys, geq_vs);
constraint count_geq(xoi, yoi, geq_vo);
constraint count_geq(xi, 1, 3);
constraint count_geq(xb, true, 3);
constraint count_geq(xf, 1.5, 3);
constraint count_geq(xs, {1, 2}, 3);

% count_gt overloads
constraint count_gt(xi, yi, gt_vi);
constraint count_gt(xb, yb, gt_vb);
constraint count_gt(xf, yf, gt_vf);
constraint count_gt(xs, ys, gt_vs);
constraint count_gt(xoi, yoi, gt_vo);
constraint count_gt(xi, 1, 3);
constraint count_gt(xb, true, 3);
constraint count_gt(xf, 1.5, 3);
constraint count_gt(xs, {1, 2}, 3);

% count_leq overloads
constraint count_leq(xi, yi, leq_vi);
constraint count_leq(xb, yb, leq_vb);
constraint count_leq(xf, yf, leq_vf);
constraint count_leq(xs, ys, leq_vs);
constraint count_leq(xoi, yoi, leq_vo);
constraint count_leq(xi, 1, 1);
constraint count_leq(xb, true, 1);
constraint count_leq(xf, 1.5, 1);
constraint count_leq(xs, {1, 2}, 1);

% count_lt overloads
constraint count_lt(xi, yi, lt_vi);
constraint count_lt(xb, yb, lt_vb);
constraint count_lt(xf, yf, lt_vf);
constraint count_lt(xs, ys, lt_vs);
constraint count_lt(xoi, yoi, lt_vo);
constraint count_lt(xi, 1, 1);
constraint count_lt(xb, true, 1);
constraint count_lt(xf, 1.5, 1);
constraint count_lt(xs, {1, 2}, 1);

% count_neq overloads
constraint count_neq(xi, yi, neq_vi);
constraint count_neq(xb, yb, neq_vb);
constraint count_neq(xf, yf, neq_vf);
constraint count_neq(xs, ys, neq_vs);
constraint count_neq(xoi, yoi, neq_vo);
constraint count_neq(xi, 1, 1);
constraint count_neq(xb, true, 1);
constraint count_neq(xf, 1.5, 1);
constraint count_neq(xs, {1, 2}, 1);

any: obj :: output = sum([
  xi[4],
  yi,
  xoi[4],
  yoi,
  xb[4],
  yb,
  xf[4],
  yf,
  max(xs[4]),
  max(ys),
  abs(eq_vi - 2),
  abs(eq_vb - 2),
  abs(eq_vf - 2),
  abs(eq_vs - 2),
  abs(eq_vo - 2),
  abs(geq_vi - 3),
  abs(geq_vb - 3),
  abs(geq_vf - 3),
  abs(geq_vs - 3),
  abs(geq_vo - 3),
  abs(gt_vi - 3),
  abs(gt_vb - 3),
  abs(gt_vf - 3),
  abs(gt_vs - 3),
  abs(gt_vo - 3),
  abs(leq_vi - 1),
  abs(leq_vb - 1),
  abs(leq_vf - 1),
  abs(leq_vs - 1),
  abs(leq_vo - 1),
  abs(lt_vi - 1),
  abs(lt_vb - 1),
  abs(lt_vf - 1),
  abs(lt_vs - 1),
  abs(lt_vo - 1),
  abs(neq_vi - 1),
  abs(neq_vb - 1),
  abs(neq_vf - 1),
  abs(neq_vs - 1),
  abs(neq_vo - 1),
]);

constraint obj = 12.5;
