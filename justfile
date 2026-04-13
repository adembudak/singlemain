buildDir := 'build'

alias c := configure
configure buildDir = buildDir:
  cmake -DVCPKG_MANIFEST_MODE=ON -S . -B {{buildDir}} --toolchain $VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

alias b := build
build buildDir = buildDir:
  cmake --build {{buildDir}} --clean-first

alias i := install
install buildDir = buildDir:
  cmake --install {{buildDir}}

run buildDir = buildDir:
  cmake --build {{buildDir}}
  ./{{buildDir}}/pot
