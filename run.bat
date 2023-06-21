@echo off
set nocompiler=INFO: Could not find files for the given pattern(s).

for /f "tokens=* usebackq" %%f in (`where cl`) do set clvar=%%f
if "%clvar%"=="%clvar:nocompiler=%" (
  echo Found MSVC as the compiler
  echo.
  cl /DLC_LOCAL /F 268435456 /Zc:preprocessor /Z7 /std:c++17 /EHsc /nologo /Fe main.exe main.cpp && goto exec
  goto compileerr
)
for /f "tokens=* usebackq" %%f in (`where g++`) do set gccvar=%%f
if "%gccvar%"=="%gccvar:nocompiler=%" (
  echo Found GCC as the compiler
  echo.
  g++ -DLC_LOCAL -std=c++17 -Wshadow main.cpp -g -o main.exe -Wl,--stack,268435456 && goto exec
  goto compileerr
)
for /f "tokens=* usebackq" %%f in (`where clang++`) do set clangvar=%%f
if "%clangvar%"=="%clangvar:nocompiler=%" (
  echo Found Clang as the compiler
  echo.
  clang++ -DLC_LOCAL -std=c++17 -Wshadow main.cpp -g -o main.exe -Wl,--stack,268435456 && goto exec
  goto compileerr
)

echo you don't have a compiler in your PATH, nothing is built and ran
exit /B 1

:compileerr
echo compilation failed! nothing will be ran
exit /B 1

:exec
.\main.exe
echo.
type output.txt
