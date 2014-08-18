#ifndef __UnitTests_h__
#define __UnitTests_h__

#include "Common\accessory\accessory.h"
#include "ProjectConfiguration.h"

COMMENT
((
	����-����� ���������� � ����� UnitTestsList.h ���������� UNIT_TEST_ON.
	����� �������� ���� - ����� ��������� cpp-����:
	//////////////////////////////////////////////////////////////////////////
	
	#include <Common\UnitTests\UnitTests.h>
	#include <����������������.h>

	UNIT_TEST_MODULE_BEGIN(���_�����_TEST, 5.0 /*����� �������� ����� �����*/)//UNIT_TEST_MODULE_BEGIN ��������� ��������� ������, ������� � ������������ �������������� ���� � ������� ������������, ��� ��� ��� ��� ���� �������� - ����� � ������. ����� ������ ����� ����������� �� ���������. ��������� � ����� �PP ����� ����� ������ ������ ������������ UNIT_TEST_MODULE_BEGIN ������.

		UNIT_TEST_ENTRY_POINT//����� ����� �������� UNIT_TEST_INITIALIZATION_POINT � UNIT_TEST_FINALIZATION_POINT ����� �� �������, � ������� ���������� ������ ������������� � ����������� ����� �������, ���� �����.
		{
			���� �����
			...
		}

	UNIT_TEST_MODULE_END
	//////////////////////////////////////////////////////////////////////////

	������ UNIT_TEST_ENTRY_POINT � ���������� � �������\������� �������� ��� �������:
	1-��������, ��� �� ����� ������ ����������:
		UNIT_TEST_CHECK_NOT_THROW_BLOCK_BEGIN
			f1();
			f2();
		UNIT_TEST_CHECK_NOT_THROW_BLOCK_END
		UNIT_TEST_CHECK_NOT_THROW(f3());
	2-��������, ��� ����� ������ ���������� std::exception:
		UNIT_TEST_CHECK_THROW_BLOCK_BEGIN
			f1();
			f2();
			UNIT_TEST_CHECK_THROW_BLOCK_END(std::exception)
		UNIT_TEST_CHECK_THROW(f3(), std::exception);
	3-UNIT_TEST_ASSERT(�����������);
	4-UNIT_TEST_SHOULD_NEVER_REACH_HERE;
	����� ���� ����������� ����������� ��� � private � protected ������ ������:
	UNIT_TEST_ALLOW_PRIVATE_TEST(���_�����_�_�������_�����������_�������������_�����_�_�������_�����������_���_���������);
		- ��������� ����������� private � protected ����� ������, � ������� ������������ ��� ���������. ��� ������������� ���� ��������� 
		������� ���������� ����� RTOS_Wrapper\Common\UnitTests\UnitTestsExternal.h
		����� ����� ����������� ������� ������� ������������ UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6 ��� ���� ����� ���� ������� � MSVC 6��.
		� ��������� 6�� �� ������� ������ �������� � ��������� �� ����������� ������ UNIT_TEST_ALLOW_PRIVATE_TEST ������. ������� ���� �������� ����,
		�������������� � RTOS_Wrapper\Common\UnitTests\UnitTest Shared Modules\MemoryAlignmentUnitTest.cpp, ����� ������������ �� ���������� ������ ��������. Fear!
))

#include <string>
#include <deque>

#include "Common\UnitTests\UnitTestsExternal.h"

#if (_MSC_VER<1400) && defined(_MSC_VER) //for MS VC6.0
	#define TTestNameToTest TTNTT
#endif

namespace NUnitTest
{
	#define UNIT_TEST_ENTRY_POINT void main(void)
	#define UNIT_TEST_INITIALIZATION_POINT void initialize(void)
	#define UNIT_TEST_FINALIZATION_POINT void finalize(void)

	struct TUnitTestBase
	{
		UNIT_TEST_INITIALIZATION_POINT{}
		UNIT_TEST_ENTRY_POINT{}
		UNIT_TEST_FINALIZATION_POINT{}
	};

	struct TAbstractUnitTest
	{
		virtual UNIT_TEST_INITIALIZATION_POINT{}
		virtual UNIT_TEST_ENTRY_POINT=0;
		virtual UNIT_TEST_FINALIZATION_POINT{}
	};

	struct TUnitTestInstnaceWithParameters
	{
		TAbstractUnitTest* const _TestInstance;
		const TSeconds _Timeout;
		
		TUnitTestInstnaceWithParameters(TAbstractUnitTest* const TestInstance, const TSeconds Timeout):
			_TestInstance(TestInstance), _Timeout(Timeout)
		{}
		
	private:
		TUnitTestInstnaceWithParameters(void);//forbidden member
	};

	struct TTestNameToTest
	{
		const std::string _TestName;
		const TUnitTestInstnaceWithParameters _Test;

		TTestNameToTest(const std::string TestName, const TUnitTestInstnaceWithParameters& Test):
			_TestName(TestName),
			_Test(Test)
		{}

		bool operator==(const std::string& TestName) const
		{
			return TestName==_TestName;
		}
		
	private:
		TTestNameToTest(void);//forbidden member
	};
	
	class TUnitTests
	{
		typedef std::deque<TTestNameToTest> TTestNameToTestInstance;//���� � std::map<...> (� ���� � std::deque<...>) � MS VC 6.5. ����������. �������. ��������� ������� ����� �� ����.
		
		TTestNameToTestInstance _testNameToTestInstance;
				
	public:
		typedef std::string TTestListItem;
		typedef const TTestListItem* TTestListIterator;
		typedef const TTestListItem TTestList[];
		static TTestList _TestList;
		
		static TUnitTests& instance(void);
		
		void add(const std::string& TestName, const TUnitTestInstnaceWithParameters& testInstance);
		
		static void run(void);
		
	private:
		TUnitTests(const TUnitTests&);//forbidden member
		const TUnitTests& operator=(const TUnitTests&);//forbidden member
		TUnitTests(void){}
		
		void runInternal(void);
	};
	
	#define UNIT_TEST_ON(TestNameForTestList) (#TestNameForTestList)
	
	template<class TUnitTest>
	struct TTypeErasureUnitTest: ::NUnitTest::TAbstractUnitTest
	{
		TUnitTest& _unitTest;

		TTypeErasureUnitTest(TUnitTest& unitTest):
			_unitTest(unitTest)
		{}
		
		UNIT_TEST_ENTRY_POINT
		{
			_unitTest.main();
		}
		
		UNIT_TEST_INITIALIZATION_POINT
		{
			_unitTest.initialize();
		}
		
		UNIT_TEST_FINALIZATION_POINT
		{
			_unitTest.finalize();
		}
	};
	
	#define UNIT_TEST_MODULE_BEGIN(TestName, TestTimeout)																					\
		template <class> struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName);																	\
		typedef UNIT_TEST_INTERNAL_CLASS_NAME(TestName)<CURRENT_CONFIGURATION_TRAIT(TUnitTests)> TCurrentUnitTest;							\
		template<> struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName)<TAtomTrait<OFF> >: ::NUnitTest::TUnitTestBase								\
		{};																																	\
		template<> struct UNIT_TEST_INTERNAL_CLASS_NAME(TestName)<TAtomTrait<ON> >: ::NUnitTest::TUnitTestBase								\
		{																																	\
			typedef UNIT_TEST_INTERNAL_CLASS_NAME(TestName) TMe;																			\
			TMe(void)																														\
			{																																\
				::NUnitTest::TUnitTests::instance().add((#TestName),																		\
				::NUnitTest::TUnitTestInstnaceWithParameters(new ::NUnitTest::TTypeErasureUnitTest<TMe>(*this), (TestTimeout)));			\
			}
	
	#define UNIT_TEST_MODULE_END }; TCurrentUnitTest CONCAT_MACRO(TestInstance_AtLine_,__LINE__);
	
	#define UNIT_TEST_CHECK_NOT_THROW_BLOCK_BEGIN try{
	#define UNIT_TEST_CHECK_NOT_THROW_BLOCK_END }catch(std::exception&){SHOULD_NEVER_REACH_HERE;}
	
	#define UNIT_TEST_CHECK_NOT_THROW(Statment) try{{Statment;};}catch(std::exception&){SHOULD_NEVER_REACH_HERE;}
	
	#define UNIT_TEST_CHECK_THROW_BLOCK_BEGIN try{
	#define UNIT_TEST_CHECK_THROW_BLOCK_END(Exception) ;SHOULD_NEVER_REACH_HERE;}catch(Exception&){}catch(std::exception&){SHOULD_NEVER_REACH_HERE;}
	
	#define UNIT_TEST_CHECK_THROW(Statment, Exception) try{{Statment;}SHOULD_NEVER_REACH_HERE;}catch(Exception&){}catch(std::exception&){SHOULD_NEVER_REACH_HERE;}
	
	#define UNIT_TEST_ASSERT(Expression) ASSERT(Expression)	
	#define UNIT_TEST_SHOULD_NEVER_REACH_HERE SHOULD_NEVER_REACH_HERE

	//������� ����� �������� ������� ��� �������� ��������� �������. 
	//� ����� ����� �������� ������� ��������� ����� � ����� � ��������� �������, �� ��� ������ ����� �++.
}

#endif // Tests_h__
