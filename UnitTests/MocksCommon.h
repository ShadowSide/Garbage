#ifndef MocksCommon_h__
#define MocksCommon_h__

#include <Common\accessory\accessory.h>
#include <boost\function.hpp>
#include <Common\ForBoost\boost__use_default.h>
#include <boost/unordered_set.hpp>
//#include <boost/move/core.hpp>

namespace NUnitTest
{
	namespace NMock
	{
		namespace NMockInternal
		{
			class TMockBase
			{
			private:
				//BOOST_COPYABLE_AND_MOVABLE(TMockBase)

				enum
				{
					SIGNATURE_CONSTRUCTED_OBJECT=0xC0ED0BEC,
					SIGNATURE_DESTROED_OBJECT=0xDE0ED0BE
				};

				int _signature;
				static bool _enabledObjectIntegrityCheck;
				typedef TMockBase TMe;
				typedef TMe const* PMe;

			public:
				typedef boost::unordered_set<PMe> TObjectsTracker;

			private:
				static TObjectsTracker _objectsTracker;
				static bool _ignoreCreating;

				void unbusyObjectPlaceCheck(void)
				{
					if(_ignoreCreating)
						return;
					const bool isFreeObjectPlace=_objectsTracker.find(this)==_objectsTracker.end();
					ASSERT(isFreeObjectPlace);
					if(isFreeObjectPlace)
						_objectsTracker.insert(this);
				}

			protected:
				TMockBase(void):
					_signature(SIGNATURE_CONSTRUCTED_OBJECT)
				{
					this->unbusyObjectPlaceCheck();
				}

				TMockBase(const TMockBase&):
					_signature(SIGNATURE_CONSTRUCTED_OBJECT)
				{
					this->unbusyObjectPlaceCheck();
				}

				/*TMockBase(BOOST_RV_REF(TMockBase)):
					_signature(SIGNATURE_CONSTRUCTED_OBJECT)
				{
					this->unbusyObjectPlaceCheck();
				}*/

				~TMockBase(void)
				{
					this->ObjectIntegrityCheck();
					this->_signature=SIGNATURE_DESTROED_OBJECT;
					const TObjectsTracker::iterator finded=_objectsTracker.find(this);
					const bool isObjectExist=finded!=_objectsTracker.end();
					ASSERT(_ignoreCreating||isObjectExist);
					if (isObjectExist)
						_objectsTracker.erase(finded);
				}

			public:
				static bool createdIsEmpty(void)
				{
					return _objectsTracker.empty();
				}


				static size_t createdAmount(void)
				{
					return _objectsTracker.size();
				}

				static void resetCreated(void)
				{
					return _objectsTracker.clear();
				}

				static const TObjectsTracker& objectsTracker(void)
				{
					return _objectsTracker;
				}

				static bool objectIntegrityCheckMode(void)
				{
					return _enabledObjectIntegrityCheck;				
				}

				static bool objectIntegrityCheckMode(const bool Enable)
				{
					const bool OldMode=_enabledObjectIntegrityCheck;
					_enabledObjectIntegrityCheck=Enable;
					return OldMode;
				}

				static bool ignoreCreating(const bool Enable)
				{
					const bool OldMode=_ignoreCreating;
					_ignoreCreating=Enable;
					return OldMode;
				}

				static bool ignoreCreating(void)
				{
					return _ignoreCreating;
				}				

				void OfflessObjectIntegrityCheck(void) const
				{
					ASSERT(this!=NULL);
					ASSERT(this->_signature!=SIGNATURE_DESTROED_OBJECT);
					ASSERT(this->_signature==SIGNATURE_CONSTRUCTED_OBJECT);
					ASSERT(_ignoreCreating||(_objectsTracker.find(this)!=_objectsTracker.end()));
				}

				void ObjectIntegrityCheck(void) const
				{
					if(_enabledObjectIntegrityCheck)
						this->OfflessObjectIntegrityCheck();
				}
			};
		}
		
		bool objectIntegrityCheckMode(const bool Enable);
		bool ignoreCreating(const bool Enable);
		bool createdIsEmpty(void);
		size_t createdAmount(void);
		void resetCreated(void);

		template<bool ExactlyCheck=true/*Slow and memory eat*/>
		class TCheckLostObjects
		{
		private:
			const size_t _Created;
			typedef NMockInternal::TMockBase TControll;
			const TControll::TObjectsTracker _SavedObjectsTracker;			

			TCheckLostObjects(const TCheckLostObjects&);
			TCheckLostObjects& operator=(const TCheckLostObjects&);

			static const TControll::TObjectsTracker& EmptyObjectsTracker(void)
			{
				static const TControll::TObjectsTracker ObjectsTracker;
				return ObjectsTracker;
			}

		public:
			TCheckLostObjects(void):
				_Created(createdAmount()),
				_SavedObjectsTracker(
					(ExactlyCheck?
						&TControll::objectsTracker
					:
						&TCheckLostObjects::EmptyObjectsTracker
					)())
			{}

			~TCheckLostObjects(void)
			{
				ASSERT(_Created==createdAmount());
				ASSERT((!ExactlyCheck)||TControll::objectsTracker()==_SavedObjectsTracker);//Slow and memory eat.
			}
		};

		namespace NBehavior
		{
			typedef boost::function<void(void)> TExceptionThrowerFunction;
			extern TExceptionThrowerFunction MockTestSomeExceptionThrower;

			class TMockTestSomeException: public std::exception{};

			namespace NMockBehaviorInternal
			{
				void MockTestSomeExceptionThrowerFunction(void);

				template<size_t defaultAmountOfConstructWithoutThrow, TExceptionThrowerFunction& exceptionThrowerFunction=MockTestSomeExceptionThrower, class tag=boost::use_default>
				struct TThrowAfterCountAmount
				{
					static size_t amountOfConstructWithoutThrow;
					static size_t amountOfConstructedWithoutThrow;
					static bool enabledThrowCount;
					static void throwCountMode(const bool Enable)
					{
						if(Enable){
							amountOfConstructedWithoutThrow=0;
							enabledThrowCount=true;
						}else{
							enabledThrowCount=false;
						}
					}

					static void throwAfterCountAmount(void)
					{
						if(enabledThrowCount)
						{
							++amountOfConstructedWithoutThrow;
							if(amountOfConstructedWithoutThrow>amountOfConstructWithoutThrow)
							{
								amountOfConstructedWithoutThrow=0;
								exceptionThrowerFunction();
							}							
						}
					}
				};
			}

			namespace NAspects
			{
 				struct TNonThrowAtConstrcutorFromValueBehaviorAspect
 				{
					template<class T, class U>
					void throwAtConstrcutorFromValue(const T& fromValue, U& toMock)
					{
						(void)fromValue;(void)toMock;
					}
 				};
 				
				template<size_t defaultAmountOfConstructWithoutThrow, TExceptionThrowerFunction& exceptionThrowerFunction=MockTestSomeExceptionThrower, class tag=boost::use_default>
				struct TThrowAtConstructObjectBehaviorAspect: NMockBehaviorInternal::TThrowAfterCountAmount<defaultAmountOfConstructWithoutThrow, exceptionThrowerFunction, tag>
				{
					typedef TThrowAfterCountAmount me;
					
					template<class T, class U>
					void throwAtConstrcutorFromValue(const T& fromValue, U& toMock)
					{
						me::throwAfterCountAmount();
					}
				};
			}

			struct TCommontDefaultBehavior: 
				NAspects::TNonThrowAtConstrcutorFromValueBehaviorAspect
				//»спользуй множественное наследование от аспектов. ј ещЄ "¬сегда ставь пустой или полный стакан"
				//		дл€ каждого аспекта, если он использует методы в качестве интерфейса дл€ изменени€ поведени€,
				//		а не конструктор\деструктор.
				//јспекты можно и нужно комбинировать через множественное наследование.
			{};

			struct TDefaultStatisticaMockBehavior: TCommontDefaultBehavior
			{};

			struct TDefaultStatisticaMockWithValueBehavior: TCommontDefaultBehavior
			{};
		}

		struct TStatistica
		{
			UINT64 instanceAmount;

			void constructObject(void)
			{
				++instanceAmount;
			}

			void destructObject(void)
			{
				ASSERT(instanceAmount>0);
				--instanceAmount;
			}
		};		
	}
}

#define UNIT_TEST_DEFINE_GLOBAL_STATISTICA(Name)\
	namespace									\
	{											\
		NUnitTest::NMock::TStatistica Name;		\
	}

template<size_t defaultAmountOfConstructWithoutThrow, NUnitTest::NMock::NBehavior::TExceptionThrowerFunction& exceptionThrowerFunction, class tag>
size_t NUnitTest::NMock::NBehavior::NMockBehaviorInternal::TThrowAfterCountAmount<defaultAmountOfConstructWithoutThrow, exceptionThrowerFunction, tag>::amountOfConstructedWithoutThrow=0;

template<size_t defaultAmountOfConstructWithoutThrow, NUnitTest::NMock::NBehavior::TExceptionThrowerFunction& exceptionThrowerFunction, class tag>
size_t NUnitTest::NMock::NBehavior::NMockBehaviorInternal::TThrowAfterCountAmount<defaultAmountOfConstructWithoutThrow, exceptionThrowerFunction, tag>::amountOfConstructWithoutThrow=defaultAmountOfConstructWithoutThrow;

template<size_t defaultAmountOfConstructWithoutThrow, NUnitTest::NMock::NBehavior::TExceptionThrowerFunction& exceptionThrowerFunction, class tag>
bool NUnitTest::NMock::NBehavior::NMockBehaviorInternal::TThrowAfterCountAmount<defaultAmountOfConstructWithoutThrow, exceptionThrowerFunction, tag>::enabledThrowCount=false;

#endif // MocksCommon_h__
