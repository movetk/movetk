from conans import ConanFile

# Dependencies for using the CGAL backend
CGAL_DEPENDENCIES = [
    "gmp/6.2.1",
    "mpfr/4.1.0",
    "cgal/5.3"
]


class MovetkDependencies(ConanFile):
    name = "movetk-dependencies"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = ("gdal/3.4.1",
                "gsl/2.7",
                "rapidjson/1.1.0",
                "boost/1.77.0",
                "zlib/1.2.12"
                )
    generators = "CMakeToolchain", "CMakeDeps"
    # Provide options such that different dependency sets can be installed
    options = {
        "cgal_backend": [True, False],
        "boost_backend": [True, False],
        "testing": [True, False],
        "docs": [True, False],
        "examples": [True, False]
    }
    default_options = {"cgal_backend": False,
                       "boost_backend": True,
                       "testing": False,
                       "docs": False,
                       "examples": False,
                       "gdal:shared": True,
                       "boost:without_context": True,
                       "boost:without_contract": True,
                       "boost:without_coroutine": True,
                       "boost:without_fiber": True,
                       "boost:without_graph_parallel": True,
                       "boost:without_json": True,
                       "boost:without_mpi": True,
                       "boost:without_nowide": True,
                       "boost:without_program_options": True,
                       "boost:without_python": True,
                       "boost:without_stacktrace": True,
                       "boost:without_test": True,
                       "boost:without_timer": True,
                       "boost:without_type_erasure": True,
                       "boost:without_wave": True}

    def requirements(self):
        if self.options.testing:
            self.requires("catch2/2.13.6")
        if self.options.cgal_backend:
            for dependency in CGAL_DEPENDENCIES:
                self.requires(dependency)
        if self.options.examples:
            self.requires('cxxopts/3.0.0')

    def imports(self):
        self.copy("*.dll", "./bin", "bin")
