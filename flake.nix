{
  description = "App that lists all opened windows and switch focus to selected one";

  # Nixpkgs / NixOS version to use.
  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      # to work with older version of flakes
      lastModifiedDate = self.lastModifiedDate or self.lastModified or "19700101";

      # Generate a user-friendly version number.
      version = builtins.substring 0 8 lastModifiedDate;

      # System types to support.
      supportedSystems = [
        "x86_64-linux"
        "x86_64-darwin"
        "aarch64-linux"
        "aarch64-darwin"
      ];

      # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      # Nixpkgs instantiated for supported system types.
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      # Provide some binary packages for selected system types.
      packages = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};
          raylib = pkgs.fetchzip {
            url = "https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz";
            hash = "sha256-F3eQG+wLF9ZAO2T9oG+SDYOBRFuMhg+86fVQhVxNFJQ=";
          };
        in
        {
          default = pkgs.stdenv.mkDerivation {
            name = "focus-switcher";
            inherit version;
            src = ./src;
            buildInputs = with pkgs; [
              clang-tools
              alsa-lib
              mesa
              libGL
              bear
              xorg.libX11
              xorg.libXrandr
              xorg.libXi
              xorg.libXinerama
              xorg.libXcursor
            ];
            configurePhase = # bash
              ''
                export RAYLIB_INCLUDE_PATH="${raylib}/include"
                export RAYLIB_LIB_PATH="${raylib}/lib"
              '';
            installPhase = # bash
              ''
                mkdir -p $out/bin
                cp focus_switcher $out/bin/
              '';
          };
        }
      );

      # The default package for 'nix build'. This makes sense if the
      # flake provides only one package or there is a clear "main"
      # package.
      defaultPackage = forAllSystems (system: self.packages.${system}.default);

      devShell = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};
          raylib = pkgs.fetchzip {
            url = "https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz";
            hash = "sha256-F3eQG+wLF9ZAO2T9oG+SDYOBRFuMhg+86fVQhVxNFJQ=";
          };
        in
        pkgs.mkShell {
          buildInputs = with pkgs; [
            clang-tools
            wine
            pkgsCross.mingw32.buildPackages.gcc
            alsa-lib
            mesa
            libGL
            bear
            xorg.libX11
            xorg.libXrandr
            xorg.libXi
            xorg.libXinerama
            xorg.libXcursor
          ];
          shellHook = # bash
            ''
              export RAYLIB_INCLUDE_PATH="${raylib}/include"
              export RAYLIB_LIB_PATH="${raylib}/lib"
            '';
        }
      );

    };
}
