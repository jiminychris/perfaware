mkdir ~/build
pushd ~/build

CommonCompilerFlags=-std=c99

clang $CommonCompilerFlags ~/Projects/perfaware/part2/homework01/haversine_gen.c -o haversine_gen

popd
