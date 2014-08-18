#ifndef BitsAlgorithms_h__
#define BitsAlgorithms_h__

#include "Common\accessory\accessory.h"
#if( (_MSC_VER>=1400) && (defined(_MSC_VER)) )// for >=MSVS2005 and for Microsoft compilers
	#include "intrin.h"
#endif
#include "Common\UnitTests\UnitTestsExternal.h"

UNIT_TEST_ALLOW_PRIVATE_TEST_THIS_MODULE_FOR_MSVC_6(BITS_ALGORITHMS_TEST);

struct TBitsAlgorithms
{
	UNIT_TEST_ALLOW_PRIVATE_TEST(BITS_ALGORITHMS_TEST);

private:
#if( (_MSC_VER>=1400) && (defined(_MSC_VER)) )// for >=MSVS2005 and for Microsoft compilers
	static size_t RoundUpPowerOfTwoIncludingZeroPowerCompilerSpecific(const size_t Value);//Same as RoundUpPowerOfTwoIncludingZeroPowerCrossPlatform
#endif
	static size_t RoundUpPowerOfTwoIncludingZeroPowerCrossPlatform(const size_t Value);
	static size_t RoundUpPowerOfTwoIncludingZeroPowerAlsoReturnZeroIfZero(size_t Value);

public:
	enum {
		MAX_TEMPLATE_VALUE=1073741824/*2^30*/,
		MAX_VALUE=MAX_TEMPLATE_VALUE//2147483648/*2^31*/
	};

	static bool IsPowerOfTwoIncludingZeroPower(const size_t Value);
	static size_t RoundUpPowerOfTwoIncludingZeroPower(const size_t Value);

	

	template<size_t Value>
	struct TIsPowerOfTwoIncludingZeroPower
	{
		enum 
		{
			value=Value != 0 && ((Value & (Value - 1)) == 0)
		};

	private:
		MEMBER_STATIC_CHECK(Value<=MAX_TEMPLATE_VALUE, assertionValueOutOfRange);
	};

	template<size_t Value>
	struct TRoundUpPowerOfTwoIncludingZeroPower
	{
	private:
		enum{
			v1= Value-1,
			v2= v1 | v1 >> 1,
			v4= v2 | v2 >> 2,
			v8= v4 | v4 >> 4,
			v16= v8 | v8 >> 8,
			v32= v16 | v16 >> 16,
			vr= v32+1
		};

	public:
		enum 
		{
			value=vr
		};

	private:
		MEMBER_STATIC_CHECK(sizeof(Value)==4&&sizeof(value)==4, assertionNotSuitableRealizationForThisPlatform);
		MEMBER_STATIC_CHECK(Value<=MAX_TEMPLATE_VALUE, assertionValueOutOfRange);
		MEMBER_STATIC_CHECK(TIsPowerOfTwoIncludingZeroPower<value>::value, assertionNotCorrectRealization);
	};
};

#endif // BitsAlgorithms_h__