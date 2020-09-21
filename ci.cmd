REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"

RD /S /Q build
MKDIR build
CD build
cmake -GNinja -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DWITH_CGAL_BACKEND=OFF ..
cmake --build .
ctest -VV
