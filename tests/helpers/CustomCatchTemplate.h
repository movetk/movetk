#ifndef MOVETK_CUSTOMCATCHTEMPLATE_H
#define MOVETK_CUSTOMCATCHTEMPLATE_H
#include <catch2/catch.hpp>
#include "../test_includes.h"

#define MOVETK_TEMPLATE_LIST_TEST_CASE_METHOD(test_class, test_name, test_tags) \
	TEMPLATE_LIST_TEST_CASE_METHOD(test_class, test_name, test_tags, movetk::test::AvailableBackends)
#define MOVETK_TEMPLATE_LIST_TEST_CASE(test_name, test_tags) \
	TEMPLATE_LIST_TEST_CASE(test_name, test_tags, movetk::test::AvailableBackends)

#define MOVETK_EXPAND(...) __VA_ARGS__
#define MOVETK_CONCAT(A, B) A B
#define MOVETK_TEMPLATED_BACKEND_TEST_CASE_METHOD(className, definitions, test_name, test_tags) \
	MOVETK_INTERNAL_TEMPLATE_LIST_TEST_CASE_METHOD(className,                                     \
	                                               definitions,                                   \
	                                               test_name,                                     \
	                                               test_tags,                                     \
	                                               movetk::test::AvailableBackends)

#define MOVETK_INTERNAL_TEMPLATE_LIST_TEST_CASE_METHOD(ClassName, Definitions, Name, Tags, TmplList) \
	MOVETK_INTERNAL_TEMPLATE_LIST_TEST_CASE_METHOD2(                                                   \
	    INTERNAL_CATCH_UNIQUE_NAME(____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____),                   \
	    INTERNAL_CATCH_UNIQUE_NAME(____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____),        \
	    ClassName,                                                                                     \
	    Definitions,                                                                                   \
	    Name,                                                                                          \
	    Tags,                                                                                          \
	    TmplList)


#define MOVETK_INTERNAL_TEMPLATE_LIST_TEST_CASE_METHOD2(TestNameClass,                                                \
                                                        TestName,                                                     \
                                                        ClassName,                                                    \
                                                        Definitions,                                                  \
                                                        Name,                                                         \
                                                        Tags,                                                         \
                                                        TmplList)                                                     \
	CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                                                                           \
	CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS                                                                            \
	CATCH_INTERNAL_SUPPRESS_UNUSED_TEMPLATE_WARNINGS                                                                    \
	template <typename TestType>                                                                                        \
	struct TestName : public INTERNAL_CATCH_REMOVE_PARENS(ClassName<TestType>) {                                        \
		using Fixture = INTERNAL_CATCH_REMOVE_PARENS(ClassName<TestType>);                                                \
		MOVETK_CONCAT(MOVETK_EXPAND, Definitions)                                                                         \
		void test();                                                                                                      \
	};                                                                                                                  \
	namespace {                                                                                                         \
	namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName) {                                                                 \
		INTERNAL_CATCH_TYPE_GEN                                                                                           \
		template <typename... Types>                                                                                      \
		struct TestNameClass {                                                                                            \
			void reg_tests() {                                                                                              \
				int index = 0;                                                                                                \
				using expander = int[];                                                                                       \
				(void)expander{                                                                                               \
				    (Catch::AutoReg(Catch::makeTestInvoker(&TestName<Types>::test),                                           \
				                    CATCH_INTERNAL_LINEINFO,                                                                  \
				                    #ClassName,                                                                               \
				                    Catch::NameAndTags{Name " - " + std::string(INTERNAL_CATCH_STRINGIZE(TmplList)) + " - " + \
				                                           std::to_string(index),                                             \
				                                       Tags}),                                                                \
				     index++)...}; /* NOLINT */                                                                               \
			}                                                                                                               \
		};                                                                                                                \
		static int INTERNAL_CATCH_UNIQUE_NAME(globalRegistrar) = []() {                                                   \
			using TestInit = typename convert<TestNameClass, TmplList>::type;                                               \
			TestInit t;                                                                                                     \
			t.reg_tests();                                                                                                  \
			return 0;                                                                                                       \
		}();                                                                                                              \
	}                                                                                                                   \
	}                                                                                                                   \
	CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION                                                                            \
	template <typename TestType>                                                                                        \
	void TestName<TestType>::test()


#endif