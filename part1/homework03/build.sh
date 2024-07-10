mkdir ~/build
pushd ~/build

CommonCompilerFlags=-std=c99

clang $CommonCompilerFlags ~/Projects/perfaware/part1/homework03/disassembler.c -o disassembler

popd
