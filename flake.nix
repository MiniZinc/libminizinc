{
  description = "Medium-level constraint modelling language";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages = {
          minizinc = pkgs.callPackage ./default.nix { };
          default = self.packages.${system}.minizinc;
        };

        devShells.default = pkgs.mkShell {
          name = "minizinc";

          packages = with pkgs; [
            bison
            cbc
            highs
            ccache
            cmake
            flex
            gecode
            mpfr
            ninja
            zlib
          ];
        };
      }
    );
}
