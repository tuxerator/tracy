{
  description = "A Nix-flake-based C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    make-shell.url = "github:nicknovitski/make-shell";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs =
    inputs@{
      flake-parts,
      ...
    }:
    # https://flake.parts/module-arguments.html
    flake-parts.lib.mkFlake { inherit inputs; } (
      top@{
        config,
        withSystem,
        moduleWithSystem,
        ...
      }:
      {
        imports = [
          # Optional: use external flake logic, e.g.
          # inputs.foo.flakeModules.default
          inputs.make-shell.flakeModules.default
        ];
        flake = {
          # Put your original flake attributes here.
        };
        systems = [
          # systems for which you want to build the `perSystem` attributes
          "x86_64-linux"
          "aarch64-linux"
          "aarch64-darwin"
          "x86_64-darwin"
        ];
        perSystem =
          { self', config, pkgs, ... }:
          {
            # Recommended: move all package definitions here.
            # e.g. (assuming you have a nixpkgs input)
            # packages.foo = pkgs.callPackage ./foo/package.nix { };
            # packages.bar = pkgs.callPackage ./bar/package.nix {
            #   foo = config.packages.foo;
            # };

            packages = rec {
              stdenv = stdenv;
              tracy = pkgs.qt6Packages.callPackage ./default.nix { inherit pkgs; };
              default = tracy;
            };

            make-shells.default = {
              packages = [
                
              ];
              inputsFrom = [
                self'.packages.tracy
              ];
              stdenv = pkgs.clangStdenv;  
            };
          };
      }
    );
}
