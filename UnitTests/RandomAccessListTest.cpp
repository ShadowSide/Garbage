
#include <Common\UnitTests\UnitTests.h>
#include <Common\UnitTests\Mocks\StatisticaMockWithValue.h>
#include <Common\Containers\random_access_list.hpp>
#include <Common\Containers\allocator_from_pool_for_node_bassed_containers.h>
#include <boost\phoenix.hpp>
#include <list>
#include <boost\unordered_set.hpp>
#include <hash_set>
#include <boost/range/algorithm.hpp>
#include <boost/range/size.hpp>
#include <boost/range/irange.hpp>
#include <boost/config/suffix.hpp>
#include <stdlib.h>
#include <Common/Containers/malloc_allocator.h>


UNIT_TEST_DEFINE_GLOBAL_STATISTICA(statistica1)

#define INSTANCE_AMOUNT_MUST_EQUAL_TO(ME) UNIT_TEST_ASSERT(statistica1.instanceAmount==(ME))
typedef ::NUnitTest::NMock::TStatisticaMockWithValue<int, statistica1>::type TMockValue;
typedef TMockValue TValue;

UNIT_TEST_MODULE_BEGIN(RANDOM_ACCESS_LIST_TEST, 20.0)
enum { 
	MAX_ITEMS_AMOUNT_WITH_ALLOCATOR = 6,
	MAX_ITEMS_AMOUNT_AT_BEFORE_CONSTRUCTED_COLLECTION=3,
	MAX_ITEMS_AMOUNT_AT_BEFORE_FULL_CONSTRUCTED_COLLECTION_WHEN_CONSTRUCT=MAX_ITEMS_AMOUNT_AT_BEFORE_CONSTRUCTED_COLLECTION*2,
	MAX_ITEMS_AMOUNT_AT_ALLOCATOR = MAX_ITEMS_AMOUNT_WITH_ALLOCATOR + MAX_ITEMS_AMOUNT_AT_BEFORE_FULL_CONSTRUCTED_COLLECTION_WHEN_CONSTRUCT
};
typedef ::NSmallSingleBlockStdAllocator::small_single_block_std_allocator<container::random_access_list<TValue>::MAX_SIZE_FOR_ALLOCATOR, MAX_ITEMS_AMOUNT_AT_ALLOCATOR, TNewDeleteAllocatorCanThrow, true, true > my_allocator;
	
	template<class C, class C2>
	void checksEmptyAndCheckEquality(C& empty, C2& empty2){
		checksEmpty(empty);
		checksEmpty(empty2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		UNIT_TEST_ASSERT(empty==empty2);
		UNIT_TEST_ASSERT(!(empty<empty2));
		UNIT_TEST_ASSERT(empty<=empty2);
		UNIT_TEST_ASSERT(!(empty>empty2));
		UNIT_TEST_ASSERT(empty>=empty2);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::size(empty2)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty2, empty));
		empty.selfCheck();
		empty2.selfCheck();
		::boost::swap(empty, empty2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty=empty2;
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty=::boost::move(empty2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.assign(empty.cbegin(), empty.cend());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.assign(empty.begin(), empty.end());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
	}

	template<class C, class C2>
	void checkEmptyWithEqualAllocators(C& empty, C2& empty2){
		ASSERT(empty.get_allocator() == empty2.get_allocator());
		ASSERT(empty.empty());
		ASSERT(empty.size()==0);
		ASSERT(empty2.empty());
		ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.merge(empty2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.merge(::boost::move(empty2));
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		using namespace boost::phoenix::arg_names;
		empty.merge(empty2, arg1<arg2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.merge(::boost::move(empty2), arg1<arg2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), empty2);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), ::boost::move(empty2));
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), empty2, empty2.begin());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), ::boost::move(empty2), empty2.begin());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), empty2, empty2.begin(), empty2.end());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
		empty.splice(empty.end(), ::boost::move(empty2), empty2.begin(), empty2.end());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty2.empty());
		UNIT_TEST_ASSERT(empty2.size()==0);
		empty.selfCheck();
		empty2.selfCheck();
	}

	template<class C>
	void checksEmpty(C& empty){
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(empty==empty);
		UNIT_TEST_ASSERT(!(empty<empty));
		UNIT_TEST_ASSERT(empty<=empty);
		UNIT_TEST_ASSERT(!(empty>empty));
		UNIT_TEST_ASSERT(empty>=empty);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		::boost::swap(empty, empty);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty=empty;
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.assign(empty.cbegin(), empty.cend());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.assign(empty.begin(), empty.end());
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.clear();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.resize(0);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.resize(0, 0);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.reverse();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.sort();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		(void)empty.max_size();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.unique();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		empty.remove(0);
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		using namespace boost::phoenix;
		empty.remove_if(val(true));
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
		(void)empty.algorithmic_complexity();
		UNIT_TEST_ASSERT(empty.empty());
		UNIT_TEST_ASSERT(empty.size()==0);
		UNIT_TEST_ASSERT(::boost::size(empty)==0);
		UNIT_TEST_ASSERT(::boost::equal(empty, empty));
		UNIT_TEST_ASSERT(::boost::equal(empty | ::boost::adaptors::reversed, empty | ::boost::adaptors::reversed));
		empty.selfCheck();
	}

	template<class TContainer1, class TContainer2>
	void checkEquality(const TContainer1& goingToTest, const TContainer2& right){
		goingToTest.selfCheck();
		UNIT_TEST_ASSERT(goingToTest.empty()==right.empty());
		UNIT_TEST_ASSERT(goingToTest.size()==right.size());
		//UNIT_TEST_ASSERT(::boost::size(goingToTest)==::boost::size(right));
		UNIT_TEST_ASSERT(::boost::equal(goingToTest, right));
		UNIT_TEST_ASSERT(::boost::equal(goingToTest | ::boost::adaptors::reversed, right | ::boost::adaptors::reversed));
		goingToTest.selfCheck();
	}
	
	template<class TContainer1>
	void checkDefaultConstructable(const ::std::size_t amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		typedef int TPureValue;
		TContainer1 goingToTest(amount);
		checkEquality(goingToTest, ::std::vector<TPureValue>(amount));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkDefaultConstructableWithIntAmount(const int amount){
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		typedef int TPureValue;
		TContainer1 goingToTest(amount);
		checkEquality(goingToTest, ::std::vector<TPureValue>(::std::size_t(amount)));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkConstructable(const ::std::size_t amount, const int value){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		typedef int TPureValue;
		::std::vector<TPureValue> gage(amount, value);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const TContainer1 goingToTest(amount, value);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
		checkEquality(goingToTest, gage);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkConstructableWithIntAmount(const int amount, const int value){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		typedef int TPureValue;
		const TContainer1 goingToTest(amount, value);
		checkEquality(goingToTest, ::std::vector<TPureValue>(::std::size_t(amount), value));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	::boost::integer_range<int> makeIncreasedRangeFromAmount(const int amount){
		return ::boost::irange(1,amount+1);
	}
	
	::std::vector<int>makeIncreasedContainerFromAmount(const int amount){
		return std::vector<int>(::boost::begin(makeIncreasedRangeFromAmount(amount)), ::boost::end(makeIncreasedRangeFromAmount(amount)));
	}
	template<class TContainer>
	void checkIncreased(const TContainer& tested, const ::std::size_t amount){
		if(!amount)
			return;
		UNIT_TEST_ASSERT(tested.front()==1);
		UNIT_TEST_ASSERT(tested.back()==amount);
		UNIT_TEST_ASSERT(*tested.begin()==1);
		UNIT_TEST_ASSERT(*(tested.end()-1)==amount);
		UNIT_TEST_ASSERT(*tested.cbegin()==1);
		UNIT_TEST_ASSERT(*(tested.cend()-1)==amount);
		UNIT_TEST_ASSERT(*(tested.rend()-1)==1);
		UNIT_TEST_ASSERT(*(tested.rbegin())==amount);
		UNIT_TEST_ASSERT(*(tested.crend()-1)==1);
		UNIT_TEST_ASSERT(*(tested.crbegin())==amount);
		UNIT_TEST_ASSERT(tested.size()==amount);
		UNIT_TEST_ASSERT(!(tested.size())==tested.empty());
		tested.selfCheck();
		//
		ptrdiff_t should_distance=-1;
		for(TContainer::const_iterator current = tested.cbegin(), last = tested.cend();current!=last; ++current){
			UNIT_TEST_ASSERT(::std::distance(tested.cbegin(), current)==++should_distance);
			UNIT_TEST_ASSERT(::std::distance(tested.cbegin(), current)+1==*current);
		}
		for(should_distance=0;should_distance+1<(ptrdiff_t)amount;++should_distance){
			TContainer::const_iterator shiftedIterator = ::boost::next(tested.cbegin(), should_distance);
			ASSERT(shiftedIterator!=tested.cend());
			UNIT_TEST_ASSERT(*shiftedIterator==should_distance+1);
			UNIT_TEST_ASSERT(tested[should_distance]==should_distance+1);
			UNIT_TEST_ASSERT(::std::distance(tested.cbegin(), shiftedIterator)==should_distance);
		}
	}
	
	template<class TContainer1>
	void checkConstructableFromIncreasedRange(const int amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		::boost::integer_range<int> source = makeIncreasedRangeFromAmount(amount);
		const TContainer1 goingToTest(::boost::begin(source), ::boost::end(source));
		checkEquality(goingToTest,makeIncreasedContainerFromAmount(amount));
		checkIncreased(goingToTest, amount);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkAssignedContainer(const ::std::size_t amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		TContainer1 goingToTest;
		goingToTest.assign(amount);
		checkEquality(goingToTest, ::std::vector<int>(amount));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkUndefaultAssignedContainer(const ::std::size_t amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		TContainer1 goingToTest;
		goingToTest.assign(amount, 5);
		checkEquality(goingToTest, ::std::vector<int>(amount, 5));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkUndefaultAssignedContainerWithIntAmount(const int amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		TContainer1 goingToTest;
		goingToTest.assign(amount, 6);
		checkEquality(goingToTest, ::std::vector<int>(::std::size_t(amount), 6));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}

	template<class TContainer1>
	void checkAssignedContainerWithIntAmount(const int amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		TContainer1 goingToTest;
		goingToTest.assign(amount);
		checkEquality(goingToTest, ::std::vector<int>(::std::size_t(amount)));
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
	}
	
	template<class TContainer1>
	void checkAssignedIncreasedRange(const int amount){
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		::boost::integer_range<int> source = makeIncreasedRangeFromAmount(amount);
		TContainer1 goingToTest;
		goingToTest.assign(source.begin(), source.end());
		checkEquality(goingToTest, makeIncreasedContainerFromAmount(amount));
		checkIncreased(goingToTest, amount);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
		/*using namespace std;
		cout<<goingToTest.algorithmic_complexity()<<endl;*/
	}
	
	signed randFromZeroToN(const unsigned N){
		ASSERT(N<=RAND_MAX);
		if(N<1)
			return 0;
		return rand()%N;
	}
	
	typedef ::std::vector<signed> TInsertPositions;
	typedef ::boost::integer_range<int> TIntegerRange;
	typedef ::std::list<int> TGage;
	
	TInsertPositions makeInsertionPositions(::std::size_t insertionAmount){
		TInsertPositions result;
		::std::size_t i=0;
		while(i<insertionAmount)
			result.push_back(randFromZeroToN(i++));
		return result;
	}
	
	TInsertPositions makeDeletionPositions(::std::size_t amount){
		TInsertPositions result;
		::std::size_t i = amount;
		while(i>0)
			result.push_back(randFromZeroToN(--i));
		return result;
	}
	
	template<class TContainer1>
	void insertTest(TContainer1& testedContainer, TGage& gage, const TIntegerRange& source, const TInsertPositions& insert_positions, const int amount){
		TIntegerRange::const_iterator insertee = source.begin();
		for(TInsertPositions::const_iterator insertPosition = insert_positions.begin(), end_ = insert_positions.end();insertPosition!=end_;++insertPosition,++insertee){
			TGage::const_iterator gageInsertIterator = ::boost::next(gage.begin(), *insertPosition);
			TContainer1::const_iterator testedContainerInsertIterator = ::boost::next(testedContainer.begin(), *insertPosition);
			ASSERT(insertee!=source.end());
			gage.insert(gageInsertIterator, *insertee);
			TContainer1::const_iterator inserted = testedContainer.insert(testedContainerInsertIterator, *insertee);
			UNIT_TEST_ASSERT(*inserted==*insertee);
			UNIT_TEST_ASSERT(::std::distance(testedContainer.cbegin(), inserted)==*insertPosition);
			checkEquality(testedContainer, gage);
		}
		ASSERT(insertee==source.end());
		/*using namespace std;
		cout<<testedContainer.algorithmic_complexity()<<endl;*/
		ASSERT(source.size()==amount);
		ASSERT(insert_positions.size()==amount);
		UNIT_TEST_ASSERT(testedContainer.size()==amount);
	}

	template<class TContainer1>
	void eraseTest(TContainer1& testedContainer, TGage& gage, const TInsertPositions& erase_positions){
		ASSERT(erase_positions.size()==testedContainer.size());
		for(TInsertPositions::const_iterator erasePosition = erase_positions.begin(), end_ = erase_positions.end();erasePosition!=end_;++erasePosition){
			TGage::const_iterator gageEraseIterator = ::boost::next(gage.begin(), *erasePosition);
			const TContainer1::const_iterator
				testedContainerEraseIterator = ::boost::next(testedContainer.begin(), *erasePosition),
				testedContainerAfterEraseIterator_ = ::boost::next(testedContainer.begin(), *erasePosition+1),
				testedContainerAfterEraseIterator = ::boost::next(testedContainerEraseIterator);
			UNIT_TEST_ASSERT(testedContainerAfterEraseIterator_==testedContainerAfterEraseIterator);
			gage.erase(gageEraseIterator);
			TContainer1::const_iterator eraseResult = testedContainer.erase(testedContainerEraseIterator);
			UNIT_TEST_ASSERT(eraseResult==testedContainerAfterEraseIterator);
			UNIT_TEST_ASSERT(eraseResult==testedContainer.cend() || *eraseResult==*testedContainerAfterEraseIterator);
			checkEquality(testedContainer, gage);
		}
		UNIT_TEST_ASSERT(testedContainer.empty());
		UNIT_TEST_ASSERT(testedContainer.size()==0);
	}
	
	template<class TContainer1>
	void checkRandInsertionsAndDeletions(int amount){
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		TIntegerRange source = makeIncreasedRangeFromAmount(amount);
		TContainer1 testedContainer;
		TInsertPositions 
			insert_positions = makeInsertionPositions(amount),
			erase_positions = makeDeletionPositions(amount);
		TGage gage;
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		insertTest(testedContainer, gage, source, insert_positions, amount);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
		eraseTest(testedContainer, gage, erase_positions);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		insert_positions = makeInsertionPositions(amount);
		erase_positions = makeDeletionPositions(amount);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		insertTest(testedContainer, gage, source, insert_positions, amount);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(amount);
		eraseTest(testedContainer, gage, erase_positions);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
	}
	
	UNIT_TEST_ENTRY_POINT
	{
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
		using namespace container;
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			random_access_list<TValue> empty;
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			UNIT_TEST_ASSERT(empty.get_allocator() == empty.get_allocator());
			UNIT_TEST_ASSERT(!(empty.get_allocator() != empty.get_allocator()));
			checksEmpty(empty);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			random_access_list<int> empty;
			UNIT_TEST_ASSERT(::boost::hash_value(empty)==::boost::hash_value(empty));
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > empty;
			checksEmpty(empty);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al;
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al);
			checksEmpty(empty);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<void> al;
			random_access_list<int, my_allocator::apply<TValue> > empty(al);
			checksEmpty(empty);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue> empty, empty2;
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<int> empty, empty2;
			UNIT_TEST_ASSERT(::boost::hash_value(empty)==::boost::hash_value(empty2));
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > empty, empty2;
			UNIT_TEST_ASSERT(!(empty.get_allocator() == empty2.get_allocator()));
			UNIT_TEST_ASSERT(empty.get_allocator() != empty2.get_allocator());
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue> empty, empty2(empty);
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			::std::allocator<TValue> al;
			random_access_list<TValue> empty(al), empty2(empty);
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al;
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al), empty2(empty);
			UNIT_TEST_ASSERT(empty.get_allocator() == empty2.get_allocator());
			UNIT_TEST_ASSERT(!(empty.get_allocator() != empty2.get_allocator()));
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue> empty, empty2(::boost::move(empty));
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			::std::allocator<TValue> al;
			random_access_list<TValue> empty(al), empty2(::boost::move(empty));
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al;
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al), empty2(::boost::move(empty));
			UNIT_TEST_ASSERT(empty.get_allocator() == empty2.get_allocator());
			UNIT_TEST_ASSERT(!(empty.get_allocator() != empty2.get_allocator()));
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			::std::allocator<TValue> al, al2;
			random_access_list<TValue> empty(al), empty2(al2);
			UNIT_TEST_ASSERT(al == al2);
			UNIT_TEST_ASSERT(!(al != al2));
			UNIT_TEST_ASSERT(empty.get_allocator() == empty2.get_allocator());
			UNIT_TEST_ASSERT(!(empty.get_allocator() != empty2.get_allocator()));
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al, al2;
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al), empty2(al2);
			UNIT_TEST_ASSERT(!(al == al2));
			UNIT_TEST_ASSERT(al != al2);
			UNIT_TEST_ASSERT(!(empty.get_allocator() == empty2.get_allocator()));
			UNIT_TEST_ASSERT(empty.get_allocator() != empty2.get_allocator());
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al, al2(al);
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al), empty2(al2);
			UNIT_TEST_ASSERT(al == al2);
			UNIT_TEST_ASSERT(!(al != al2));
			UNIT_TEST_ASSERT(empty.get_allocator() == empty2.get_allocator());
			UNIT_TEST_ASSERT(!(empty.get_allocator() != empty2.get_allocator()));
			checkEmptyWithEqualAllocators(empty, empty2);
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al, al2;
			random_access_list<TValue, my_allocator::apply<TValue> > empty(al), empty2(al2);
			UNIT_TEST_ASSERT(!(al == al2));
			UNIT_TEST_ASSERT(al != al2);
			UNIT_TEST_ASSERT(al == empty.get_allocator());
			UNIT_TEST_ASSERT(al2 != empty.get_allocator());
			UNIT_TEST_ASSERT(al != empty2.get_allocator());
			UNIT_TEST_ASSERT(al2 == empty2.get_allocator());
			checksEmptyAndCheckEquality(empty, empty2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > c, c2(c), c3, c4(c3);
			checkEmptyWithEqualAllocators(c, c2);
			checkEmptyWithEqualAllocators(c4, c3);
			checkEmptyWithEqualAllocators(c3, c4);
			checksEmptyAndCheckEquality(c, c3);
			checksEmptyAndCheckEquality(c2, c4);
			checksEmptyAndCheckEquality(c2, c3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > c, c2(c), c3, c4(c3);
			UNIT_TEST_ASSERT(c.get_allocator()==c2.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()==c4.get_allocator());
			UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c.get_allocator());
			c.selfCheck();
			c2.selfCheck();
			c3.selfCheck();
			c4.selfCheck();
			::boost::swap(c,c3);
			UNIT_TEST_ASSERT(c.get_allocator()!=c2.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c.get_allocator()==c4.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()==c2.get_allocator());
			c.selfCheck();
			c3.selfCheck();
			::boost::swap(c2,c4);
			UNIT_TEST_ASSERT(c.get_allocator()==c2.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()==c4.get_allocator());
			UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c.get_allocator());
			c2.selfCheck();
			c4.selfCheck();
			c=c3;
			{
				const my_allocator::apply<TValue> oldCAllocator = c.get_allocator();
				UNIT_TEST_ASSERT(c.get_allocator()==oldCAllocator);
				UNIT_TEST_ASSERT(c.get_allocator()==c2.get_allocator());
				UNIT_TEST_ASSERT(c.get_allocator()!=c3.get_allocator());
				UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			}
			c.selfCheck();
			c3.selfCheck();
			c=::boost::move(c3);
			{
				const my_allocator::apply<TValue> oldCAllocator = c.get_allocator();
				UNIT_TEST_ASSERT(c.get_allocator()==oldCAllocator);
				UNIT_TEST_ASSERT(c.get_allocator()==c2.get_allocator());
				UNIT_TEST_ASSERT(c.get_allocator()!=c3.get_allocator());
				UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			}
			c.selfCheck();
			c3.selfCheck();
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > c, c2, c3, c4;
			const my_allocator::apply<TValue> oldCAllocator = c.get_allocator();
			c.selfCheck();
			c2.selfCheck();
			c3.selfCheck();
			c4.selfCheck();
			c=c2;
			c3=c4;
			UNIT_TEST_ASSERT(c.get_allocator()==oldCAllocator);
			UNIT_TEST_ASSERT(c.get_allocator()!=c2.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c.get_allocator());
			UNIT_TEST_ASSERT(c2.get_allocator()!=c4.get_allocator());
			c.selfCheck();
			c2.selfCheck();
			c3.selfCheck();
			c4.selfCheck();
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > c, c2(::boost::move(c)), c3, c4(::boost::move(c3));
			UNIT_TEST_ASSERT(c.get_allocator()==c2.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()==c4.get_allocator());
			UNIT_TEST_ASSERT(c.get_allocator()!=c4.get_allocator());
			UNIT_TEST_ASSERT(c3.get_allocator()!=c.get_allocator());
			c.selfCheck();
			c2.selfCheck();
			c3.selfCheck();
			c4.selfCheck();
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			::std::list<int> l;
			random_access_list<TValue> c(l.begin(), l.end());
			checksEmpty(c);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			::std::list<int> l;
			my_allocator::apply<TValue> al;
			random_access_list<TValue, my_allocator::apply<TValue> > c(l.begin(), l.end(), al), c2;
			checksEmptyAndCheckEquality(c, c2);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			my_allocator::apply<TValue> al;
			random_access_list<TValue, my_allocator::apply<TValue> >c(::std::size_t(0), int(), al);
			checksEmpty(c);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> >c(::std::size_t(0), int());
			checksEmpty(c);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			random_access_list<TValue, my_allocator::apply<TValue> > c(0);
			checksEmpty(c);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		{
			typedef random_access_list<int> TContainer;
			{
				TContainer c(0);
				checksEmpty(c);
				{
					TContainer::const_iterator t (c.begin());
					TContainer::const_iterator t1 (c.cbegin());
					TContainer::const_iterator t2 (c.end());
					TContainer::const_iterator t3 (c.cend());
					TContainer::iterator t_ (c.begin());
					TContainer::iterator t2_ (c.end());
					t = t;
					t = t_;
				}
				{
					TContainer::const_reverse_iterator t (c.rbegin());
					TContainer::const_reverse_iterator t1 (c.crbegin());
					TContainer::const_reverse_iterator t2 (c.rend());
					TContainer::const_reverse_iterator t3 (c.crend());
					TContainer::reverse_iterator t_ (c.rbegin());
					TContainer::reverse_iterator t2_ (c.rend());
					t = t;
					t = t_;
				}
			}
			{
				const TContainer c;
				{
					TContainer::const_iterator t = c.begin();
					TContainer::const_iterator t1 = c.cbegin();
					TContainer::const_iterator t2 = c.end();
					TContainer::const_iterator t3 = c.cend();
				}
				{
					TContainer::const_reverse_iterator t = c.rbegin();
					TContainer::const_reverse_iterator t1 = c.crbegin();
					TContainer::const_reverse_iterator t2 = c.rend();
					TContainer::const_reverse_iterator t3 = c.crend();
				}
			}
		}
		{
			typedef random_access_list<int> TContainer;
			::std::list<TContainer> a;
			a.push_back(TContainer());
			UNIT_TEST_ASSERT(::boost::range::find(a, TContainer())!=a.end());
			a.back().selfCheck();
		}
		{
			typedef random_access_list<int, my_allocator::apply<int> > TContainer;
			::std::list<TContainer> a;
			a.push_back(TContainer());
			UNIT_TEST_ASSERT(::boost::range::find(a, TContainer())!=a.end());
			a.back().selfCheck();
		}
		{
			typedef random_access_list<int, my_allocator::apply<int> > TContainer;
			::std::list<TContainer, my_allocator::apply<int> > a;
			a.push_back(TContainer());
			UNIT_TEST_ASSERT(::boost::range::find(a, TContainer())!=a.end());
			a.back().selfCheck();
		}
		{
			typedef random_access_list<int> TContainer;
			::boost::unordered_set<TContainer> a;
			a.insert(TContainer());
			UNIT_TEST_ASSERT(a.find(TContainer())!=a.end());
		}
		{
			typedef random_access_list<int, my_allocator::apply<int> > TContainer;
			::boost::unordered_set<TContainer> a;
			a.insert(TContainer());
			UNIT_TEST_ASSERT(a.find(TContainer())!=a.end());
		}
		{
			typedef random_access_list<int> TContainer;
			::stdext::hash_set<TContainer> a;
			a.insert(TContainer());
			UNIT_TEST_ASSERT(a.find(TContainer())!=a.end());
		}
		{
			typedef random_access_list<int, my_allocator::apply<int> > TContainer;
			::stdext::hash_set<TContainer> a;
			a.insert(TContainer());
			UNIT_TEST_ASSERT(a.find(TContainer())!=a.end());
		}
		{
			typedef random_access_list<int> TContainer;
			typedef TContainer::const_iterator TContainer_cit;
			typedef TContainer::iterator TContainer_it;
			typedef TContainer::const_reverse_iterator TContainer_crit;
			typedef TContainer::reverse_iterator TContainer_rit;
			TContainer c;
			::boost::unordered_set<TContainer_it> a;
			a.insert(c.end());
			UNIT_TEST_ASSERT(a.find(c.end())!=a.end());
			::boost::unordered_set<TContainer_cit> b;
			b.insert(c.end());
			UNIT_TEST_ASSERT(b.find(c.end())!=b.end());
			::boost::unordered_set<TContainer_rit> e;
			e.insert(c.rend());
			UNIT_TEST_ASSERT(e.find(c.rend())!=e.end());
			::boost::unordered_set<TContainer_crit> d;
			d.insert(c.rend());
			UNIT_TEST_ASSERT(d.find(c.rend())!=d.end());
		}
		{
			typedef random_access_list<int> TContainer;
			typedef TContainer::const_iterator TContainer_cit;
			typedef TContainer::iterator TContainer_it;
			typedef TContainer::const_reverse_iterator TContainer_crit;
			typedef TContainer::reverse_iterator TContainer_rit;
			TContainer c;
			::stdext::hash_set<TContainer_it> a;
			a.insert(c.end());
			UNIT_TEST_ASSERT(a.find(c.end())!=a.end());
			::stdext::hash_set<TContainer_cit> b;
			b.insert(c.end());
			UNIT_TEST_ASSERT(b.find(c.end())!=b.end());
			::stdext::hash_set<TContainer_rit> e;
			e.insert(c.rend());
			UNIT_TEST_ASSERT(e.find(c.rend())!=e.end());
			::stdext::hash_set<TContainer_crit> d;
			d.insert(c.rend());
			UNIT_TEST_ASSERT(d.find(c.rend())!=d.end());
		}
		//////////////////////////////////////////////////////////////////////////
		for(::std::size_t i = 0;i<=30;++i)
			checkDefaultConstructable<container::random_access_list<TValue> >(i);
		for(::std::size_t i = 0;i<=MAX_ITEMS_AMOUNT_WITH_ALLOCATOR;++i)
			checkDefaultConstructable<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkConstructable<container::random_access_list<TValue> >(i, 50);
		for(::std::size_t i = 0;i<=MAX_ITEMS_AMOUNT_WITH_ALLOCATOR;++i)
			checkConstructable<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i, 40);
		for(::std::size_t i = 0;i<=30;++i)
			checkConstructableFromIncreasedRange<container::random_access_list<TValue> >(i);
		for(::std::size_t i = 0;i<=6;++i)
			checkConstructableFromIncreasedRange<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		//////////////////////////////////////////////////////////////////////////
		for(::std::size_t i = 0;i<=30;++i)
			checkUndefaultAssignedContainer<container::random_access_list<TValue> >(i);
		for(::std::size_t i = 0;i<=MAX_ITEMS_AMOUNT_WITH_ALLOCATOR;++i)
			checkUndefaultAssignedContainer<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		for(::std::size_t i = 0;i<=30;++i)
			checkAssignedContainer<container::random_access_list<TValue> >(i);
		for(::std::size_t i = 0;i<=MAX_ITEMS_AMOUNT_WITH_ALLOCATOR;++i)
			checkAssignedContainer<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		for(::std::size_t i = 0;i<=30;++i)
			checkAssignedIncreasedRange<container::random_access_list<TValue> >(i);
		for(::std::size_t i = 0;i<=6;++i)
			checkAssignedIncreasedRange<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		//////////////////////////////////////////////////////////////////////////
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkDefaultConstructableWithIntAmount<container::random_access_list<TValue> >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkConstructableWithIntAmount<container::random_access_list<TValue> >(i,67);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkUndefaultAssignedContainerWithIntAmount<container::random_access_list<TValue> >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkAssignedContainerWithIntAmount<container::random_access_list<TValue> >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=30;++i)
			checkRandInsertionsAndDeletions<container::random_access_list<TValue> >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		for(::std::size_t i = 0;i<=6;++i)
			checkRandInsertionsAndDeletions<container::random_access_list<TValue, my_allocator::template apply<TValue> > >(i);
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			enum {i=1000};
			checkAssignedIncreasedRange<container::random_access_list<TValue, malloc_allocator<TValue> > >(i);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			checkRandInsertionsAndDeletions<container::random_access_list<TValue, malloc_allocator<TValue> > >(i);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			using namespace ::boost::assign;
			::std::list<int> a=list_of<int> (1)(1)(3);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			container::random_access_list<TValue> b(a.begin(), a.end());
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.unique();
			b.unique();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			using namespace ::boost::assign;
			::std::list<int> a=list_of<int> (1)(3)(5)(1);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			container::random_access_list<TValue> b(a.begin(), a.end());
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			b.selfCheck();
			a.sort();
			b.sort();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.unique();
			b.unique();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			using namespace ::boost::assign;
			::std::list<int> a=list_of<int> (1)(3)(4)(5)(1)(5)(7);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			container::random_access_list<TValue> b(a.begin(), a.end());
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			b.selfCheck();
			a.sort();
			b.sort();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.unique();
			b.unique();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			using namespace ::boost::assign;
			::std::list<int> a=list_of<int> (1)(3)(4)(5)(1)(5)(7)(8)(8)(9)(3)(100)(100)(123);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			container::random_access_list<TValue> b(a.begin(), a.end());
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			b.selfCheck();
			a.sort();
			b.sort();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.unique();
			b.unique();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
		}
		INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			using namespace ::boost::assign;
			::std::list<int> a=list_of<int> (1)(3)(4)(5)(1)(5)(7)(8)(8)(9)(3)(100)(100)(123)(1100)(1170)(1160)(1150)(1140)(1300)(1102)(1120)(1200)(3100)(11100);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			container::random_access_list<TValue> b(a.begin(), a.end());
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			b.selfCheck();
			a.sort();
			b.sort();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.unique();
			b.unique();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
			a.reverse();
			b.reverse();
			checkEquality(b,a);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(a.size());
		}
		{
			const NUnitTest::NMock::TCheckLostObjects<> CheckLostObjects;
			typedef container::random_access_list<TValue> TContainer;
			typedef TContainer::const_iterator TContainerIterator;
			TContainer a;
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(0);
			{
				TContainerIterator b = a.emplace_back(4);
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==4);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==1);
			UNIT_TEST_ASSERT(a[0]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(1);
			{
				TContainerIterator b = a.emplace_front(3);
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==3);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==2);
			UNIT_TEST_ASSERT(a[0]==3);
			UNIT_TEST_ASSERT(a[1]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(2);
			{
				TContainerIterator b = a.emplace(a.cbegin(), 2);
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==2);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==3);
			UNIT_TEST_ASSERT(a[0]==2);
			UNIT_TEST_ASSERT(a[1]==3);
			UNIT_TEST_ASSERT(a[2]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(3);
			{
				TContainerIterator b = a.emplace_front();
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==0);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==4);
			UNIT_TEST_ASSERT(a[0]==0);
			UNIT_TEST_ASSERT(a[1]==2);
			UNIT_TEST_ASSERT(a[2]==3);
			UNIT_TEST_ASSERT(a[3]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(4);
			{
				TContainerIterator b = a.emplace(a.cbegin());
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==0);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==5);
			UNIT_TEST_ASSERT(a[0]==0);
			UNIT_TEST_ASSERT(a[1]==0);
			UNIT_TEST_ASSERT(a[2]==2);
			UNIT_TEST_ASSERT(a[3]==3);
			UNIT_TEST_ASSERT(a[4]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(5);
			{
				TContainerIterator b = a.emplace(a.begin());
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==0);
				UNIT_TEST_ASSERT(*b==0);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==6);
			UNIT_TEST_ASSERT(a[0]==0);
			UNIT_TEST_ASSERT(a[1]==0);
			UNIT_TEST_ASSERT(a[2]==0);
			UNIT_TEST_ASSERT(a[3]==2);
			UNIT_TEST_ASSERT(a[4]==3);
			UNIT_TEST_ASSERT(a[5]==4);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(6);
			{
				TContainerIterator b = a.emplace_back();
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==6);
				UNIT_TEST_ASSERT(*b==0);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==7);
			UNIT_TEST_ASSERT(a[0]==0);
			UNIT_TEST_ASSERT(a[1]==0);
			UNIT_TEST_ASSERT(a[2]==0);
			UNIT_TEST_ASSERT(a[3]==2);
			UNIT_TEST_ASSERT(a[4]==3);
			UNIT_TEST_ASSERT(a[5]==4);
			UNIT_TEST_ASSERT(a[6]==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(7);
			{
				TContainerIterator b = a.emplace(a.cend());
				UNIT_TEST_ASSERT(::std::distance(a.cbegin(), b)==7);
				UNIT_TEST_ASSERT(::std::distance(b, a.cbegin())==-7);
				UNIT_TEST_ASSERT(*b==0);
			}
			a.selfCheck();
			UNIT_TEST_ASSERT(a.size()==8);
			UNIT_TEST_ASSERT(a[0]==0);
			UNIT_TEST_ASSERT(a[1]==0);
			UNIT_TEST_ASSERT(a[2]==0);
			UNIT_TEST_ASSERT(a[3]==2);
			UNIT_TEST_ASSERT(a[4]==3);
			UNIT_TEST_ASSERT(a[5]==4);
			UNIT_TEST_ASSERT(a[6]==0);
			UNIT_TEST_ASSERT(a[7]==0);
			INSTANCE_AMOUNT_MUST_EQUAL_TO(8);
		}
	}
UNIT_TEST_MODULE_END