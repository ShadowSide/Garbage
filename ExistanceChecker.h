#ifndef __EXISTANCE_CHECKER_H__
#define __EXISTANCE_CHECKER_H__

#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\accessory.h"

COMMENT((
»спользование:
TExistanceChecker - чекер, провер€ющий целостность объекта и отсутствие вызовов его функций, после вызова его деструктора.
const TExistanceChecker<0x8429f7a9l> ExistanceChecker;
0x8429f7a9l - случайна€ 32 разр€дна€ сигнатура. „ем случайней, тем лучше.
ExistanceChecker не должен быть static.
»з TExistanceChecker<0x8429f7a9l> можно делать предка провер€емого класса или объ€вл€ть частью провер€емого класса.
ExistanceChecker.Check(); - вызывать в каждой функции.
));

template<INT32U ExistanceSignatureGage>
class TExistanceChecker
{
public:
	void Check(void) const;
	bool IsExistance(void) const;
	TExistanceChecker(void);
	virtual ~TExistanceChecker(void);

private:
	mutable INT32U ExistanceSignature;
};

#include "Common\Realization\ExistanceCheckerRealization.h"

#endif