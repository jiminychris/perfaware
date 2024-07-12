@echo off

if not exist w:\build mkdir w:\build
pushd w:\build

set CommonCompilerFlags=%Optimize% /Zi /fp:fast /W4 /WX /nologo /wd4996 /wd4100 /wd4065 /wd4189 /wd4201 /wd4505
set CommonLinkerFlags=/DEBUG /WX /NOLOGO /OPT:REF

cl %CommonCompilerFlags% w:\perfaware\part1\homework08\disassembler.c /link %CommonLinkerFlags%

popd
