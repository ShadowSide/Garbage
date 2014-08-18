#ifndef __StatisticaMock_h__
#define __StatisticaMock_h__

#include <Common\UnitTests\Mocks\MocksCommon.h>
#include <boost/move/core.hpp>
#include <boost/mpl/identity.hpp>

//Объект для сбора различной статистики в различных поведенческих условиях. Используется для тестирования коллекций и очередей и прочих шаблонных объектов.
//Не тестировал корректность и даже компилябильность, тк некогда. Протестирую по мере использования.
//Скорее всего не хватает всякого рода операторов сравнения, хеширования (std::hash) и тд. По мере использования добавлю.
//Моки однопоточны.

COMMENT((
		UNIT_TEST_DEFINE_GLOBAL_STATISTICA(statistica)
		//...
		//typedef int TValue;
		typedef /*typename (если нужно)*/ TStatisticaMock<statistica>::type TValue;
		std::my_container<TValue> mc;
		mc.push_back(TValue());
		std::cout<<statistica.instanceAmount<<std::endl;
		))

namespace NUnitTest
{
	namespace NMock
	{
		namespace NMockInternal
		{
			template<class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockTemplate: TMockBase
			{
				TBehavior _behavior;

				TStatisticaMockTemplate(void)
					:_behavior()
				{
					statistica.constructObject();
				}

				TStatisticaMockTemplate(const TStatisticaMockTemplate & other)
					:_behavior(other._behavior)
				{
					statistica.constructObject();
				}

				TStatisticaMockTemplate(BOOST_RV_REF(TStatisticaMockTemplate) other)
					:_behavior(boost::move(other._behavior))
				{
					statistica.constructObject();
				}

				TStatisticaMockTemplate & operator=(BOOST_COPY_ASSIGN_REF(TStatisticaMockTemplate) other)
				{
					this->ObjectIntegrityCheck();
					_behavior=other._behavior;
					return *this;
				}

				TStatisticaMockTemplate & operator=(BOOST_RV_REF(TStatisticaMockTemplate) other)
				{
					this->ObjectIntegrityCheck();
					_behavior=boost::move(other._behavior);
					return *this;
				}

				~TStatisticaMockTemplate(void)
				{
					statistica.destructObject();
				}

				BOOST_COPYABLE_AND_MOVABLE(TStatisticaMockTemplate)
			};
		}
		template<class TStatisticaClass, TStatisticaClass& statistica, class TBehavior=NBehavior::TDefaultStatisticaMockBehavior>
		struct TStatisticaMockPowerful: boost::mpl::identity<NMockInternal::TStatisticaMockTemplate<TStatisticaClass, statistica, TBehavior> >{};

		template<TStatistica& statistica, class TBehavior=NBehavior::TDefaultStatisticaMockBehavior>
		struct TStatisticaMock: TStatisticaMockPowerful<TStatistica, statistica, TBehavior>{};
	}
}
#endif // __StatisticaMock_h__
