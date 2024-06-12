{ lib, stdenv, cbc, cmake, gecode, highs, mpfr, ninja, zlib }:

stdenv.mkDerivation {
  name = "minizinc";
  src = ./.;

  nativeBuildInputs = [ cmake ninja ];
  buildInputs = [ gecode mpfr zlib cbc highs ];

  meta = with lib; {
    homepage = "https://www.minizinc.org/";
    description = "A medium-level constraint modelling language";
    longDescription = ''
      MiniZinc is a medium-level constraint modelling
      language. It is high-level enough to express most
      constraint problems easily, but low-level enough
      that it can be mapped onto existing solvers easily and consistently.
    '';
    license = licenses.mpl20;
    platforms = platforms.unix;
  };
}
