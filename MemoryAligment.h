#ifndef __MEMORYALIGMENT_H__
#define __MEMORYALIGMENT_H__

#include "Common\accessory\accessory.h"
#include "Common\accessory\BitsAlgorithms.h"
#include "Common\UnitTests\UnitTestsExternal.h"

//UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6(MEMORY_ALIGNMENT_TEST);

//��������� ������������ ������. ������ ������������� � TMemoryPoolSimple, � ��� �������� � � ��������� ������ ��� 2.0.
template <const size_t MaxAlignmentStep>
class TMemoryAlignment
{
	//UNIT_TEST_ALLOW_PRIVATE_TEST(MEMORY_ALIGNMENT_TEST);//�������� � MSVC 6�� �� ���������� �� ���������� ������, ���� ��� �� ���������� �������� � ������ �����. ��� ������ ��������� � ������ ������ ����� ����� �������, ������������ protected �����.
	
protected:
	static size_t getAlignmentStep(size_t MaxSize, const size_t MaxAlignmentStep);//������ ���������� ��� ������� ������� ��� ������������, �� �� ����� ��� MaxAlignmentStep, ��������� � ��������� �������.

	//��������������, ������� ���� � private, �� ���� �����, �� ����� �������.
	static size_t alignSizeTo(size_t Size, const size_t AlignmentStep);//����������� ������ �� ������������ AlignmentStep ��� ������� ��������� ������.
	static size_t getAlignmentStep(const size_t MaxSize);//������ ���������� ��� ������� ������� ��� ������������, �� �� ����� ��� MaxAlignmentStep.
	static bool isAlignedAddressTo(void* const Address, const size_t AlignmentStep);//��������� ������������ ������������ ������ � ������������ � ������������ alignAddressTo
	static void* alignAddressTo(void* const Address, const size_t AlignmentStep);//����������� ����� �� ������������ AlignmentStep ��� ������� ��������� ������.
	
public:
	static void* alignAddressFor(void* const Address, const size_t MaxSize);//����������� ����� �� ������������ ��������� � MaxAlignmentStep � ���� ������ MaxSize ��������� ��������� � ������� �������� ������, �� ����������� �� AlignmentStep, �������, ��� MaxAlignmentStep.
	static bool isAlignedAddressFor(void* const Address, const size_t MaxSize);//��������� ������������ ������������ ������ � ������������ � ������������ alignAddressFor
	static size_t alignSize(const size_t MaxSize);//����������� ������ �� ������������ ��������� � MaxAlignmentStep � ���� ������ MaxSize ��������� ��������� � ������� �������� ������, �� ����������� �� AlignmentStep, �������, ��� MaxAlignmentStep.
	static bool isAlignedSize(const size_t MaxSize);//��������� ������������ ������������ ������� � ������������ � ������������ alignSize.	

	enum
	{
		MaxAlignmentStep=MaxAlignmentStep
	};

/*private:
	template<size_t Size>
	struct TAlignSizeInternal
	{
		enum 
		{
			value=Size+(Size%MaxAlignmentStep==0 ? 0 : MaxAlignmentStep-(Size%MaxAlignmentStep))
		};

	private:
		MEMBER_STATIC_CHECK(Size!=0, assertionSizeCantBeNull);
	};

public:
	template<size_t MaxSize>
	struct TAlignSize
	{
		enum 
		{
			value=MaxSize>=MaxAlignmentStep ? TAlignSizeInternal<MaxSize>::value : TBitsAlgorithms::TRoundUpPowerOfTwoIncludingZeroPower<MaxSize>::value
		};

	private:
		MEMBER_STATIC_CHECK(MaxSize!=0, assertionMaxSizeCantBeNull);
	};*/

private:
	MEMBER_STATIC_CHECK(MaxAlignmentStep==MaxAlignmentStep, assertionWrongRealization);
	MEMBER_STATIC_CHECK(TBitsAlgorithms::TIsPowerOfTwoIncludingZeroPower<MaxAlignmentStep>::value, assertionMaxAlignmentStepMustBePowerOfTwo);
};

template <>
class TMemoryAlignment<0>;

typedef TMemoryAlignment<CPU_SUITABLE_MEMORY_ALIGNMENT_STEP> TCPUSuitableMemoryAlignment;//���������� ������������ ������ ��� ������� ���������.
typedef TMemoryAlignment<1> TWithoutMemoryAlignment;//�� ������������ ������������ ������.

#ifdef DISABLE_MEMORY_ALIGNMENT
	typedef TWithoutMemoryAlignment TSystemMemoryAligment;
#else
	typedef TCPUSuitableMemoryAlignment TSystemMemoryAligment;
#endif


namespace NMemoryAlignmentInternal
{
	static void assertion(void)
	{
#ifndef CPU_SUITABLE_MEMORY_ALIGNMENT_STEP
		STATIC_CHECK(false, CPU_SUITABLE_MEMORY_ALIGNMENT_STEP_NotDefined);
#endif
		STATIC_CHECK(CPU_SUITABLE_MEMORY_ALIGNMENT_STEP>0, CPU_SUITABLE_MEMORY_ALIGNMENT_STEP_wrong_value);
	}	
};

#include "Common\Realization\memoryAligmentRealization.h"

#endif