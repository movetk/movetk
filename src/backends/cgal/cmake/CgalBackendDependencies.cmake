
# Find CGAL dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/common)
if(UNIX)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/unix)
elseif(MSVC)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/msvc)
endif()
find_package(CGAL REQUIRED)
CreateImportTarget(CGAL)

#Use different multiple precision frameworks between Unix and Windows
if(MSVC)
    find_package(GMP)
    if(NOT TARGET GMP::GMP AND NOT TARGET gmp::GMP)
        find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
    endif()
else()
    find_package(GMP REQUIRED)
endif()
CreateImportTarget(GMP)

find_package(MPFR)
if(NOT MPFR_FOUND)
    # Try in config mode with extra names
    message(STATUS "Finding MPFR in config mode")
    find_package(MPFR REQUIRED NAMES mpfr MPFR)
endif()
CreateImportTarget(MPFR)

# Restore module path
if(UNIX OR MSVC)
    list(POP_BACK CMAKE_MODULE_PATH)
endif()
list(POP_BACK CMAKE_MODULE_PATH)