
#include <Common\UnitTests\UnitTests.h>
#include <Common\InternalClasses\MemoryAligment.h>

namespace
{
	template<size_t MaxAlignmentStep>
	struct TMemoryAlignmentFriend: TMemoryAlignment<MaxAlignmentStep>
	{
		typedef TMemoryAlignment<MaxAlignmentStep> TBase;
		static size_t getAlignmentStep(const size_t MaxSize)
		{
			return TBase::getAlignmentStep(MaxSize);
		}
	};

	void* address(const uintptr_t Address, const intptr_t Shift=0)
	{
		return (void*)(Address+Shift);
	}

	template<size_t TestedAlignment>
	struct TEndestAlignedAddress
	{
		static void* execute(const intptr_t Shift=0)
		{
			return ((void*)((((intptr_t)(-1))-(TMemoryAlignmentFriend<TestedAlignment>::MaxAlignmentStep)+1)+(Shift)));
		}
	};
}

UNIT_TEST_MODULE_BEGIN(MEMORY_ALIGNMENT_TEST, 5.0)

	UNIT_TEST_ENTRY_POINT
	{
		//Test unit-test:
		UNIT_TEST_ASSERT(TEndestAlignedAddress<1>::execute()==address(0xffffffff));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<1>::execute(-1)==address(0xfffffffe));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<1>::execute(-2)==address(0xfffffffd));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<2>::execute()==address(0xfffffffe));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<2>::execute(-1)==address(0xfffffffd));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<2>::execute(-2)==address(0xfffffffc));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<4>::execute()==address(0xfffffffe-2));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<4>::execute(-1)==address(0xfffffffe-2, -1));
		UNIT_TEST_ASSERT(TEndestAlignedAddress<4>::execute(-2)==address(0xfffffffe-2, -2));

		#ifndef CPU_SUITABLE_MEMORY_ALIGNMENT_STEP
			STATIC_CHECK(false, CPU_SUITABLE_MEMORY_ALIGNMENT_STEP_NotDefined);
		#endif
		STATIC_CHECK(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP>0, CPU_SUITABLE_MEMORY_ALIGNMENT_STEP_wrong_value);
		UNIT_TEST_ASSERT(TSystemMemoryAligment::alignSize(1)==1);
		UNIT_TEST_ASSERT(TSystemMemoryAligment::alignSize(2)==2);
		UNIT_TEST_ASSERT(TSystemMemoryAligment::alignSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TSystemMemoryAligment::alignSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TSystemMemoryAligment::isAlignedSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TSystemMemoryAligment::isAlignedSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TSystemMemoryAligment::isAlignedSize(1));
		UNIT_TEST_ASSERT(TSystemMemoryAligment::isAlignedSize(2));

		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::alignSize(1)==1);
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::alignSize(2)==2);
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::alignSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::alignSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::isAlignedSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::isAlignedSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::isAlignedSize(1));
		UNIT_TEST_ASSERT(TCPUSuitableMemoryAlignment::isAlignedSize(2));

		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::alignSize(1)==1);
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::alignSize(2)==2);
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::alignSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP-1)==2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP-1);
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::alignSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::alignSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP)==2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP);
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::isAlignedSize(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::isAlignedSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP));
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::isAlignedSize(2*CPU_SUITABLE_MEMORY_ALIGNMENT_STEP-1));
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::isAlignedSize(1));
		UNIT_TEST_ASSERT(TWithoutMemoryAlignment::isAlignedSize(2));

		{
			enum{TestedAlignment=1};

			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::MaxAlignmentStep)==TestedAlignment, _);
			
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(1)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(2)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(3)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(4)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(5)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(6)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(7)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(8)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(9)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(10)==1);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(1)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(2)==2*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(3)==3*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(4)==4*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(5)==5*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(6)==6*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(7)==7*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(8)==8*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(9)==9*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(10)==10*TestedAlignment);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(2));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(3));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(6));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(7));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(8));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(9));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, 1)==NULL);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, 1)==(void*)1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, 1)==(void*)2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, 1)==(void*)3);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, 1)==(void*)4);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, 1)==(void*)5);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, 7)==NULL);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, 7)==(void*)1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, 7)==(void*)2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, 7)==(void*)3);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, 7)==(void*)4);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, 7)==(void*)5);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, 8)==NULL);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, 8)==(void*)1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, 8)==(void*)2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, 8)==(void*)3);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, 8)==(void*)4);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, 8)==(void*)5);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment), 1)==(void*)(0xfffffffe-2*TestedAlignment));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment-1), 1)==(void*)(0xfffffffe-2*TestedAlignment-1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment+1), 1)==(void*)(0xfffffffe-2*TestedAlignment+1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-TestedAlignment), 1)==(void*)(0xfffffffe-TestedAlignment));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment), 6)==(void*)(0xfffffffe-2*TestedAlignment));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment-1), 6)==(void*)(0xfffffffe-2*TestedAlignment-1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment+1), 6)==(void*)(0xfffffffe-2*TestedAlignment+1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-TestedAlignment), 6)==(void*)(0xfffffffe-TestedAlignment));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment), 4)==(void*)(0xfffffffe-2*TestedAlignment));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment-1), 4)==(void*)(0xfffffffe-2*TestedAlignment-1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment+1), 4)==(void*)(0xfffffffe-2*TestedAlignment+1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-TestedAlignment), 4)==(void*)(0xfffffffe-TestedAlignment));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment), 1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment-1), 1));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment), 4));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment-1), 4));

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment), 5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xffffffff-TestedAlignment-1), 5));

			/*//STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<0>::value)==TestedAlignment, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<1>::value)==1, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<2>::value)==2, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<3>::value)==3, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<4>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<5>::value)==5, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<6>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<7>::value)==7, _);

			//STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<0>::value)==TestedAlignment, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<1>::value)==1, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<2>::value)==2, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<3>::value)==3, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<4>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<5>::value)==5, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<6>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<7>::value)==7, _);*/
		}

		{
			enum {TestedAlignment=2};

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(1)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(2)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(3)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(4)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(5)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(6)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(7)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(8)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(9)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(10)==2);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(1)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(2)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(3)==2*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(4)==2*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(5)==3*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(6)==3*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(7)==4*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(8)==4*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(9)==5*TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(10)==5*TestedAlignment);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(2));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(3));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(4));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(5));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(6));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(7));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(8));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(9));
		
			{
				enum {MaxSize=1};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, MaxSize)==(void*)1);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, MaxSize)==(void*)3);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, MaxSize)==(void*)5);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)6, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)7, MaxSize)==(void*)7);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)8, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)9, MaxSize)==(void*)9);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)10, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment), MaxSize)==(void*)(0xfffffffe-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment-1), MaxSize)==(void*)(0xfffffffe-2*TestedAlignment-1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-2*TestedAlignment+1), MaxSize)==(void*)(0xfffffffe-2*TestedAlignment+1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)(0xfffffffe-TestedAlignment), MaxSize)==(void*)(0xfffffffe-TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)6, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)7, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)8, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)9, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)10, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xfffffffe-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xfffffffe-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xfffffffe-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)(0xfffffffe-TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=2};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)6, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)7, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)8, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)9, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)10, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)6, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)7, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)8, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)9, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)10, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=8};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)6, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)7, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)8, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)9, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)10, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)6, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)7, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)8, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)9, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)10, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)15, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)16, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)17, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=5};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)1, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)2, MaxSize)==(void*)2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)3, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)4, MaxSize)==(void*)4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)5, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)6, MaxSize)==(void*)6);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)7, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)8, MaxSize)==(void*)8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)9, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor((void*)10, MaxSize)==(void*)10);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)1, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)2, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)3, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)4, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)5, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)6, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)7, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)8, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)9, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)10, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)15, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)16, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor((void*)17, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize));
			}

			/*//STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<0>::value)==TestedAlignment, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<1>::value)==2, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<2>::value)==2, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<3>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<4>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<5>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<6>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignSize<7>::value)==8, _);

			//STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<0>::value)==TestedAlignment, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<1>::value)==1, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<2>::value)==2, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<3>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<4>::value)==4, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<5>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<6>::value)==6, _);
			STATIC_CHECK((TMemoryAlignmentFriend<TestedAlignment>::TAlignMaxSize<7>::value)==8, _);*/
		}

		
		{
			enum {TestedAlignment=4};

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(1)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(2)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(3)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(4)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(5)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(6)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(7)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(8)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(9)==TestedAlignment);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(10)==TestedAlignment);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(1)==1);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(2)==2);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(3)==4);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(4)==4);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(5)==8);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(6)==8);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(7)==8);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(8)==8);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(9)==12);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(10)==12);
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(16)==16);

			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(1));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(2));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(3));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(4));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(5));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(6));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(7));
			UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(8));
			UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(9));

			{
				enum {MaxSize=1};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(3));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(5));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(7));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(9));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=2};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment-MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=8};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(15), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(16), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(12), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(15), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(16), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(17), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=5};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(12), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(15), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(16), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(17), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));

			}			
		}

		{	
			enum {TestedAlignment=8};
			{
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(1)==1);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(2)==2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(3)==4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(4)==4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(5)==TestedAlignment);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(6)==TestedAlignment);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(7)==TestedAlignment);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(8)==TestedAlignment);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(9)==TestedAlignment);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::getAlignmentStep(10)==TestedAlignment);

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(1)==1);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(2)==2);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(3)==4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(4)==4);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(5)==8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(6)==8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(7)==8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(8)==8);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(9)==16);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(10)==16);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignSize(16)==16);

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(2));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(3));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(4));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(5));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(6));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(7));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(8));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedSize(9));				
			}
			
			{
				enum {MaxSize=1};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(3));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(5));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(7));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(9));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment+1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute( -TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=2};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(2));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(6));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(10));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=8};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(15), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(16), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(12), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(15), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(16), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(17), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=5};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(16));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(12), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(15), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(16), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(17), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize));
			}

			{
				enum {MaxSize=3};		
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(NULL, MaxSize)==NULL);
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(1), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(2), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(3), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(4), MaxSize)==address(4));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(5), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(6), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(7), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(8), MaxSize)==address(8));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(9), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(address(10), MaxSize)==address(12));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+MaxSize+1));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::alignAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize)==TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment));

				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(NULL, MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(2), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(3), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(4), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(5), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(6), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(7), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(8), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(9), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(10), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(12), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(15), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(16), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(address(17), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment-1), MaxSize));
				UNIT_TEST_ASSERT(!TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-2*TestedAlignment+1), MaxSize));
				UNIT_TEST_ASSERT(TMemoryAlignmentFriend<TestedAlignment>::isAlignedAddressFor(TEndestAlignedAddress<TestedAlignment>::execute(-TestedAlignment), MaxSize));
			}
		}
	}
UNIT_TEST_MODULE_END
