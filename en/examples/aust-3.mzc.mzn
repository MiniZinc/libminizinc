int: wa;
int: nt;
int: sa;
int: q;
int: nsw;
int: v;
int: t;

test check(bool: b,string: s) =
  if b then true else trace_stdout("ERROR: "++s++"\n",false) endif;

array[int] of bool: checks = [
  check(wa!=nt, "wa and nt have the same colour"),
  check(wa!=sa, "wa and sa have the same colour"),
  check(nt!=sa, "nt and sa have the same colour"),
  check(nt!=q, "nt and q have the same colour"),
  check(sa!=q, "sa and q have the same colour"),
  check(sa!=nsw, "sa and nsw have the same colour"),
  check(sa!=v, "sa and v have the same colour"),
  check(q!=nsw, "q and nsw have the same colour"),
  check(nsw!=v, "nsw and v have the same colour")
];

output [
  if forall(checks)
  then "CORRECT: all constraints hold.\n"
  else "INCORRECT\n"
  endif
];
