{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    systems.url = "github:nix-systems/default";
  };

  outputs = {
    systems,
    nixpkgs,
    ...
  }: let
    eachSystem = f:
      nixpkgs.lib.genAttrs (import systems) (
        system:
          f nixpkgs.legacyPackages.${system}
      );
  in {
    devShells = eachSystem (pkgs: {
      default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gnumake
          gdb
          bear
          clang-tools
          libllvm
          gf
        ];

        shellHook = ''
          export CC=${pkgs.clang}/bin/clang
          export ASAN_SYMBOLIZER_PATH=${pkgs.libllvm}/bin/llvm-symbolizer
        '';
      };
    });
  };
}
