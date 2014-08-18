#ifndef __CRTHAPPY_H__
#define __CRTHAPPY_H__
#include <Common\accessory\accessory.h>
#include <Rttarget.h>

void patchCRT(void);

void FlushCPUCodeCache(void);

#ifdef RTOS_WRAPPER_BOOST_ALLOW
	#include "boost/type_traits.hpp"
#endif
#include "Common\accessory\functional_extension.h"

template<class TFunc1, class TFunc2>
void replaceFunctionByMemoryPatch(TFunc1 replacedFunction, TFunc2 functionReplacer)
{
#ifdef RTOS_WRAPPER_BOOST_ALLOW
	STATIC_CHECK((::boost::is_pointer<TFunc1>::value), replacedFunction_must_be_POINTER_to_function);
	STATIC_CHECK((::boost::is_pointer<TFunc2>::value), functionReplacer_must_be_POINTER_to_function);
	STATIC_CHECK((TIsFunctionalType<TFunc1>::Result), replacedFunction_must_be_pointer_to_FUNCTION);
	STATIC_CHECK((TIsFunctionalType<TFunc2>::Result), functionReplacer_must_be_pointer_to_FUNCTION);
#endif
	struct TRelative32Jmp
	{
		unsigned char opCode:8;
		TPointerArithmetic relative32AddressForJump:32;
	};

	const int OldAccessRights=RTGetPageAccess(replacedFunction);
	RTSetPageAccess(replacedFunction, RT_PG_USERREADWRITE);
	TRelative32Jmp* const replacedFunctionFirstBytes=reinterpret_cast<TRelative32Jmp*>(replacedFunction);
	enum {Relative32JumpOpcode=0xE9};
	replacedFunctionFirstBytes->opCode=Relative32JumpOpcode;
	replacedFunctionFirstBytes->relative32AddressForJump=reinterpret_cast<TPointerArithmetic>(functionReplacer)-reinterpret_cast<TPointerArithmetic>(replacedFunctionFirstBytes)-sizeof(TRelative32Jmp);
	RTSetPageAccess(replacedFunction, OldAccessRights);
	FlushCPUCodeCache();
}


#endif