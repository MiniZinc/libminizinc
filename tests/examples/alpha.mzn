% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
%-----------------------------------------------------------------------------
% Alphabet puzzle, a well-known cryptoarithmetic puzzle
%
% Guido Tack, tack@gecode.org
% 2007-02-22
%
% Ported from the Gecode example
%
%-----------------------------------------------------------------------------

include "globals.mzn";

var 1..26: a;
var 1..26: b;
var 1..26: c;
var 1..26: d;
var 1..26: e;
var 1..26: f;
var 1..26: g;
var 1..26: h;
var 1..26: i;
var 1..26: j;
var 1..26: k;
var 1..26: l;
var 1..26: m;
var 1..26: n;
var 1..26: o;
var 1..26: p;
var 1..26: q;
var 1..26: r;
var 1..26: s;
var 1..26: t;
var 1..26: u;
var 1..26: v;
var 1..26: w;
var 1..26: x;
var 1..26: y;
var 1..26: z;

constraint
  b+a+l+l+e+t       = 45  /\
  c+e+l+l+o         = 43  /\
  c+o+n+c+e+r+t     = 74  /\
  f+l+u+t+e         = 30  /\
  f+u+g+u+e         = 50  /\
  g+l+e+e           = 66  /\
  j+a+z+z           = 58  /\
  l+y+r+e           = 47  /\
  o+b+o+e           = 53  /\
  o+p+e+r+a         = 65  /\
  p+o+l+k+a         = 59  /\
  q+u+a+r+t+e+t     = 50  /\
  s+a+x+o+p+h+o+n+e = 134 /\
  s+c+a+l+e         = 51  /\
  s+o+l+o           = 37  /\
  s+o+n+g           = 61  /\
  s+o+p+r+a+n+o     = 82  /\
  t+h+e+m+e         = 72  /\
  v+i+o+l+i+n       = 100 /\
  w+a+l+t+z         = 34;

constraint
  alldifferent([a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z]);
  
solve satisfy;

output [
  "a = ", show(a),
  "\tb = ", show(b),
  "\tc = ", show(c),
  "\td = ", show(d),
  "\te = ", show(e),
  "\tf = ", show(f),
  "\ng = ", show(g),
  "\th = ", show(h),
  "\ti = ", show(i),
  "\tj = ", show(j),
  "\tk = ", show(k),
  "\tl = ", show(l),
  "\nm = ", show(m),
  "\tn = ", show(n),
  "\to = ", show(o),
  "\tp = ", show(p),
  "\tq = ", show(q),
  "\tr = ", show(r),
  "\ns = ", show(s),
  "\tt = ", show(t),
  "\tu = ", show(u),
  "\tv = ", show(v),
  "\tw = ", show(w),
  "\tx = ", show(x),
  "\ny = ", show(y),
  "\tz = ", show(z),
  "\n"
];
