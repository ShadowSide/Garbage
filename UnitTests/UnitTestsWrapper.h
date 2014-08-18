#ifndef UnitTestsWrapper_h__
#define UnitTestsWrapper_h__

#include <Common\accessory\accessory.h>
#include <Global_consts.h>
#include <Common\UnitTests\UnitTests.h>

namespace NUnitTest
{
	class TTestThread//Чтобы не зависить от изменений во враппере.
	{
		enum {
			UNIT_TEST_PRIORITY=DEFAULT_PRIO,
			UNIT_TEST_ORDER_TIMEOUT=KERNEL_TICKS_PER_SECOND*2
		};
		
		TTestThread(const TTestThread&);//forbidden member
		const TTestThread& operator=(const TTestThread&);//forbidden member	
		
		const unsigned volatile _CastTestSignature;
		const TSystemHandle _UnitTestThread;
		
		TAbstractUnitTest* _processedUnitTest;
		const char* _testName;
		unsigned _testNumber;
		TSystemHandle _ordinatorThread;		
		enum
		{
			CAST_TEST_SIGNATURE=0xffcd458a
		};
		
		static TSystemHandle CurrentTaskHandle(void);
		
		void WaitTestOrder(void) const;
		void TestOrderComplete(void) const;
		bool SendTestOrder(void) const;
		bool WaitTestOrderComplete(const TSeconds UnitTestTimeout) const;
		
		static void RTKAPI body(void* /*const */ this_);
		bool IsInitialized(void) const;
		TTestThread* const GetThis(void);
		
	public:
		bool callAsyncTest(TAbstractUnitTest& unitTest, const TSeconds UnitTestTimeout, const char* const TestName, const size_t TestNumber);		
		TTestThread(void);
		virtual ~TTestThread(void);
	};
}

#endif // UnitTestsWrapper_h__
