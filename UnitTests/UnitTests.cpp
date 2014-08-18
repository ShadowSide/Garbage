
#include <Common\accessory\TDebugConsoleMessage.h>
#include "Common\UnitTests\UnitTests.h"
#include <Common\UnitTests\UnitTestsWrapper.h>

namespace NUnitTestInternal
{		
	void runUnitTests(TAtomTrait<ON>)
	{
		NUnitTest::TUnitTests::run();
	} 
	
	void runUnitTests(TAtomTrait<OFF>)
	{} 

	NUnitTest::TUnitTests::TTestList TestList=
	{
		#include "UnitTests\UnitTestsList.h"
	};
};	
	
void NUnitTest::runUnitTests(void)
{
	NUnitTestInternal::runUnitTests(CURRENT_CONFIGURATION_TRAIT(TUnitTests)());
}

void NUnitTest::TUnitTests::runInternal(void)
{
	static const char* const BeginMessageFormat="Started %u Unit-tests.";
	static const char* const OnceErrorMessage="**ERROR** Unit-tests must start only once, but this violated. At every run of test lost system thread handle.";
	static const char* const InitializationUnknownExceptionMessageFormat="**ERROR** Initialization of unit-test \"%s\" with number %u throw unknown exception!";
	static const char* const InitializationExceptionMessageFormat="**ERROR** Initialization of unit-test \"%s\" with number %u throw exception \"%s\" !";
	static const char* const FinalizationUnknownExceptionMessageFormat="**ERROR** Finalization of unit-test \"%s\" with number %u throw unknown exception!";
	static const char* const FinalizationExceptionMessageFormat="**ERROR** Finalization of unit-test \"%s\" with number %u throw exception \"%s\" !";
	static const char* const UnknownUnitTestMessageFormat="**ERROR** Unknown unit-test \"%s\" with number %u! See file \"UnitTestsList.h\"";
	static const char* const ExecuteUnitTestMessageFormat="Execute unit-test \"%s\" with number %u.";
	static const char* const EndMessage="All unit-test complete.";
	static bool once=false;
	const unsigned AmountOfTests=std::distance(NUnitTestInternal::TestList, EndIteratorOfArray(NUnitTestInternal::TestList));
	DebugConsoleMessageFormated(BeginMessageFormat, AmountOfTests);
	if(once)
		DebugConsoleMessage(OnceErrorMessage);
	ASSERT(!once);
	once=true;
	TTestThread testThread;
	for(TTestListIterator testNamesIterator=NUnitTestInternal::TestList, End=EndIteratorOfArray(NUnitTestInternal::TestList); testNamesIterator!=End; ++testNamesIterator)
	{
		const char* const TestName=testNamesIterator->c_str();
		const unsigned TestNumber=std::distance(NUnitTestInternal::TestList, testNamesIterator)+1;
		DebugConsoleMessageFormated(ExecuteUnitTestMessageFormat, TestName, TestNumber);
		TTestNameToTestInstance::const_iterator testInstanceIterator = std::find(_testNameToTestInstance.begin(),  _testNameToTestInstance.end(), TestName);
		const TTestNameToTestInstance::const_iterator NotFound = _testNameToTestInstance.end();			
		if(testInstanceIterator==NotFound)
			DebugConsoleMessageFormated(UnknownUnitTestMessageFormat, TestName, TestNumber);
		UNIT_TEST_ASSERT(testInstanceIterator!=NotFound);//Тест с именем TestName не найден. Проверь файл UnitTestsList.h.			
		TAbstractUnitTest& testInstance=*(testInstanceIterator->_Test._TestInstance);
		const TSeconds TestTimeout=testInstanceIterator->_Test._Timeout;
		try{
			try{
				testInstance.initialize();
			}catch (std::exception& e){
				DebugConsoleMessageFormated(InitializationExceptionMessageFormat, TestName, TestNumber, e.what());
				UNIT_TEST_SHOULD_NEVER_REACH_HERE;//Ошибка в реализации инициализации модуля тестирования testInstance с именем TestName.
				throw;
			}
			try{
				if(!testThread.callAsyncTest(testInstance, TestTimeout, TestName, TestNumber))
					return;
			}catch (std::exception&){
				SHOULD_NEVER_REACH_HERE;//Ошибка в реализации системы юнит-тестирования.
			}
			try{
				testInstance.finalize();
			}catch (std::exception& e){
				DebugConsoleMessageFormated(FinalizationExceptionMessageFormat, TestName, TestNumber, e.what());
				UNIT_TEST_SHOULD_NEVER_REACH_HERE;//Ошибка в реализации финализации модуля тестирования testInstance с именем TestName.
				throw;
			}
		}catch(std::exception&){}
	}
	DebugConsoleMessage(EndMessage);
}

NUnitTest::TUnitTests& NUnitTest::TUnitTests::instance(void)
{
	static TUnitTests test;
	return test;
}

void NUnitTest::TUnitTests::add(const std::string& TestName, const TUnitTestInstnaceWithParameters& testInstance)
{
	_testNameToTestInstance.push_back(NUnitTest::TTestNameToTest(TestName, testInstance));
}

void NUnitTest::TUnitTests::run(void)
{
	TUnitTests::instance().runInternal();
}

void NUnitTest::TTestThread::body(void* /*const*/ this_)
{
	static const char* const UnknownExceptionMessageFormat="**ERROR** At unit-test \"%s\" with number %u throw unknown exception!";
	static const char* const ExceptionMessageFormat="**ERROR** At unit-test \"%s\" with number %u throw exception \"%s\"!";
	ASSERT(this_!=NULL);
	TTestThread* const This = static_cast<TTestThread*>(this_);
	ASSERT(This->_CastTestSignature==CAST_TEST_SIGNATURE);
	ASSERT(This->IsInitialized());
	for(;;)
		try{				
			This->WaitTestOrder();
			try{
				This->_processedUnitTest->main();
			}catch (std::exception& e){
				DebugConsoleMessageFormated(ExceptionMessageFormat, This->_testName, This->_testNumber, e.what());
				UNIT_TEST_SHOULD_NEVER_REACH_HERE;//Тест модульного тестирования с именем _testName и номером _testNumber кинул исключение e.				
			}
			This->TestOrderComplete();
		}catch(std::exception&){
			SHOULD_NEVER_REACH_HERE;//Ошибка в реализации системы юнит-тестирования.
		}
}

bool NUnitTest::TTestThread::callAsyncTest(TAbstractUnitTest& unitTest, const TSeconds UnitTestTimeout, const char* const TestName, const size_t TestNumber)
{
	ASSERT(this->IsInitialized());
	static const char* const LackFreeCPUTimeMessage="**ERROR** Lack free CPU time (Need increase UNIT_TEST_PRIORITY?) for unit-tests or lost unit-test executor thread.";
	static const char* const TimeoutMessageFormat="**ERROR** Unit-test \"%s\" with number %u expired time period %f seconds! All next tests skipped!.";
	_processedUnitTest=&unitTest;
	_testName=TestName;
	_testNumber=TestNumber;
	_ordinatorThread=this->CurrentTaskHandle();
	const bool SendOrderForTestSuccess=this->SendTestOrder();
	if(!SendOrderForTestSuccess)
	{
		DebugConsoleMessage(LackFreeCPUTimeMessage);
		UNIT_TEST_SHOULD_NEVER_REACH_HERE;//Возможно система слишком загружена и заказ на тест не дошел до исполнителя теста, но может быть и другая причина, например исполнитель заказов на тесты погиб.
		return false;
	}
	const bool TestTimeoutExceeded=!(this->WaitTestOrderComplete(UnitTestTimeout));
	if(TestTimeoutExceeded)
	{
		DebugConsoleMessageFormated(TimeoutMessageFormat, TestName, TestNumber, double(UnitTestTimeout));
		UNIT_TEST_SHOULD_NEVER_REACH_HERE;//Превышено время выполнения теста testInstance с именем TestName и таймаутом TestTimeout.
		return false;
	}
	return true;
}

NUnitTest::TTestThread* const NUnitTest::TTestThread::GetThis(void)
{
	return this;
}
