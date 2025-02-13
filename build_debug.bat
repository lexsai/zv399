@echo off

pushd build\\Debug
  del *.pdb
popd

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
