#ifndef __UnitTests_h__
#define __UnitTests_h__

#include "Common\accessory\accessory.h"
#include "ProjectConfiguration.h"

COMMENT
((
	Юнит-тесты включаются в файле UnitTestsList.h директивой UNIT_TEST_ON.
	Чтобы написать тест - нужно создается cpp-файл:
	//////////////////////////////////////////////////////////////////////////
	
	#include <Common\UnitTests\UnitTests.h>
	#include <ТестируемыйХедер.h>

	UNIT_TEST_MODULE_BEGIN(ИМЯ_ТЕСТА_TEST, 5.0 /*время таймаута этого теста*/)//UNIT_TEST_MODULE_BEGIN объявляет экземпляр класса, который в конструкторе зарегистрирует тест в системе тестирования, так что все что ниже написано - члены и методы. Члены обзаны иметь конструктор по умолчанию. Объявлять в одном СPP файле более одного модуля тестирования UNIT_TEST_MODULE_BEGIN нельзя.

		UNIT_TEST_ENTRY_POINT//Также можно объявить UNIT_TEST_INITIALIZATION_POINT и UNIT_TEST_FINALIZATION_POINT таким же образом, в которых произвести нужную инициализацию и финализацию перед тестами, если нужно.
		{
			Сами Тесты
			...
		}

	UNIT_TEST_MODULE_END
	//////////////////////////////////////////////////////////////////////////

	Внутри UNIT_TEST_ENTRY_POINT и вызываемых её методов\функций доступно как минимум:
	1-Проверка, что не будет кинуто исключение:
		UNIT_TEST_CHECK_NOT_THROW_BLOCK_BEGIN
			f1();
			f2();
		UNIT_TEST_CHECK_NOT_THROW_BLOCK_END
		UNIT_TEST_CHECK_NOT_THROW(f3());
	2-Проверка, что будет кинуто исключение std::exception:
		UNIT_TEST_CHECK_THROW_BLOCK_BEGIN
			f1();
			f2();
			UNIT_TEST_CHECK_THROW_BLOCK_END(std::exception)
		UNIT_TEST_CHECK_THROW(f3(), std::exception);
	3-UNIT_TEST_ASSERT(Утверждение);
	4-UNIT_TEST_SHOULD_NEVER_REACH_HERE;
	Также есть возможность тестировать код в private и protected секция класса:
	UNIT_TEST_ALLOW_PRIVATE_TEST(имя_теста_в_котором_планируется_тестироваться_класс_в_котором_расположена_эта_директива);
		- позволяет тестировать private и protected часть класса, в котором используется эта директива. Для использования этой директивы 
		следует подключить хедер RTOS_Wrapper\Common\UnitTests\UnitTestsExternal.h
		Также перед тестируемым классом следует использовать UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6 для того чтобы тест работал в MSVC 6ке.
		К сожалению 6ка не осилила полную эмуляцию и применять из шаблоннного класса UNIT_TEST_ALLOW_PRIVATE_TEST нельзя. Поэтому есть обходной путь,
		представленный в RTOS_Wrapper\Common\UnitTests\UnitTest Shared Modules\MemoryAlignmentUnitTest.cpp, путем наследования от шаблонного класса макросов. Fear!
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
		typedef std::deque<TTestNameToTest> TTestNameToTestInstance;//Беда с std::map<...> (и даже с std::deque<...>) в MS VC 6.5. Оплакиваем. Скорбим. Вставляем костыли везде по коду.
		
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

	//Следует также написать макросы для проверки таймаутов функций. 
	//А также нужно написать макросы сравнения строк и чисел с плавющими точками, тк это тонкие места С++.
}

#endif // Tests_h__
