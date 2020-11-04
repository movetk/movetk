:: Base folder for visual studio compiler
SET VC_FOLDER = C:/Program\ Files\ (x86)/Microsoft\ Visual\ Studio/2019/Enterprise/VC
:: Setup VC variables on path
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
:: Suffix for compiler executable in the VC_FOLDER
SET CL_SUFFIX = Tools/MSVC/14.27.29016/bin/Hostx64/x64/cl.exe

RD /S /Q build
MKDIR build
CD build
cmake -GNinja -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DWITH_CGAL_BACKEND=OFF -DCMAKE_CXX_COMPILER=%VC_FOLDER%/%CL_SUFFIX% -DCMAKE_C_COMPILER=%VC_FOLDER%/%CL_SUFFIX% -DBUILD_DOC=ON ..
cmake --build .
ctest -VV
