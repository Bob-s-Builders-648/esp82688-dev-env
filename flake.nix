{
  description = "ESP-32 Firmware";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";

    process-compose-flake.url = "github:Platonic-Systems/process-compose-flake";
  };

  outputs = {flake-parts, ...} @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [inputs.process-compose-flake.flakeModule];

      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];

      perSystem = {pkgs, ...}: {

        devShells = {
          default = pkgs.mkShell {
            name = "dev-shell";
            buildInputs = with pkgs; [
              python312
              python312Packages.pip
              esptool
            ];

            shellHook = ''
            '';
          };
        };

        process-compose = {
          localpi = {
            
          };
        };
      };
    };
}
