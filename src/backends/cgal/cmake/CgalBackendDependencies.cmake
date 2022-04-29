
# Find CGAL dependencies
find_package(CGAL REQUIRED)

#Use different multiple precision frameworks between Unix and Windows
find_package(GMP)
if(MSVC AND NOT TARGET GMP::GMP AND NOT TARGET gmp::GMP)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/msvc)
    find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
    list(POP_BACK CMAKE_MODULE_PATH)
endif()
if(TARGET gmp::GMP)
    add_library(GMP::GMP ALIAS gmp::GMP)
endif()
find_package(MPFR REQUIRED)
if(NOT TARGET MPFR::MPFR)
    message(FATAL_ERROR "Could not find MPFR target")    
endif()
