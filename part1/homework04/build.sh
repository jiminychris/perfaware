mkdir ~/build
pushd ~/build

CommonCompilerFlags=-std=c99

clang $CommonCompilerFlags ~/Projects/perfaware/part1/homework04/disassembler.c -o disassembler

popd
