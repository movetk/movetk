#ifndef MOVETK_UTILS_ALGORITHMUTILS_H
#define MOVETK_UTILS_ALGORITHMUTILS_H 
namespace movetk::utils{

    /**
     * @brief Utility class to tag some element as an output element.
     * Allows to determine at compile time whether something should be written
     * @tparam OUTPUT The type of the output element
    */
    template<typename OUTPUT>
    struct Output {
        OUTPUT& target;

        explicit Output(OUTPUT& target_in) :target(target_in) {}

        static constexpr bool requires_output() { return true; }
    };
    /**
     * @brief Specialization that conveys that no output is required.
    */
    template<>
    struct Output<void> {
        static constexpr bool requires_output() { return false; }
    };
}
#endif