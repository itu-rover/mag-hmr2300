{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      lib = nixpkgs.lib;
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" ];
      forEachSupportedSystem = f: lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs {
          config.allowUnfree = true;
          inherit system;
        };
      });
    in
    {
      devShells = forEachSupportedSystem ({ pkgs }: let
        deps = with pkgs; [
          cmake
          ninja
          gcc-arm-embedded
          openocd
          gdb
          git
        ] ++ lib.optionals pkgs.stdenv.hostPlatform.isLinux [
          stm32cubemx
          stlink
        ];
      in {
        default = pkgs.mkShell {
          packages = deps;

          env = {
          };
        };
      });

      packages = forEachSupportedSystem ({ pkgs }: with pkgs; {
        default = stdenv.mkDerivation {
          pname = "mag-hmr2300";
          version = "latest";

          src = lib.cleanSource ./.;

          nativeBuildInputs = [
            cmake
            ninja
          ];

          buildInputs = [
            gcc-arm-embedded
          ];

          cmakeBuildType = "Release";
        };
      });
    };
}
