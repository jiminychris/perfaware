mkdir ~/build
pushd ~/build

CommonCompilerFlags=-std=c99

clang $CommonCompilerFlags ~/Projects/perfaware/part2/homework02/haversine_gen.c -o haversine_gen
clang $CommonCompilerFlags ~/Projects/perfaware/part2/homework02/haversine_calc.c -o haversine_calc

popd
