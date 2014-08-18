#ifndef range_h__
#define range_h__

#include <Common\accessory\accessory.h>
#include <Common\ForBoost\boost__use_default.h>
//#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
//#include <boost/move/move.hpp>
//#include <Common\ForBoost\unique_ptr_extension.h>
#include <boost/range.hpp>
#include <boost/range/any_range.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <stddef.h>
//#include <boost/cast.hpp>
//#include <boost/iterator/iterator_facade.hpp>

//any_range_type_default_generator_ и any_iterator_type_default_generator_ предназаначены для автоматического заполнения параметров шаблона boost::any_range и boost::***::any_iterator.

/*Допустимые значения для параметра шаблона Traversal: 
 *::boost::random_access_traversal_tag 
 ::boost::bidirectional_traversal_tag
 ::boost::forward_traversal_tag
 ::boost::single_pass_traversal_tag
 ::boost::incrementable_traversal_tag
 ::boost::no_traversal_tag
 */
template<
	class Value
	, class Traversal = ::boost::random_access_traversal_tag //::boost::use_default эквивалентно random_access_traversal_tag
	, class Reference = typename ::boost::add_reference<Value>::type //::boost::use_default эквивалентно Value&
	, class Difference = std::ptrdiff_t
	, class Buffer = ::boost::use_default>
struct any_range_type_default_generator_
{
	typedef boost::any_range<
		Value,
		typename TBoostDefaultTypeHelper<Traversal, ::boost::random_access_traversal_tag>::type,
		typename TBoostDefaultTypeHelperWithTDefaultedResultFunctor<Reference, ::boost::add_reference<Value> >::type,
		Difference,
		Buffer> type;
};

template<
	class Value
	, class Traversal = ::boost::random_access_traversal_tag //::boost::use_default эквивалентно random_access_traversal_tag
	, class Reference = typename ::boost::add_reference<Value>::type //::boost::use_default эквивалентно Value&
	, class Difference = std::ptrdiff_t
	, class Buffer = ::boost::any_iterator_default_buffer>
struct any_iterator_type_default_generator_
{
	typedef boost::range_detail::any_iterator<
		Value,
		typename TBoostDefaultTypeHelper<Traversal, ::boost::random_access_traversal_tag>::type,
		typename TBoostDefaultTypeHelperWithTDefaultedResultFunctor<Reference, ::boost::add_reference<Value> >::type,
		Difference,
		Buffer> type;
};

//В any_range_type_default_generator_ и any_iterator_type_default_generator_ указано дефолтное значение для параметра шаблона Traversal, но лучше бы его не забывать указывать, тк зачастую этот параметр не соответсвует нужному типу итератора.
 
  
   
    
	 
	 
/*Мусорный неадекватный код. Не используется. Удали.

template<class TItem>
class TPolymorphicMutableRandomAccessIterator: public boost::iterator_facade<TPolymorphicMutableRandomAccessIterator<TItem>, TItem, boost::random_access_traversal_tag>
{
	BOOST_COPYABLE_AND_MOVABLE(TPolymorphicMutableRandomAccessIterator);

private:
	typedef boost::iterator_facade<TPolymorphicMutableRandomAccessIterator<TItem>, TItem, boost::random_access_traversal_tag> TBaseWithRealization;
	typedef TPolymorphicMutableRandomAccessIterator<TItem> TMe;
	friend class boost::iterator_core_access; //TBaseWithRealization;
	class ITypeErasure;
	typedef typename unique_ptr_type_generator_<ITypeErasure>::type PIterator;
	
	struct ITypeErasure
	{
	protected:
		typedef TPolymorphicMutableRandomAccessIterator TEnvelop;		

	public:
		virtual void increment(void) = 0;
		virtual void decrement(void) = 0;
		virtual bool equal(const PIterator& other) const = 0;
		virtual typename TEnvelop::reference dereference(void) const = 0;
		virtual void advance(const typename TMe::difference_type difference) = 0;
		virtual typename TEnvelop::difference_type distance_to(const PIterator& other) = 0;

		virtual PIterator Clone(void) const = 0;
		
		virtual ~ITypeErasure(void){}

	protected: 
		ITypeErasure(void){};

	private:		
		ITypeErasure(const ITypeErasure&);//forbidden member
		const ITypeErasure& operator=(const ITypeErasure&);//forbidden member
	};	
	
	template<class TSourceIterator>
	class TTypeErasure: public ITypeErasure
	{
	public:
		explicit TTypeErasure(const TSourceIterator& iterator_):
			_iterator(iterator_)
		{}

	private:
		TSourceIterator _iterator;
		void increment(void) {++(this->_iterator);}
		void decrement(void) {--(this->_iterator);}
		bool equal(const PIterator& other) const
		{
			const TTypeErasure& OtherIterator = *boost::polymorphic_downcast<TTypeErasure*>(other.get());
			return this->_iterator == OtherIterator._iterator;
		}
		typename TEnvelop::reference dereference(void) const {return *(this->_iterator);}
		void advance(const typename TMe::difference_type difference) {this->_iterator+=difference;}
		typename TEnvelop::difference_type distance_to(const PIterator& other)
		{
			const TTypeErasure& OtherIterator = *boost::polymorphic_downcast<TTypeErasure*>(other.get());
			return std::distance(this->_iterator, OtherIterator._iterator);
		}
		
		TTypeErasure(const TTypeErasure& other):
			_iterator(other._iterator)
		{}

		//friend typename ::boost::interprocess::unique_ptr_generator<TTypeErasure>::type make_unique_(const TTypeErasure&);
		
		PIterator Clone(void) const
		{
			return PIterator(new TTypeErasure(*this));
			//using namespace ::boost::interprocess;
			//return static_pointer_cast_<ITypeErasure>(make_unique_<TTypeErasure>(this));
			////       ^  static_pointer_cast_ needed, because boost::interprocess::unique_ptr have bug.
		}

		TTypeErasure(void);//forbidden member		
		const TTypeErasure& operator=(const TTypeErasure&);//forbidden member		
	};
	
	PIterator _piterator;
	
public:
	TPolymorphicMutableRandomAccessIterator(void):
		_piterator()
	{}
	
	template<class TSourceIterator>
	//explicit
	TPolymorphicMutableRandomAccessIterator(const TSourceIterator& SourceIterator):
		_piterator(new TTypeErasure<TSourceIterator>(SourceIterator))
	{}

	TPolymorphicMutableRandomAccessIterator(const TPolymorphicMutableRandomAccessIterator& SourceIterator):
		_piterator(SourceIterator._piterator->Clone())
	{}

	TPolymorphicMutableRandomAccessIterator(BOOST_RV_REF(TPolymorphicMutableRandomAccessIterator) SourceIterator):
		_piterator(boost::move(SourceIterator))
	{}

	const TPolymorphicMutableRandomAccessIterator& operator=(TPolymorphicMutableRandomAccessIterator SourceIterator)
	{
		boost::swap(this->_piterator, SourceIterator._piterator);
		return *this;
	}

	const TPolymorphicMutableRandomAccessIterator& operator=(BOOST_RV_REF(TPolymorphicMutableRandomAccessIterator) SourceIterator)
	{
		this->_piterator=boost::move(SourceIterator._piterator);
		return *this;
	}
	
private:
	void increment(void) { return this->_piterator->increment(); }
	void decrement(void) { return this->_piterator->decrement(); }
	
	bool equal(TMe const& other) const 
	{ 
		if(this == &other)
			return true;
		ASSERT(this->_piterator != other._piterator);
		if(this->_piterator == other._piterator)
			return true;
		if(this->_piterator == NULL || other._piterator == NULL)
			return true;
		//ASSERT(typeid(this->_piterator)==typeid(other._piterator));
		return this->_piterator->equal(other._piterator);
	}
	
	reference dereference(void) const { return this->_piterator->dereference(); }
	void advance(const difference_type difference) { return this->_piterator->advance(difference); }
	difference_type distance_to(TMe const& other) { return this->_piterator->distance_to(other._piterator); }
};

namespace boost
{
	template<class T>
	struct has_nothrow_move<TPolymorphicMutableRandomAccessIterator<T> >
	{
	   static const bool value = true;//std::is_nothrow_move_assignable<TPolymorphicMutableRandomAccessIterator<T>::PIterator>::value && std::is_nothrow_copy_constructible<TPolymorphicMutableRandomAccessIterator<T>::PIterator>::value;
	};
}  //namespace boost{

template<class TIterator>
TPolymorphicMutableRandomAccessIterator<typename boost::iterator_value<TIterator>::type> make_PolymorphicMutableRandomAccessIterator(const TIterator& Iterator)
{
	return TPolymorphicMutableRandomAccessIterator<boost::iterator_value<TIterator>::type>(Iterator);
}

template<class TValue>
TPolymorphicMutableRandomAccessIterator<TValue> make_PolymorphicMutableRandomAccessIterator(void)
{
	return TPolymorphicMutableRandomAccessIterator<TValue>();
}

// template<class TItem>
// class TPolymorphicMutableRandomAccessIteratorContainer: public TAbstractBidirectionalOrRandomAccessIteratorContainer<TPolymorphicMutableRandomAccessIterator<TItem> >
// {
// 	
// };
*/
#endif // range_h__
