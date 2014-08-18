#ifndef __EXISTANCE_CHECKER_H__
#define __EXISTANCE_CHECKER_H__

#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\accessory.h"

COMMENT((
�������������:
TExistanceChecker - �����, ����������� ����������� ������� � ���������� ������� ��� �������, ����� ������ ��� �����������.
const TExistanceChecker<0x8429f7a9l> ExistanceChecker;
0x8429f7a9l - ��������� 32 ��������� ���������. ��� ���������, ��� �����.
ExistanceChecker �� ������ ���� static.
�� TExistanceChecker<0x8429f7a9l> ����� ������ ������ ������������ ������ ��� ��������� ������ ������������ ������.
ExistanceChecker.Check(); - �������� � ������ �������.
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