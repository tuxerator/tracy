{
  pkgs,
  qt6,
  ninja,
  cmake,
  stdenv,
}:
stdenv.mkDerivation rec {
  pname = "tracy";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    ninja
    cmake
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtwayland
    qt6.qt5compat
  ];

  checkInputs = [
  ];

  configurePhase = ''
    cmake . -G Ninja
  '';

  buildPhase = ''
    ninja
  '';

  checkPhase = ''
    ninja test
  '';

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    mv tracy $out/bin

    runHook postInstall
  '';

  meta = with pkgs.lib; {
    description = "A simple raytracer with QT";
    license = licenses.gpl2;
    platforms = platforms.unix;
  };
}
