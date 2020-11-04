:: Base folder for visual studio compiler
SET VC_FOLDER=C:/Program\ Files\ (x86)/Microsoft\ Visual\ Studio/2019/Enterprise/VC
:: Setup VC variables on path
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
:: Suffix for compiler executable in the VC_FOLDER
SET CL_SUFFIX=Tools/MSVC/14.27.29016/bin/Hostx64/x64/cl.exe
RD /S /Q build
MKDIR build
CD build
SET CC=cl.exe
SET CXX=cl.exe
cmake -GNinja -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DWITH_CGAL_BACKEND=OFF -DBUILD_DOC=ON ..
cmake --build .
ctest -VV
