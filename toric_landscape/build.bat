@ECHO ON

RMDIR /Q /S build
MKDIR build
PUSHD build

conan install .. -s build_type=Release
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
