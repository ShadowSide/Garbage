
#include <Common\UnitTests\Mocks\MocksCommon.h>
#include <exception>

void NUnitTest::NMock::NBehavior::NMockBehaviorInternal::MockTestSomeExceptionThrowerFunction(void)
{
	throw NUnitTest::NMock::NBehavior::TMockTestSomeException();
}

NUnitTest::NMock::NBehavior::TExceptionThrowerFunction NUnitTest::NMock::NBehavior::MockTestSomeExceptionThrower=NUnitTest::NMock::NBehavior::NMockBehaviorInternal::MockTestSomeExceptionThrowerFunction;

bool NUnitTest::NMock::NMockInternal::TMockBase::_enabledObjectIntegrityCheck=true;
bool NUnitTest::NMock::NMockInternal::TMockBase::_ignoreCreating=false;

bool NUnitTest::NMock::objectIntegrityCheckMode(const bool Enable)
{
	return NUnitTest::NMock::NMockInternal::TMockBase::objectIntegrityCheckMode(Enable);
}

bool NUnitTest::NMock::ignoreCreating(const bool Enable)
{
	return NUnitTest::NMock::NMockInternal::TMockBase::ignoreCreating(Enable);
}

bool NUnitTest::NMock::createdIsEmpty(void)
{
	return NUnitTest::NMock::NMockInternal::TMockBase::createdIsEmpty();
}

size_t NUnitTest::NMock::createdAmount(void)
{
	return NUnitTest::NMock::NMockInternal::TMockBase::createdAmount();
}

void NUnitTest::NMock::resetCreated(void)
{
	return NUnitTest::NMock::NMockInternal::TMockBase::resetCreated();
}

NUnitTest::NMock::NMockInternal::TMockBase::TObjectsTracker NUnitTest::NMock::NMockInternal::TMockBase::_objectsTracker;