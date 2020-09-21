REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

RD /S /Q build
MKDIR build
CD build
cmake -GNinja -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DWITH_CGAL_BACKEND=OFF -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_C_COMPILER=cl.exe -DBUILD_DOC=ON ..
cmake --build .
ctest -VV
