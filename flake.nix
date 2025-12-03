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

      packages = forEachSupportedSystem ({ pkgs }: with pkgs; rec {
        library = stdenv.mkDerivation {
          pname = "hmr2300";
          version = "latest";

          src = lib.cleanSource ./.;

          nativeBuildInputs = [
            cmake
          ];

          cmakeBuildType = "Release";
        };
        hmrtool = stdenv.mkDerivation {
          pname = "hmrtool";
          version = "latest";

          src = lib.cleanSource ./hmrtool;

          nativeBuildInputs = [
            cmake
          ];

          buildInputs = [
            library
          ];

          cmakeBuildType = "Release";
        };
        stm32 = stdenv.mkDerivation {
          pname = "hmr2300-stm32";
          version = "latest";

          src = lib.cleanSource ./.;

          nativeBuildInputs = [
            cmake
          ];

          buildInputs = [
            gcc-arm-embedded
          ];

          cmakeFlags = [
            "-DTARGET_STM32=ON"
          ];
          cmakeBuildType = "Release";
        };
      });
    };
}
