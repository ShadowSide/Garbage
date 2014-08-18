#include <Common\UnitTests\UnitTests.h>
#include <Common\Containers\inplace_fixed_deque.hpp>
#include <boost/range/irange.hpp>
#include <utility>
#include <Common\UnitTests\Mocks\StatisticaMockWithValue.h>
#include <boost/range/iterator_range_core.hpp>
#include <algorithm>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/end.hpp>
#include <boost/range/begin.hpp>
#include <vector>

UNIT_TEST_DEFINE_GLOBAL_STATISTICA(statistica)

UNIT_TEST_MODULE_BEGIN(INPLACE_FIXED_DEQUE_TEST, 5.0)

	template<class TValue, bool not_check_statistica>
	void test(void)
	{
		#define INSTANCE_AMOUNT_MUST_EQUAL_TO(ME) UNIT_TEST_ASSERT(not_check_statistica||(statistica.instanceAmount==(ME)))
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			{
				TValue v;
				(void)v;
				INSTANCE_AMOUNT_MUST_EQUAL_TO(1);
			}
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			typedef inplace_fixed_deque<TValue, 3> TContainer;
			TContainer c;
			UNIT_TEST_ASSERT(c.empty());		
			const TContainer c1((c));
			UNIT_TEST_ASSERT(c1.empty());
			TContainer c2;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			c.push_back(0);
			UNIT_TEST_ASSERT(c.size()==1);
			UNIT_TEST_ASSERT(c.front()==0);
			UNIT_TEST_ASSERT(c.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(1);
			c.push_front(-1);
			UNIT_TEST_ASSERT(c.size()==2);
			UNIT_TEST_ASSERT(c.front()==-1);
			UNIT_TEST_ASSERT(c.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(2);
			c2=c;
			UNIT_TEST_ASSERT(c2.size()==2);
			UNIT_TEST_ASSERT(c2.front()==-1);
			UNIT_TEST_ASSERT(c2.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(1);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.front()==-1);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==0);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.size()==1);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);
			c.push_back(2);
			UNIT_TEST_ASSERT(c.back()==2);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(3);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(4);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(5);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(6);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_back(7);
			UNIT_TEST_ASSERT(c.back()==7);
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			UNIT_TEST_ASSERT(c[0]==5);
			UNIT_TEST_ASSERT(c[1]==6);
			UNIT_TEST_ASSERT(c[2]==7);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_front(4);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_front(3);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_front(2);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.push_front(1);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			UNIT_TEST_ASSERT(c[0]==1);
			UNIT_TEST_ASSERT(c[1]==2);
			UNIT_TEST_ASSERT(c[2]==3);

			UNIT_TEST_ASSERT(c2.size()==2);
			UNIT_TEST_ASSERT(c2.front()==-1);
			UNIT_TEST_ASSERT(c2.back()==0);

			UNIT_TEST_ASSERT(c2[0]==-1);
			UNIT_TEST_ASSERT(c2[1]==0);

			UNIT_TEST_ASSERT(c.size()==3);
			UNIT_TEST_ASSERT(c2.size()==2);

			c2=c;

			UNIT_TEST_ASSERT(!c2.empty());
			UNIT_TEST_ASSERT(c2.size()==3);
			UNIT_TEST_ASSERT(c2[0]==1);
			UNIT_TEST_ASSERT(c2[1]==2);
			UNIT_TEST_ASSERT(c2[2]==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(6);

			c2.clear();
			UNIT_TEST_ASSERT(c2.empty());
			UNIT_TEST_ASSERT(c2.size()==0);

			UNIT_TEST_ASSERT(!c.empty());
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);

			TContainer::iterator ib1=c.begin();
			TContainer::iterator ie1=c.end();
			const TContainer::const_iterator icb1=ib1;
			const TContainer::const_iterator ice1=ie1;
			TContainer::const_iterator icb2=icb1;
			TContainer::const_iterator ice2=ice1;

			int left_border=1;
			int right_border_not_included=4;
			boost::integer_range<int> check=boost::irange(left_border, right_border_not_included);
			UNIT_TEST_ASSERT(std::distance(icb2, ice2)==boost::distance(check));
			UNIT_TEST_ASSERT(std::distance(icb2, ice2)==c.size());
			UNIT_TEST_ASSERT(boost::equal(boost::make_iterator_range(icb2, ice2), check));

			left_border=5;
			right_border_not_included=left_border+3;
			check=boost::irange(left_border, right_border_not_included);

			*ib1=left_border;
			const int t=*ib1+1;
			*++ib1=t;
			*++ib1=t+1;
			UNIT_TEST_ASSERT(ib1++!=c.end());
			UNIT_TEST_ASSERT(ib1==c.end());
			UNIT_TEST_ASSERT(std::distance(ib1, ie1)==0);
			ib1=c.begin();
			UNIT_TEST_ASSERT(std::distance(ib1, ie1)==c.size());
			UNIT_TEST_ASSERT(boost::equal(boost::make_iterator_range(ib1, ie1), check));
			++ib1;
			UNIT_TEST_ASSERT(*--ib1==left_border);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);

			const TContainer c3(c);
			UNIT_TEST_ASSERT(c3.size()==c.size());
			UNIT_TEST_ASSERT(c3[0]==c[0]);
			UNIT_TEST_ASSERT(c3.begin()==c3.cbegin());
			UNIT_TEST_ASSERT(c3.end()==c3.cend());
			UNIT_TEST_ASSERT(c.begin()==c.cbegin());
			UNIT_TEST_ASSERT(c.end()==c.cend());
			UNIT_TEST_ASSERT(c.cbegin()==c.begin());
			UNIT_TEST_ASSERT(c.cend()==c.end());
			UNIT_TEST_ASSERT(c.cend()==c.cend());
			UNIT_TEST_ASSERT(c3.cend()==c3.cend());
			UNIT_TEST_ASSERT(c3.cend()==c3.end());
			UNIT_TEST_ASSERT(c3.end()==c3.cend());
			{
				const TContainer::const_iterator icb_=c.begin();
				TContainer::iterator ie_=c.begin();
				UNIT_TEST_ASSERT(ie_==icb_);
			}
			{
				TContainer::const_iterator icb_=c.begin();
				const TContainer::iterator ie_=c.begin();
				UNIT_TEST_ASSERT(icb_==ie_);
			}
			INSTANCE_AMOUNT_MUST_EQUAL_TO(6);
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			typedef inplace_fixed_deque<TValue, 3> TContainer;
			TContainer c;
			boost::integer_range<int> ir=boost::irange(1, 4);
			std::vector<int> iv(boost::const_begin(ir), boost::const_end(ir));
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			c=ir;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);
			c=iv;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);
			std::reverse(boost::begin(iv), boost::end(iv));
			UNIT_TEST_ASSERT(iv.size()==3);
			UNIT_TEST_ASSERT(iv[0]==3);
			UNIT_TEST_ASSERT(iv[1]==2);
			UNIT_TEST_ASSERT(iv[2]==1);
			UNIT_TEST_ASSERT(c.size()==3);
			UNIT_TEST_ASSERT(!boost::range::equal(c, iv));
			UNIT_TEST_ASSERT(boost::range::equal(boost::make_iterator_range(c.rbegin(),c.rend()), iv));
			std::reverse(boost::begin(c), boost::end(c));
			UNIT_TEST_ASSERT(boost::range::equal(c, iv));
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{//Check emplace
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			typedef inplace_fixed_deque<TValue, 3> TContainer;
			TContainer c;
			UNIT_TEST_ASSERT(c.empty());		
			TContainer c2;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			c.emplace_back(0);
			UNIT_TEST_ASSERT(c.size()==1);
			UNIT_TEST_ASSERT(c.front()==0);
			UNIT_TEST_ASSERT(c.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(1);
			c.emplace_front(-1);
			UNIT_TEST_ASSERT(c.size()==2);
			UNIT_TEST_ASSERT(c.front()==-1);
			UNIT_TEST_ASSERT(c.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(2);
			c2=c;
			UNIT_TEST_ASSERT(c2.size()==2);
			UNIT_TEST_ASSERT(c2.front()==-1);
			UNIT_TEST_ASSERT(c2.back()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(1);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.front()==-1);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==0);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.back()==1);
			UNIT_TEST_ASSERT(c.size()==1);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);
			c.emplace_back(2);
			UNIT_TEST_ASSERT(c.back()==2);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(3);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(4);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(5);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(6);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			
			c.pop_front();
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_back(7);
			UNIT_TEST_ASSERT(c.back()==7);
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			
			UNIT_TEST_ASSERT(c[0]==5);
			UNIT_TEST_ASSERT(c[1]==6);
			UNIT_TEST_ASSERT(c[2]==7);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.front()==5);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_front(4);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.back()==6);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.front()==4);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_front(3);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.back()==5);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.front()==3);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_front(2);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.back()==4);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			c.pop_back();
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.front()==2);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			c.emplace_front(1);
			UNIT_TEST_ASSERT(c.front()==1);
			UNIT_TEST_ASSERT(c.back()==3);
			UNIT_TEST_ASSERT(c.size()==3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);

			UNIT_TEST_ASSERT(c[0]==1);
			UNIT_TEST_ASSERT(c[1]==2);
			UNIT_TEST_ASSERT(c[2]==3);
			
			c.clear();
			
			INSTANCE_AMOUNT_MUST_EQUAL_TO(2);
			
			c.emplace_front();
			c.emplace_back();
			
			UNIT_TEST_ASSERT(c.front()==0);
			UNIT_TEST_ASSERT(c.back()==0);
			UNIT_TEST_ASSERT(c.size()==2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			typedef inplace_fixed_deque<TValue, 3> TContainer;
			TContainer c;
			UNIT_TEST_ASSERT(c.begin()==c.end());
			UNIT_TEST_ASSERT(c.cbegin()==c.cend());
			UNIT_TEST_ASSERT(c.cbegin()==c.end());
			UNIT_TEST_ASSERT(std::distance(c.begin(), c.end())==0);
			UNIT_TEST_ASSERT(std::distance(c.cbegin(), c.cend())==0);
			UNIT_TEST_ASSERT(c.rbegin()==c.rend());
			UNIT_TEST_ASSERT(c.crbegin()==c.crend());
			UNIT_TEST_ASSERT(c.crbegin()==c.rend());
			UNIT_TEST_ASSERT(std::distance(c.rbegin(), c.rend())==0);
			UNIT_TEST_ASSERT(std::distance(c.crbegin(), c.crend())==0);
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		#undef INSTANCE_AMOUNT_MUST_EQUAL_TO
	}

	UNIT_TEST_ENTRY_POINT
	{
		{
			typedef int TValue;
			test<TValue, true>();
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			typedef NUnitTest::NMock::TStatisticaMockWithValue<int, statistica>::type TValue;
			test<TValue, false>();
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			ASSERT(statistica.instanceAmount==0);
			statistica.instanceAmount=0;
			typedef NUnitTest::NMock::NBehavior::NAspects::TThrowAtConstructObjectBehaviorAspect<3> TThrowAtConstructObjectBehavior;
			typedef NUnitTest::NMock::TStatisticaMockWithValue<int, statistica, TThrowAtConstructObjectBehavior >::type TThrowAfterNConstruction;
			TThrowAtConstructObjectBehavior::throwCountMode(true);
			typedef inplace_fixed_deque<TThrowAfterNConstruction, 4> TContainer;
			TContainer c2;
			c2.push_back(0);
			c2.push_back(1);
			UNIT_TEST_ASSERT(c2.size()==2);
			UNIT_TEST_ASSERT(statistica.instanceAmount==2);
			boost::integer_range<int> src=boost::irange(1,3);
			UNIT_TEST_CHECK_THROW(
				c2=src;
			,NUnitTest::NMock::NBehavior::TMockTestSomeException);
			UNIT_TEST_ASSERT(c2.size()==0);
			UNIT_TEST_ASSERT(c2.begin()==c2.end());
			UNIT_TEST_ASSERT(c2.cbegin()==c2.cend());
			UNIT_TEST_ASSERT(c2.cbegin()==c2.end());
			UNIT_TEST_ASSERT(c2.begin()==c2.cend());
			UNIT_TEST_ASSERT(c2.cend()==c2.cend());
			UNIT_TEST_ASSERT(statistica.instanceAmount==0);
		}
		{
			struct T
			{
				T(void){}
				T(int){}
				T(int, int){}
				T(int, int, int){}
				T(int, int, int, int){}
			};
			typedef inplace_fixed_deque<T, 10> TContainer;
			TContainer c;
			c.emplace_back();
			c.emplace_front();
			c.emplace_back(1);
			c.emplace_front(1);
			c.emplace_back(1,2);
			c.emplace_front(1,2);
			c.emplace_back(1,2,3);
			c.emplace_front(1,2,3);
			c.emplace_back(1,2,3,4);
			c.emplace_front(1,2,3,4);
			UNIT_TEST_ASSERT(c.size()==10);
		}
	}

UNIT_TEST_MODULE_END