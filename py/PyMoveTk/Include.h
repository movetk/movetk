#ifndef PYMOVETK_INCLUDE_H
#define PYMOVETK_INCLUDE_H
#include <pybind11/pybind11.h>
namespace PyMovetk{

    template<typename T>
    struct Type
    {
        using type = T;
    };

    // Specialize by redefining the struct with the below elements
    template<typename ObjectType>
    struct PythonObjectInterface{
        // Tuple of pairs of names and types, where types should be function pointers
        static auto definitions(){}

        static constexpr const char* name() { return ""; } //return the name of the object

        // Override if extra elements should be registered.
        static void after_base_register(pybind11::class_<ObjectType>& class_def){}
    };

    template<typename ObjectType>
    struct RegisterPy{
        static void register_object(pybind11::module& mod)
        {
            pybind11::class_<ObjectType> cls(mod, PythonObjectInterface<ObjectType>::name());
            // Register all elements
        }
        template<typename ClsPointer>
        static void register_member(pybind11::class_<ObjectType>& clsDef, const std::pair<const char*, ClsPointer>& pntr)
        {
            clsDef.def()
        }
    };
}
#endif