#ifndef __StatisticaMockWithValue_h__
#define __StatisticaMockWithValue_h__

#include <Common\UnitTests\Mocks\MocksCommon.h>
#include <boost/move/core.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <utility>
#include <boost/utility.hpp>

//Объект для сбора различной статистики и изменения поведения класса. Используется для тестирования коллекций и очередей и прочих шаблонных объектов.
//Позволяет хранить классы и не классы, а также ссылки на них.
//Объект старается подстроится под свой опекаемый тип.
//Не тестировал корректность и даже компилябильность, тк некогда. Протестирую по мере использования.
//Возможно не хватает некоторых операторов сравнения и тд. По мере использования добавлю, если нужно. Но скорее всего для TCountedValue в этом плане все хорошо.
//Моки однопоточны.

COMMENT((
		UNIT_TEST_DEFINE_GLOBAL_STATISTICA(statistica)
		//...
		//typedef int TValue;
		typedef /*typename (если нужно)*/ TCountedValue<int, statistica>::type TValue;
		std::my_container<TValue> mc;
		mc.push_back(5);
		std::cout<<statistica.instanceAmount<<std::endl;
))

namespace NUnitTest
{
	namespace NMock
	{
		namespace NMockInternal
		{
			template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockWithValueForSimpleType: TMockBase
			{
				TValue _value;
				TBehavior _behavior;

				TValue& get(void)
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				const TValue& get(void) const
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				/*template<class T>
				const T get(void) const
				{
					return T(_value);
				}*/

				operator TValue&(void)
				{
					return this->get();
				}

				operator const TValue&(void) const
				{
					return this->get();
				}

				/*template<class T>
				operator T(void) const
				{
					return this->get();
				}*/

				size_t hash_value(void) const{
					return ::boost::hash_value(this->get());
				}

				TStatisticaMockWithValueForSimpleType(void)
					:_value()
					,_behavior()
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleType(const TValue & other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				template<class T>
				TStatisticaMockWithValueForSimpleType(const T & other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleType(const TStatisticaMockWithValueForSimpleType& other)
					:_value(other.get())
					,_behavior(other._behavior)
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleType & operator=(const TStatisticaMockWithValueForSimpleType& other) {
					this->ObjectIntegrityCheck();
					_value=other.get();
					_behavior=other._behavior;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForSimpleType & operator=(const TValue& other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				template<class T>
				TStatisticaMockWithValueForSimpleType & operator=(const T& other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				~TStatisticaMockWithValueForSimpleType(void) {
					statistica.destructObject();
				}

				friend void swap(TStatisticaMockWithValueForSimpleType& l, TStatisticaMockWithValueForSimpleType& r){
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
					::boost::swap(l->_value, r->_value);
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
				}
			};

			template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockWithValueForSimpleTypeReference: TMockBase
			{
				TValue _value;
				TBehavior _behavior;

				TValue get(void)
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				const TValue get(void) const
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				/*template<class T>
				const T get(void) const
				{
					return T(_value);
				}*/

				operator TValue(void)
				{
					return this->get();
				}

				operator const TValue(void) const
				{
					return this->get();
				}

				/*template<class T>
				operator T(void) const
				{
					return this->get();
				}*/

				size_t hash_value(void) const{
					return ::boost::hash_value(this->get());
				}

				TStatisticaMockWithValueForSimpleTypeReference(void)
					:_value()
					,_behavior()
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleTypeReference(const TValue other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				template<class T>
				TStatisticaMockWithValueForSimpleTypeReference(const T other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleTypeReference(const TStatisticaMockWithValueForSimpleTypeReference& other)
					:_value(other.get())
					,_behavior(other._behavior)
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForSimpleTypeReference & operator=(const TStatisticaMockWithValueForSimpleTypeReference& other) {
					this->ObjectIntegrityCheck();
					_value=other.get();
					_behavior=other._behavior;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForSimpleTypeReference & operator=(const TValue other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				template<class T>
				TStatisticaMockWithValueForSimpleTypeReference & operator=(const T& other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				~TStatisticaMockWithValueForSimpleTypeReference(void) {
					statistica.destructObject();
				}

				friend void swap(TStatisticaMockWithValueForSimpleTypeReference& l, TStatisticaMockWithValueForSimpleTypeReference& r){
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
					::boost::swap(l->_value, r->_value);
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
				}
			};

			template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockWithValueForClass: TMockBase
			{
				TValue _value;
				TBehavior _behavior;

				TValue& get(void)
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				const TValue& get(void) const
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				/*template<class T>
				const T get(void) const
				{
					return T(_value);
				}*/

				operator TValue&(void)
				{
					return this->get();
				}

				operator const TValue&(void) const
				{
					return this->get();
				}

				/*template<class T>
				operator T(void) const
				{
					return this->get();
				}*/

				size_t hash_value(void) const{
					return ::boost::hash_value(this->get());
				}

				TStatisticaMockWithValueForClass(void)
					:_value()
					,_behavior()
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClass(const TValue& other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				template<class T>
				TStatisticaMockWithValueForClass(const T& other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClass(BOOST_RV_REF(TValue) other)
					:_value(boost::move(other))
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				template<class T>
				TStatisticaMockWithValueForClass(BOOST_RV_REF(T) other)
					:_value(boost::move(other))
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClass(const TStatisticaMockWithValueForClass& other)
					:_value(other.get())
					,_behavior(other._behavior)
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClass(BOOST_RV_REF(TStatisticaMockWithValueForClass) other)
					:_value(boost::move(other.get()))
					,_behavior(boost::move(other._behavior))
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClass & operator=(BOOST_COPY_ASSIGN_REF(TStatisticaMockWithValueForClass) other) {
					this->ObjectIntegrityCheck();
					_value=other.get();
					_behavior=other._behavior;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClass & operator=(BOOST_RV_REF(TStatisticaMockWithValueForClass) other) {
					this->ObjectIntegrityCheck();
					_value=boost::move(other.get());
					_behavior=boost::move(other._behavior);
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClass & operator=(BOOST_COPY_ASSIGN_REF(TValue) other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClass & operator=(BOOST_RV_REF(TValue) other) {
					this->ObjectIntegrityCheck();
					_value=boost::move(other);
					this->ObjectIntegrityCheck();
					return *this;
				}

				template<class T>
				TStatisticaMockWithValueForClass & operator=(BOOST_COPY_ASSIGN_REF(T) other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				template<class T>
				TStatisticaMockWithValueForClass & operator=(BOOST_RV_REF(T) other) {
					this->ObjectIntegrityCheck();
					_value=boost::move(other);
					this->ObjectIntegrityCheck();
					return *this;
				}

				~TStatisticaMockWithValueForClass(void) {
					statistica.destructObject();
				}

				friend void swap(TStatisticaMockWithValueForClass& l, TStatisticaMockWithValueForClass& r){
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
					::boost::swap(l->_value, r->_value);
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
				}

				BOOST_COPYABLE_AND_MOVABLE(TStatisticaMockWithValueForClass)
			};

			template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockWithValueForClassReference: TMockBase
			{
				TValue _value;
				TBehavior _behavior;

				TValue get(void)
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				const TValue get(void) const
				{
					this->ObjectIntegrityCheck();
					return _value;
				}

				/*template<class T>
				const T get(void) const
				{
					return T(_value);
				}*/

				operator TValue(void)
				{
					return this->get();
				}

				operator const TValue(void) const
				{
					return this->get();
				}

				size_t hash_value(void) const{
					return ::boost::hash_value(this->get());
				}

				/*template<class T>
				operator T(void) const
				{
					return this->get();
				}*/

			private:
				TStatisticaMockWithValueForClassReference(void);

			public:
				TStatisticaMockWithValueForClassReference(const TValue other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				template<class T>
				TStatisticaMockWithValueForClassReference(const T& other)
					:_value(other)
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

			private:
				typedef typename boost::remove_reference<TValue>::type TUnReferencedValue;
				typedef typename boost::remove_cv<TUnReferencedValue>::type TUnReferencedUnConstValue;

				TStatisticaMockWithValueForClassReference(BOOST_RV_REF(TUnReferencedUnConstValue) other)
					:_value(boost::move(other))
					,_behavior()
				{
					_behavior.throwAtConstrcutorFromValue(other, *this);
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClassReference(const TStatisticaMockWithValueForClassReference& other)
					:_value(other.get())
					,_behavior(other._behavior)
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClassReference(BOOST_RV_REF(TStatisticaMockWithValueForClassReference) other)
					:_value(boost::move(other.get()))
					,_behavior(boost::move(other._behavior))
				{
					statistica.constructObject();
				}

				TStatisticaMockWithValueForClassReference & operator=(BOOST_COPY_ASSIGN_REF(TStatisticaMockWithValueForClassReference) other) {
					this->ObjectIntegrityCheck();
					_value=other.get();
					_behavior=other._behavior;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClassReference & operator=(BOOST_RV_REF(TStatisticaMockWithValueForClassReference) other) {
					this->ObjectIntegrityCheck();
					_value=boost::move(other.get());
					_behavior=boost::move(other._behavior);
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClassReference & operator=(BOOST_COPY_ASSIGN_REF(TUnReferencedUnConstValue) other) {
					this->ObjectIntegrityCheck();
					_value=other;
					this->ObjectIntegrityCheck();
					return *this;
				}

				TStatisticaMockWithValueForClassReference & operator=(BOOST_RV_REF(TUnReferencedUnConstValue) other) {
					this->ObjectIntegrityCheck();
					_value=boost::move(other);
					this->ObjectIntegrityCheck();
					return *this;
				}

				~TStatisticaMockWithValueForClassReference(void) {
					statistica.destructObject();
				}

				friend void swap(TStatisticaMockWithValueForClassReference& l, TStatisticaMockWithValueForClassReference& r){
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
					::boost::swap(l->_value, r->_value);
					l->ObjectIntegrityCheck();
					r->ObjectIntegrityCheck();
				}

				BOOST_COPYABLE_AND_MOVABLE(TStatisticaMockWithValueForClassReference)
			};

			template<class T, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
			struct TStatisticaMockWithValueChoicer:
				boost::mpl::eval_if<boost::is_reference<T>,
					boost::mpl::if_<boost::is_class<T>,
						TStatisticaMockWithValueForClassReference<T, TStatisticaClass, statistica, TBehavior>
					, 
						TStatisticaMockWithValueForSimpleTypeReference<T, TStatisticaClass, statistica, TBehavior>
					>
				,
					boost::mpl::if_<boost::is_class<T>,
						TStatisticaMockWithValueForClass<T, TStatisticaClass, statistica, TBehavior>
					, 
						TStatisticaMockWithValueForSimpleType<T, TStatisticaClass, statistica, TBehavior>
					>
				>
			{};
		}

		template<class T, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior=NBehavior::TDefaultStatisticaMockWithValueBehavior>
		struct TStatisticaMockWithValuePowerful: NMockInternal::TStatisticaMockWithValueChoicer<T, TStatisticaClass, statistica, TBehavior>{};

		template<class T, TStatistica& statistica, class TBehavior=NBehavior::TDefaultStatisticaMockWithValueBehavior>
		struct TStatisticaMockWithValue: TStatisticaMockWithValuePowerful<T, TStatistica, statistica, TBehavior>{};
	}
}

namespace std {
	namespace tr1 {
		template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
		struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior>, size_t >{
			::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
				return x.hash_value();
			}
		};
		template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
		struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior> , size_t >{
			::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior>  &x ) const{
				return x.hash_value();
			}
		};
		template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
		struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> , size_t >{
			::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
				return x.base().hash_value();
			}
		};
		template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
		struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior>  , size_t >{
			::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
				return x.base().hash_value();
			}
		};
	}
	/*	
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior>, size_t >{
		::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
			return x.hash_value();
		}
	};
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior> , size_t >{
		::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior>  &x ) const{
			return x.hash_value();
		}
	};
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior > , size_t >{
		::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
			return x.base().hash_value();
		}
	};
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	struct hash<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior> >: ::std::unary_function<::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior>  , size_t >{
		::std::size_t operator()(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior> &x ) const{
			return x.base().hash_value();
		}
	};
	*/
}

namespace boost{
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	::std::size_t hash_value(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClassReference<TValue, TStatisticaClass, statistica, TBehavior> & b){
		return b.hash_value();
	}
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	::std::size_t hash_value(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleTypeReference<TValue, TStatisticaClass, statistica, TBehavior> & b){
		return b.hash_value();
	}
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	::std::size_t hash_value(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForClass<TValue, TStatisticaClass, statistica, TBehavior> & b){
		return b.base().hash_value();
	}
	template<class TValue, class TStatisticaClass, TStatisticaClass& statistica, class TBehavior>
	::std::size_t hash_value(const ::NUnitTest::NMock::NMockInternal::TStatisticaMockWithValueForSimpleType<TValue, TStatisticaClass, statistica, TBehavior> & b){
		return b.base().hash_value();
	}
};

#endif // __StatisticaMockWithValue_h__
