
# Find CGAL dependencies
find_package(CGAL REQUIRED)

#Use different multiple precision frameworks between Unix and Windows
if(UNIX)
    find_package(GMP REQUIRED)
elseif(MSVC)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/msvc)
    find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
    list(POP_BACK CMAKE_MODULE_PATH)
endif()
find_package(MPFR REQUIRED)
if(NOT TARGET MPFR::MPFR)
    message(FATAL_ERROR "Could not find MPFR target")    
endif()
