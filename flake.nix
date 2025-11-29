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
          stm32cubemx
          cmake
          ninja
          gcc-arm-embedded
          openocd
          gdb
          stlink
        ];
      in {
        default = pkgs.mkShell {
          packages = deps;

          env = {
          };
        };
      });
    };
}
