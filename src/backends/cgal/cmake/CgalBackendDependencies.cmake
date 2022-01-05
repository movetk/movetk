
# Find CGAL dependencies
find_package(CGAL REQUIRED)

#Use different multiple precision frameworks between Unix and Windows
if(UNIX)
    find_package(GMP REQUIRED)
elseif(MSVC)
    find_package(MPIR REQUIRED)  # exports GMP_INCLUDE_DIR and GMP::GMP
endif()
find_package(MPFR REQUIRED)