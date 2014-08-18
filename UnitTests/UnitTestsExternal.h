#ifndef UnitTestsExternal_h__
#define UnitTestsExternal_h__

#include "ProjectConfiguration.h"

namespace NUnitTest
{
	void runUnitTests(void);
	
	#define UNIT_TEST_INTERNAL_CLASS_NAME(TestName) TUnitTest_##TestName
	
#if (_MSC_VER<1400) && defined(_MSC_VER) //for MS VC6.0
	#define UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6(TestName) template<class> struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName)
	#define UNIT_TEST_ALLOW_PRIVATE_TEST(TestName) friend struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName)<TAtomTrait<ON> >
#else
	#define UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6(TestName)
	#define UNIT_TEST_ALLOW_PRIVATE_TEST(TestName) template <class>	friend struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName)
#endif
}
#endif // UnitTestsExternal_h__