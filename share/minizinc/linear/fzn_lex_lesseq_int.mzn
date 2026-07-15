%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

opt bool: UseCPLexLesseq; %% When not UseOrbisack, use CP decomposition
opt bool: OrbisackAlwaysModelConstraint; %% Use with SCIP 7.0.2

predicate fzn_lex_lesseq_int(
  array [int] of var int: xs :: promise_ctx_antitone,
  array [int] of var int: ys :: promise_ctx_monotone,
) =
  assert(
    length(xs) == length(ys), %% SCIP cannot
    "lex_lesseq_int(\(xs), \(ys)): arrays of different lengths",
  ) /\
    if MZN__Orbisack /\ dom_array(xs) subset 0..1 /\ dom_array(ys) subset 0..1 then
      fzn_lex_lesseq__orbisack(
        xs,
        ys,
        (occurs(OrbisackAlwaysModelConstraint) /\ deopt(OrbisackAlwaysModelConstraint)) \/
          not mzn_in_symmetry_breaking_constraint(),
      )
    elseif occurs(UseCPLexLesseq) /\ deopt(UseCPLexLesseq) then
      lex_lesseq_std_decomposition(xs, ys)
    else
      fzn_lex_lesseq_int__MIP(xs, ys)
    endif;

predicate fzn_lex_lesseq_int__MIP(
  array [int] of var int: x :: promise_ctx_antitone,
  array [int] of var int: y :: promise_ctx_monotone,
) =
  if length(x) = 0 then
    true
  elseif length(y) = 0 then
    length(x) = 0
  else
    let {
      int: lx = min(index_set(x));
      int: ux = max(index_set(x));
      int: ly = min(index_set(y));
      int: uy = max(index_set(y));
      int: size = min(ux - lx, uy - ly);
      array [0..size] of var 0..1: fEQ;
      array [0..size] of var 0..1: fLT;
    } in sum(fLT) <= 1 /\
      fEQ[0] + fLT[0] == 1 /\
      forall (i in 1..size) (fEQ[i - 1] == fEQ[i] + fLT[i]) /\
      forall (i in 0..size) (
        aux_int_eq_if_1(x[lx + i], y[ly + i], fEQ[i]) /\
          aux_int_lt_if_1(x[lx + i], y[ly + i], fLT[i])
      )
  endif;

%% SCIP constraint handler orbisack
predicate fzn_lex_lesseq__orbisack(
  array [int] of var int: vec1,
  array [int] of var int: vec2,
  bool: isModelCons,
);

%-----------------------------------------------------------------------------%
