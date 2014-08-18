#include <Common\UnitTests\UnitTests.h>
#include <Common\Containers\containers_utility.hpp>
#include <boost/range/irange.hpp>
#include <boost/container/list.hpp>
#include <boost/container/slist.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/distance.hpp>
#include <Common\UnitTests\Mocks\StatisticaMockWithValue.h>

UNIT_TEST_DEFINE_GLOBAL_STATISTICA(statistica)

UNIT_TEST_MODULE_BEGIN(CONTAINERS_UTILITY_TEST, 5.0)

	UNIT_TEST_ENTRY_POINT
	{
		{
			int begin=0;
			int end=6;
			const int Wrong=-1;
			const boost::integer_range<int> input=boost::irange(0, end+1);
			UNIT_TEST_ASSERT(boost::distance(input)==end+1);
			UNIT_TEST_ASSERT(boost::distance(input)==input.size());
			UNIT_TEST_ASSERT(input.size()>0);
			{
				const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
				typedef int TValue;
				typedef boost::container::list<TValue> TBidirectionalContainer;
				typedef boost::container::slist<TValue> TForwardContainer;
				TBidirectionalContainer bi;
				TForwardContainer fo;				

				for(int i=begin; i!=end+1; ++i) fo.insert_after(fo.before_begin(), Wrong);
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(fo));
				const containers_utility::uninitialized_copy_result_t<TForwardContainer::iterator> result1 = containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), fo.begin());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(fo));
				UNIT_TEST_ASSERT(boost::range::equal(input, fo));
				UNIT_TEST_ASSERT(result1.m_unitialized_iterator==fo.end());
				UNIT_TEST_ASSERT(result1.m_sucessfull_inserted_items_amount==input.size());
				UNIT_TEST_ASSERT(result1.m_transaction_fully_successful);

				for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
				const containers_utility::uninitialized_copy_result_t<TBidirectionalContainer::iterator> result2 = containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), bi.begin());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
				UNIT_TEST_ASSERT(boost::range::equal(input, bi));
				UNIT_TEST_ASSERT(result2.m_unitialized_iterator==bi.end());
				UNIT_TEST_ASSERT(result2.m_sucessfull_inserted_items_amount==input.size());
				UNIT_TEST_ASSERT(result2.m_transaction_fully_successful);
			}

			{
				const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);
				typedef NUnitTest::NMock::TStatisticaMockWithValue<int, statistica>::type TValue;
				typedef boost::container::list<TValue> TBidirectionalContainer;
				typedef boost::container::slist<TValue> TForwardContainer;
				TBidirectionalContainer bi;
				TForwardContainer fo;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);

				NUnitTest::NMock::ignoreCreating(true);
				for(int i=begin; i!=end+1; ++i) fo.insert_after(fo.before_begin(), Wrong);
				NUnitTest::NMock::ignoreCreating(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(fo));
				const containers_utility::uninitialized_copy_result_t<TForwardContainer::iterator> result1 = containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), fo.begin());
				UNIT_TEST_ASSERT(statistica.instanceAmount==2*input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(fo));
				UNIT_TEST_ASSERT(boost::range::equal(input, fo));
				UNIT_TEST_ASSERT(result1.m_unitialized_iterator==fo.end());
				UNIT_TEST_ASSERT(result1.m_sucessfull_inserted_items_amount==input.size());
				UNIT_TEST_ASSERT(result1.m_transaction_fully_successful);

				NUnitTest::NMock::ignoreCreating(true);
				for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
				NUnitTest::NMock::ignoreCreating(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==3*input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
				const containers_utility::uninitialized_copy_result_t<TBidirectionalContainer::iterator> result2 = containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), bi.begin());
				UNIT_TEST_ASSERT(statistica.instanceAmount==4*input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
				UNIT_TEST_ASSERT(boost::range::equal(input, bi));
				UNIT_TEST_ASSERT(result2.m_unitialized_iterator==bi.end());
				UNIT_TEST_ASSERT(result2.m_sucessfull_inserted_items_amount==input.size());
				UNIT_TEST_ASSERT(result2.m_transaction_fully_successful);
			}
			UNIT_TEST_ASSERT(statistica.instanceAmount==2*input.size());
			statistica.instanceAmount=0;
			{
				const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);
				statistica.instanceAmount=0;
				typedef NUnitTest::NMock::NBehavior::NAspects::TThrowAtConstructObjectBehaviorAspect<4> TThrowDefaultStdExceptionAfterAround4Constructed;
				TThrowDefaultStdExceptionAfterAround4Constructed::throwCountMode(false);
				typedef NUnitTest::NMock::TStatisticaMockWithValue<int, statistica, TThrowDefaultStdExceptionAfterAround4Constructed>::type TValue;
				typedef boost::container::list<TValue> TBidirectionalContainer;
				typedef boost::container::slist<TValue> TForwardContainer;
				TBidirectionalContainer bi;
				TForwardContainer fo;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);

				NUnitTest::NMock::ignoreCreating(true);
				for(int i=begin; i!=end+1; ++i) fo.insert_after(fo.before_begin(), Wrong);
				NUnitTest::NMock::ignoreCreating(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
				TThrowDefaultStdExceptionAfterAround4Constructed::throwCountMode(true);

				UNIT_TEST_CHECK_THROW(
					containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), fo.begin());
				,NUnitTest::NMock::NBehavior::TMockTestSomeException);
				TThrowDefaultStdExceptionAfterAround4Constructed::throwCountMode(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(fo));
				
				NUnitTest::NMock::ignoreCreating(true);
				for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
				NUnitTest::NMock::ignoreCreating(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==2*input.size());
				TThrowDefaultStdExceptionAfterAround4Constructed::throwCountMode(true);
				UNIT_TEST_CHECK_THROW(
					containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), bi.begin());
				,NUnitTest::NMock::NBehavior::TMockTestSomeException);
				TThrowDefaultStdExceptionAfterAround4Constructed::throwCountMode(false);
				UNIT_TEST_ASSERT(statistica.instanceAmount==2*input.size());
				UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
				NUnitTest::NMock::objectIntegrityCheckMode(false);
				NUnitTest::NMock::ignoreCreating(true);
			}
			NUnitTest::NMock::ignoreCreating(false);
			NUnitTest::NMock::objectIntegrityCheckMode(true);
			statistica.instanceAmount=0;
			{
				const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);
				statistica.instanceAmount=0;
				typedef NUnitTest::NMock::TStatisticaMockWithValue<int, statistica>::type TValue;
				typedef std::vector<TValue> TContainer_;
				{
					const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
					TContainer_ bi;
					NUnitTest::NMock::ignoreCreating(true);
					for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
					NUnitTest::NMock::ignoreCreating(false);
					UNIT_TEST_ASSERT(input.size()==end+1);
					UNIT_TEST_ASSERT(bi.size()==input.size());
					UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
					const containers_utility::uninitialized_copy_result_t<TContainer_::iterator> result1 = containers_utility::uninitialized_copy_and_rollback_if_exception_or_overflow(input.begin(), input.end(), bi.begin(), bi.end());
					UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
					UNIT_TEST_ASSERT(boost::range::equal(input, bi));
					UNIT_TEST_ASSERT(statistica.instanceAmount==2*input.size());
					UNIT_TEST_ASSERT(result1.m_unitialized_iterator==bi.end());
					UNIT_TEST_ASSERT(result1.m_sucessfull_inserted_items_amount==input.size());
					UNIT_TEST_ASSERT(result1.m_transaction_fully_successful);
				}
				statistica.instanceAmount=0;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);
				{
					const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
					TContainer_ bi;
					UNIT_TEST_ASSERT(statistica.instanceAmount==0);
					NUnitTest::NMock::ignoreCreating(true);
					for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
					NUnitTest::NMock::ignoreCreating(false);
					UNIT_TEST_ASSERT(input.size()==end+1);
					UNIT_TEST_ASSERT(bi.size()==input.size());
					UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
					const containers_utility::uninitialized_copy_result_t<TContainer_::iterator> result2 = containers_utility::uninitialized_copy_and_rollback_if_exception_or_overflow(input.begin(), input.end(), bi.begin(), (bi.end()-1));
					UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
					UNIT_TEST_ASSERT(result2.m_unitialized_iterator==bi.begin());
					UNIT_TEST_ASSERT(result2.m_sucessfull_inserted_items_amount==0);
					UNIT_TEST_ASSERT(!(result2.m_transaction_fully_successful));
					NUnitTest::NMock::objectIntegrityCheckMode(false);
					NUnitTest::NMock::ignoreCreating(true);
				}
				NUnitTest::NMock::objectIntegrityCheckMode(true);
				NUnitTest::NMock::ignoreCreating(false);
				statistica.instanceAmount=0;
				UNIT_TEST_ASSERT(statistica.instanceAmount==0);
				{
					const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
					TContainer_ bi;
					UNIT_TEST_ASSERT(statistica.instanceAmount==0);
					NUnitTest::NMock::ignoreCreating(true);
					for(int i=begin; i!=end+1; ++i) bi.push_back(Wrong);
					NUnitTest::NMock::ignoreCreating(false);
					UNIT_TEST_ASSERT(input.size()==end+1);
					UNIT_TEST_ASSERT(bi.size()==input.size());
					UNIT_TEST_ASSERT(statistica.instanceAmount==input.size());
					const containers_utility::uninitialized_copy_result_t<TContainer_::iterator> result3 = containers_utility::uninitialized_copy_and_rollback_if_exception(input.begin(), input.end(), bi.begin(), bi.end()-1);
					UNIT_TEST_ASSERT(boost::distance(input)==boost::distance(bi));
					UNIT_TEST_ASSERT(std::equal(boost::begin(input), (boost::end(input)-1), boost::begin(bi)));
					UNIT_TEST_ASSERT(statistica.instanceAmount==((2*input.size())-1));
					UNIT_TEST_ASSERT(result3.m_unitialized_iterator==(bi.end()-1));
					UNIT_TEST_ASSERT(result3.m_sucessfull_inserted_items_amount==input.size()-1);
					UNIT_TEST_ASSERT(!(result3.m_transaction_fully_successful));
					NUnitTest::NMock::ignoreCreating(true);
				}
				NUnitTest::NMock::ignoreCreating(false);
			}
			statistica.instanceAmount=0;
			UNIT_TEST_ASSERT(statistica.instanceAmount==0);
		}
	}

UNIT_TEST_MODULE_END