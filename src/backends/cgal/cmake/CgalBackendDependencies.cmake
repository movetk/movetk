
# Find CGAL dependencies
if(UNIX)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/unix)
endif()
find_package(CGAL REQUIRED)
if(UNIX)
    list(POP_BACK CMAKE_MODULE_PATH)
endif()
CreateImportTarget(CGAL)

#Use different multiple precision frameworks between Unix and Windows
find_package(GMP)
if(MSVC AND NOT TARGET GMP::GMP AND NOT TARGET gmp::GMP)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/msvc)
    find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
    list(POP_BACK CMAKE_MODULE_PATH)
endif()
CreateImportTarget(GMP)
find_package(MPFR)
if(NOT MPFR_FOUND)
    # Try in config mode with extra names
    message(STATUS "Finding MPFR in config mode")
    find_package(MPFR REQUIRED NAMES mpfr MPFR)
endif()
CreateImportTarget(MPFR)