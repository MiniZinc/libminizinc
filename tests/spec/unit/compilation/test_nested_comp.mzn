/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    A: [[[0], [0,], [0]], [[3], [3], [3]], [[0], [0], [0]]]
    T: [[18], [30], [26], [25], [24], [29], [44], [21], [30], [39]]
***/

% Use this editor as a MiniZinc scratch book
% Nested comprehensions + nested let expressions stress test
% ----------------------------------------------------------
% This model constructs a 3D array using nested comprehensions,
% with inner arrays defined via let-in, and then aggregates them.

int: N = 3;
set of int: IDX = 1..N;

% Some base data to weave through nested lets/comprehensions.
array[IDX] of int: base = [2, 3, 5];

% A 2D array built by a comprehension that uses a nested let,
% builds an inner array via another comprehension, and folds it.
any: A ::output =
  array2d(IDX, IDX,
    [
      let {
        int: t = i * j,
        int: u = sum(k in 1..j) (k),                  % triangular-ish helper
        any: helper =                   % inner array via comp.
          [ (t + u + base[k],) | k in IDX where (i + k) mod 2 = 1 ]
      } in
        % Mix min/max over the inner array (force evaluation of 'helper')
        (max([h.1 | h in helper]) - min([h.1 | h in helper]),)
    | i in IDX, j in IDX
    ]
  );

% A 3D array that nests comprehensions deeper and uses multiple lets.
any: T ::output = (
  % array3d(IDX, IDX, IDX,
    [
      let {
        int: s = i + j + k,
        int: cross = i * j + j * k + k * i,
        any: inner1 =
          [ let {
              int: a = i * j,
              int: b = j * k
            } in (a + b + p,)
          | p in base
          ],
        any: inner2 =
          [ ((inner1[q].1 + s + base[q]) mod 7,)
          | q in index_set(inner1)      % refer to inner array's index set
          ]
      } in
        % Combine both inner arrays; also reference A to force 2D->3D interplay.
        (s + cross + sum(q in index_set(inner2))(inner2[q].1) + A[i, ((j + k - 1) mod N) + 1].1,)
    | i in IDX, j in IDX where i != j, k in IDX where (i + j + k) mod 2 = 0
    ]
  );
