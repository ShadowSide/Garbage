#ifndef inplace_fixed_deque_h__
#define inplace_fixed_deque_h__

//Container template<class T, std::size_t item_amount> inplace_fixed_deque similar to STL-container std::deque. 
//Differences and features:
//Container inplace (not use allocator) and have fixed maximal size item_amount. Insertion\deleteion at begin or at and O(1) same as at std::deque. Insertion at other position - O(n), but not implemented yet.
//Container haven't each methods from std::deque, because it not need now, but you can add any.
//See http://en.cppreference.com/w/cpp/container/deque for interface.
//Realization is closer to boost::circular_buffer or boost::container::static_vector.
//Container don't use pattern "pimpl" unlike the STL-library, therefore swap, move, assign, and assignment operator is slow and haven't exception guarantee.
//Default constructor of container can throw unlike the STL-library.
//Requirements to T: non throw destructor, copy constructor, copy assignment.
//Container use name convention similar to boost-library, sorry.
//Unused PIMPL and allocators, fixed container length is main features of this container. Don't change this.
//std::swap of this container not suit for O(1). It suit for O(n).
//Not pass to inplace_fixed_deque::emplace_front or to inplace_fixed_deque::emplace_back objects stored at container itself. It's UB. But you can at inplace_fixed_deque::push_back and inplace_fixed_deque::push_front.
//Container empty, if in assignment operator throw exception at copy constructor of any coped item.
//Dirrect or undirrect (by using iterators range) assignment (c1=c1) is UB.

#include <Common\accessory\accessory.h>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/utility/swap.hpp>
#include <memory>
#include <algorithm>
#include <Common\Containers\containers_utility.hpp>
#include <boost/function.hpp>
#include <utility>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/move/utility.hpp>
#include <Common/ForBoost/boost_preprocessor_extension.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)  
# pragma warning(push)  
# pragma warning(disable:4996) // 'std::equal': Function call with parameters that may be unsafe
#endif

template<class T, std::size_t item_amount>
class inplace_fixed_deque;

namespace inplace_fixed_deque_detail
{
	template<class Source>
	struct iterator_value_t
	{
		typedef typename boost::mpl::if_<boost::is_const<Source>, const typename Source::value_type, typename Source::value_type >::type type;
	};
	
	template <class Source>
	struct iterator_implementation_template
		: boost::iterator_facade<
			iterator_implementation_template<Source>,
			typename iterator_value_t<Source>::type,
			boost::random_access_traversal_tag
		>
	{
	private:
		typedef typename iterator_value_t<Source>::type value_t;
		typedef typename Source::size_type index_t;
		typedef iterator_implementation_template me;
		typedef Source* source_t;
		typedef boost::iterator_facade<	iterator_implementation_template, value_t, boost::random_access_traversal_tag> base;
	
	public:	
		typedef typename base::reference reference;
		typedef typename base::difference_type difference_type;
		typedef typename Source::size_type size_type;
		
		iterator_implementation_template(void) /*BOOST_NOEXCEPT_OR_NOTHROW*/
			: m_source(0), m_index(0)
		{}
		
		iterator_implementation_template(const source_t source, const index_t index) /*BOOST_NOEXCEPT_OR_NOTHROW*/
			: m_source(source), m_index(index)
		{}
		
		template <class OtherSource>
		iterator_implementation_template(iterator_implementation_template<OtherSource> const& other) /*BOOST_NOEXCEPT_OR_NOTHROW*/
			: m_source(other.m_source), m_index(other.m_index) 
		{}
		
		iterator_implementation_template(me const& other) /*BOOST_NOEXCEPT_OR_NOTHROW*/
			: m_source(other.m_source), m_index(other.m_index) 
		{}
		
		template <class OtherSource>
		iterator_implementation_template& operator=(const iterator_implementation_template<OtherSource>& other) BOOST_NOEXCEPT_OR_NOTHROW
		{
			me coped(other.m_source, other.m_index);
			boost::swap(coped, *this);
			return *this;
		}
		
		iterator_implementation_template& operator=(me other) BOOST_NOEXCEPT_OR_NOTHROW
		{
			boost::swap(other, *this);
			return *this;
		}
		
		friend void swap(
			iterator_implementation_template& l,
			iterator_implementation_template& r) BOOST_NOEXCEPT_OR_NOTHROW
		{
			boost::swap(l.m_source, r.m_source);
			boost::swap(l.m_index, r.m_index);
		}

	private:
		friend class boost::iterator_core_access;
		template <class> friend struct iterator_implementation_template;

		reference dereference(void) const BOOST_NOEXCEPT_OR_NOTHROW;		

		template <class OtherSource>
		bool equal(iterator_implementation_template<OtherSource> const& other) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			ASSERT((&other!=NULL)&&(this!=NULL));
			if((&other==NULL)||(this==NULL))
				return true;
			ASSERT(other.m_source==this->m_source);
			if(this->m_source!=other.m_source)
				return true;
			ASSERT((other.m_source!=NULL)&&(this->m_source!=NULL));
			if((other.m_source==NULL)||(this->m_source==NULL))
				return true;
			return this->m_index==other.m_index;
		}

		void increment(void) BOOST_NOEXCEPT_OR_NOTHROW;

		void decrement(void) BOOST_NOEXCEPT_OR_NOTHROW;

		void advance(difference_type added_to_index) BOOST_NOEXCEPT_OR_NOTHROW;

		template <class OtherSource>
		difference_type distance_to(iterator_implementation_template<OtherSource> const& other) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			ASSERT(this!=NULL);
			if(this==NULL)
				return 0;
			ASSERT(this->m_source!=NULL);
			if(this->m_source==NULL)
				return 0;
			ASSERT(other.m_source==this->m_source);
			if(this->m_source!=other.m_source)
				return 0;
			ASSERT((other.m_source!=NULL)&&(this->m_source!=NULL));
			if((other.m_source==NULL)||(this->m_source==NULL))
				return 0;
			if(other.m_index >= this->m_index)
				return other.m_index - this->m_index;
			const size_type unsigned_distance = this->m_index - other.m_index;
			return - (difference_type)(unsigned_distance);
		}
		
		source_t m_source;
		index_t m_index;
	};
	
	template<class T, std::size_t item_amount>
	struct iterator_implementation_from_T
	{
		typedef iterator_implementation_template<inplace_fixed_deque<T, item_amount> >			iterator_implementation;
		typedef iterator_implementation_template<inplace_fixed_deque<T, item_amount> const>		iterator_const_implementation;
	};
}

template<class T, std::size_t item_amount>
class inplace_fixed_deque
{
public:
	// type definitions
	typedef T																															value_type;
	typedef typename inplace_fixed_deque_detail::iterator_implementation_from_T<T, item_amount>::iterator_implementation				iterator;
	typedef typename inplace_fixed_deque_detail::iterator_implementation_from_T<T, item_amount>::iterator_const_implementation			const_iterator;
	typedef std::reverse_iterator<iterator>																								reverse_iterator;
	typedef std::reverse_iterator<const_iterator>																						const_reverse_iterator;
	typedef T&																															reference;
	typedef const T&																													const_reference;
	typedef std::size_t																													size_type;
	typedef std::ptrdiff_t																												difference_type;

private:
	MEMBER_STATIC_CHECK(item_amount>0, inplace_fixed_deque__must_used_with__item_amount__more_then_0);
	BOOST_STATIC_CONSTANT(size_type, CAPACITY=item_amount);
	
	typedef char raw_byte_array[sizeof(T)*CAPACITY];
	raw_byte_array m_raw_byte_array;
	
	typedef inplace_fixed_deque<T, item_amount> me;
	typedef T* raw_array_iterator;
	typedef const T* raw_array_const_iterator;
	
	size_type m_begin, m_end;
	
	raw_array_iterator raw_array_iterator_unsafe_at(const size_type position) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(position>=0);
		ASSERT(position<=this->capacity());
		return &(((T*)(this->m_raw_byte_array))[position]);
	}
	
	raw_array_const_iterator raw_array_iterator_unsafe_at(const size_type position) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_cast<me*>(this)->raw_array_iterator_unsafe_at(position);
	}
	
	raw_array_iterator raw_array_iterator_at(const size_type position) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(position<this->capacity());
		return this->raw_array_iterator_unsafe_at(position);
	}
	
	raw_array_const_iterator raw_array_iterator_at(const size_type position) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_cast<me*>(this)->raw_array_iterator_at(position);
	}
	
	void pretend_clear(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		this->m_begin=0;
		this->m_end=0;
	}
	
	template<class ForwardIterator>
	void assign_internal(ForwardIterator first, const ForwardIterator& last)//weak exception safety guarantee.
	{
		this->clear();
		const raw_array_iterator destination_begin=this->raw_array_iterator_at(0);
		const raw_array_iterator destination_end=this->raw_array_iterator_unsafe_at(this->capacity());
		const containers_utility::uninitialized_copy_simple_result_t result=containers_utility::uninitialized_copy_and_rollback_if_exception(first, last, destination_begin, destination_end);
		ASSERT(result.m_transaction_fully_successful);
		this->m_begin=0;
		this->m_end=result.m_sucessfull_inserted_items_amount;
	}
	
	size_type size_internal(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		if(this->m_begin<=this->m_end)
			return this->m_end-this->m_begin;
		else
			return this->capacity()-this->m_begin+this->m_end+1;
	}
	
	void construct_at_position(const size_type position, const_reference value)
	{
		ASSERT(position>=0);
		ASSERT(position<this->capacity());
		const raw_array_iterator construct_position=this->raw_array_iterator_at(position);
		ASSERT((char*)&*construct_position>=(char*)(this->m_raw_byte_array));
		ASSERT((char*)&*construct_position<=(char*)(&(((T*)(&(this->m_raw_byte_array)))[this->capacity()-1])));
		ASSERT(&*construct_position!=&value);
		std::uninitialized_fill_n(construct_position, 1, value);
	}
	
	class DestroyValue
	{
	private:
		const me* const _This;
		
	public:
		DestroyValue(const me* const This) BOOST_NOEXCEPT_OR_NOTHROW:
			_This(This)
		{}
		
		void operator()(const T& item) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			ASSERT((char*)&item>=(char*)(this->_This->m_raw_byte_array));
			ASSERT((char*)&item<=(char*)(&(((T*)(&(this->_This->m_raw_byte_array)))[this->_This->capacity()-1])));
			me::destroy_value_release(item);
		}
	} m_destroy_value;
	
	static void destroy_value_release(const T& item) BOOST_NOEXCEPT_OR_NOTHROW
	{
		containers_utility::destroy_value(item);
	}
	
	void destroy_by_position(const size_type position) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(position>=0);
		ASSERT(position<this->capacity());
		m_destroy_value(*(this->raw_array_iterator_at(position)));
	}
	
	const me* This(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this;
	}
	
	me* This(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this;
	}
	
	void push_back_internal(const_reference value)//strong exception safety guarantee.
	{
		ASSERT(!this->full());//Overflow
		if(this->full())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_end;
		if(this->m_end>=this->capacity())
			new_end=0;
		else
			new_end=this->m_end+1;
		size_type creation_position;
		if(new_end<this->m_begin)
			creation_position=new_end;
		else
			creation_position=this->m_end;
		ASSERT(creation_position>=0);
		ASSERT(new_end>=0);
		ASSERT(creation_position<this->capacity());
		ASSERT(new_end<=this->capacity());
		this->construct_at_position(creation_position, value);
		this->m_end=new_end;
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size+1==this->size());
	}
	
	void push_front_internal(const_reference value)//strong exception safety guarantee.
	{
		ASSERT(!(this->full()));//Overflow
		if(this->full())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_begin;
		size_type new_end=this->m_end;
		if(this->m_begin<=0)
		{
			new_begin=this->capacity()-1;
			if(this->m_end<=0)//if (this->m_end==this->m_begin==0)
				new_end=this->capacity();
			else
				new_end=this->m_end-1;
		}
		else
			new_begin=this->m_begin-1;
		ASSERT(new_begin>=0);
		ASSERT(new_begin<this->capacity());
		ASSERT(new_end>=0);
		ASSERT(new_end<=this->capacity());
		this->construct_at_position(new_begin, value);
		this->m_end=new_end;
		this->m_begin=new_begin;
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size+1==this->size());
	}
	
public:
	size_type capacity(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return CAPACITY;
	}
	
	inplace_fixed_deque(void) BOOST_NOEXCEPT_OR_NOTHROW:
		m_begin(),
		m_end(),
		m_destroy_value(This())
	{}
	
	inplace_fixed_deque(const inplace_fixed_deque& rhs):
		m_begin(),
		m_end(),
		m_destroy_value(This())
	{
		this->assign_internal(boost::begin(rhs), boost::end(rhs));
	}
	
	template<class Range>
	explicit inplace_fixed_deque(const Range& rhs):
		m_begin(),
		m_end(),
		m_destroy_value(This())
	{
		this->assign_internal(boost::begin(rhs), boost::end(rhs));
	}
	
	template<class ForwardIterator>
	inplace_fixed_deque(const ForwardIterator& first, const ForwardIterator& last):
		m_begin(),
		m_end(),
		m_destroy_value(This())
	{
		this->assign_internal(first, last);
	}
	
	inplace_fixed_deque& operator=(const inplace_fixed_deque& rhs)//weak exception safety guarantee.
	{
		if(&rhs==this)
			return *this;
		this->assign_internal(boost::begin(rhs), boost::end(rhs));
		return *this;
	}
	
	template<class Range>
	inplace_fixed_deque& operator=(const Range& rhs)//weak exception safety guarantee.
	{
		this->assign_internal(boost::begin(rhs), boost::end(rhs));
		return *this;
	}
	
	~inplace_fixed_deque(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		this->clear();
	}
	
	void assign(const inplace_fixed_deque& source)//weak exception safety guarantee.
	{
		if(&source==this)
			return;
		this->assign_internal(source);
	}
	
	template<class Range>
	void assign(const Range& source)//weak exception safety guarantee.
	{
		this->assign_internal(boost::begin(source), boost::end(source));
	}
	
	template<class ForwardIterator>
	void assign(const ForwardIterator& first, const ForwardIterator& last)//weak exception safety guarantee.
	{
		this->assign_internal(first, last);
	}
	
	void clear(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		const size_type begin = this->m_begin;
		const size_type end = this->m_end;
		this->pretend_clear();
		if(begin<=end)
			std::for_each(
				this->raw_array_iterator_unsafe_at(begin),
				this->raw_array_iterator_unsafe_at(end),
				this->m_destroy_value);
			else
			{
				std::for_each(
					this->raw_array_iterator_unsafe_at(begin),
					this->raw_array_iterator_unsafe_at(this->capacity()),
					this->m_destroy_value);
				std::for_each(
					this->raw_array_iterator_unsafe_at(0),
					this->raw_array_iterator_unsafe_at(end+1),
					this->m_destroy_value);
			}			
	}
	
	bool empty(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const bool is_empty = this->m_begin==this->m_end;
		ASSERT(is_empty==(this->size()==0));
		return is_empty;
	}
	
	bool full(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		bool is_full;
		if(this->m_begin>this->m_end)
			is_full=(this->m_end+1==this->m_begin);
		else
			is_full=(this->m_begin==0)&&(this->m_end==this->capacity());
		ASSERT(is_full==(this->size()==this->capacity()));
		return is_full;
	}
	
	size_type size(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const size_type result = this->size_internal();
		ASSERT(result>=0);
		ASSERT(result<=this->capacity());
		ASSERT(result<=item_amount);
		ASSERT((this->m_begin==this->m_end)==(result==0));
		return result;
	}

	reference operator[](const size_type index) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(index<this->capacity());
		ASSERT(index<item_amount);
		ASSERT(index<this->size());
		if(this->m_begin<=this->m_end)
		{
			const size_type index_=index+this->m_begin;
			ASSERT(index_<=this->m_end);
			ASSERT(index<this->capacity());
			return ((T*)(this->m_raw_byte_array))[index_];
		}
		else
		{
			const size_type index_=(index+this->m_begin)%(this->capacity());
			ASSERT(index_<this->capacity());
			return ((T*)(this->m_raw_byte_array))[index_];
		}
	}

	const_reference operator[](const size_type index) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return (*const_cast<me*>(this))[index];
	}

	void push_back(const_reference value)//strong exception safety guarantee (GoogleIt).
	{
		/*const char* const ValueAddr = (char*)&value;
		const char* const BeginInternalStorage = this->m_raw_byte_array;
		const char* const EndInternalStorage = (char*)&(((T*)&(this->m_raw_byte_array))[this->capacity()-1]);
		if( (ValueAddr>=BeginInternalStorage) && (ValueAddr<=EndInternalStorage) )
		{
			const value_type Value(value);
			this->push_back_internal(Value);
			return;
		}*/
		this->push_back_internal(value);
	}
	
	/*Generate code:
	template<class ... TArgs>
	void emplace_back(BOOST_FWD_REF(TArgs) ... args)//strong exception safety guarantee.
	{
		ASSERT(!this->full());//Overflow
		if(this->full())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_end;
		if(this->m_end>=this->capacity())
			new_end=0;
		else
			new_end=this->m_end+1;
		size_type creation_position;
		if(new_end<this->m_begin)
			creation_position=new_end;
		else
			creation_position=this->m_end;
		ASSERT(creation_position>=0);
		ASSERT(new_end>=0);
		ASSERT(creation_position<this->capacity());
		ASSERT(new_end<=this->capacity());
		
		const raw_array_iterator construct_position=this->raw_array_iterator_at(creation_position);
		ASSERT((char*)&*construct_position>=(char*)(this->m_raw_byte_array));
		ASSERT((char*)&*construct_position<=(char*)(&(((T*)(&(this->m_raw_byte_array)))[this->capacity()-1])));
		::new(&*construct_position) value_type(boost::forward<TArgs>(args)...);
		
		this->m_end=new_end;
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size+1==this->size());
	}*/

#define AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER 10

#define GENERATE_EMPLACE_BACK(_, ARG_COUNTER, __)								\
	PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)											\
	void emplace_back( PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER) )					\
	{																			\
		ASSERT(!this->full());/*Overflow*/										\
		if(this->full())														\
			return;/*throw std::out_of_range();*/								\
		const size_type old_size=this->size();									\
		size_type new_end;														\
		if(this->m_end>=this->capacity())										\
			new_end=0;															\
		else																	\
			new_end=this->m_end+1;												\
		size_type creation_position;											\
		if(new_end<this->m_begin)												\
			creation_position=new_end;											\
		else																	\
			creation_position=this->m_end;										\
		ASSERT(creation_position>=0);											\
		ASSERT(new_end>=0);														\
		ASSERT(creation_position<this->capacity());								\
		ASSERT(new_end<=this->capacity());										\
																				\
		const raw_array_iterator construct_position=this->raw_array_iterator_at(creation_position);\
		ASSERT((char*)&*construct_position>=(char*)(this->m_raw_byte_array));	\
		ASSERT((char*)&*construct_position<=(char*)(&(((T*)(&(this->m_raw_byte_array)))[this->capacity()-1])));\
		::new(&*construct_position) value_type( PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER) );\
																				\
		this->m_end=new_end;													\
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));				\
		ASSERT(old_size+1==this->size());										\
	}
	BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_EMPLACE_BACK, BOOST_PP_EMPTY())//Если видишь access violation, то это просто сработали ассерты. Вынеси их в отдельные функции для удобства отладки (см GENERATE_EMPLACE_BACK). Мне пока не понадобилось, а сейчас это делать лениво.
	#undef GENERATE_EMPLACE_BACK
	
	void pop_back(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(!(this->empty()));//Underflow
		if(this->empty())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_end;
		if(this->m_end<=0)
			new_end=this->capacity();
		else
			new_end=this->m_end-1;
		size_type destruction_position;
		if(this->m_end<this->m_begin)
			destruction_position=this->m_end;
		else
			destruction_position=new_end;
		ASSERT(destruction_position>=0);
		ASSERT(new_end>=0);
		ASSERT(destruction_position<this->capacity());
		ASSERT(new_end<=this->capacity());
		this->m_end=new_end;
		this->destroy_by_position(destruction_position);		
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size-1==this->size());
	}
	
	void push_front(const_reference value)//strong exception safety guarantee.
	{
		/*const char* const ValueAddr = (char*)&value;
		const char* const BeginInternalStorage = this->m_raw_byte_array;
		const char* const EndInternalStorage = (char*)&(((T*)&(this->m_raw_byte_array))[this->capacity()-1]);
		if( (ValueAddr>=BeginInternalStorage) && (ValueAddr<=EndInternalStorage) )
		{
			const value_type Value(value);
			this->push_front_internal(Value);
			return;
		}*/
		this->push_front_internal(value);
	}
	
	/*Generate code:
	template<class ... TArgs>
	void emplace_front(BOOST_FWD_REF(TArgs)... args)//strong exception safety guarantee.
	{
		ASSERT(!(this->full()));//Overflow
		if(this->full())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_begin;
		size_type new_end=this->m_end;
		if(this->m_begin<=0)
		{
			new_begin=this->capacity()-1;
			if(this->m_end<=0)//if (this->m_end==this->m_begin==0)
				new_end=this->capacity();
			else
				new_end=this->m_end-1;
		}
		else
			new_begin=this->m_begin-1;
		ASSERT(new_begin>=0);
		ASSERT(new_begin<this->capacity());
		ASSERT(new_end>=0);
		ASSERT(new_end<=this->capacity());
		
		const raw_array_iterator construct_position=this->raw_array_iterator_at(new_begin);
		ASSERT((char*)&*construct_position>=(char*)(this->m_raw_byte_array));
		ASSERT((char*)&*construct_position<=(char*)(&(((T*)(&(this->m_raw_byte_array)))[this->capacity()-1])));
		::new(&*construct_position) value_type(boost::forward<TArgs>(args)...);
		
		this->m_end=new_end;
		this->m_begin=new_begin;
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size+1==this->size());
	}*/
#define GENERATE_EMPLACE_FRONT(_, ARG_COUNTER, __)				\
	PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)							\
	void emplace_front(PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER))	\
	{															\
		ASSERT(!(this->full()));/*Overflow*/					\
		if(this->full())										\
			return;/*throw std::out_of_range();*/				\
		const size_type old_size=this->size();					\
		size_type new_begin;									\
		size_type new_end=this->m_end;							\
		if(this->m_begin<=0)									\
		{														\
			new_begin=this->capacity()-1;						\
			if(this->m_end<=0)/*if (this->m_end==this->m_begin==0)*/\
				new_end=this->capacity();						\
			else												\
				new_end=this->m_end-1;							\
		}														\
		else													\
			new_begin=this->m_begin-1;							\
		ASSERT(new_begin>=0);									\
		ASSERT(new_begin<this->capacity());						\
		ASSERT(new_end>=0);										\
		ASSERT(new_end<=this->capacity());						\
																\
		const raw_array_iterator construct_position=this->raw_array_iterator_at(new_begin);\
		ASSERT((char*)&*construct_position>=(char*)(this->m_raw_byte_array));\
		ASSERT((char*)&*construct_position<=(char*)(&(((T*)(&(this->m_raw_byte_array)))[this->capacity()-1])));\
		::new(&*construct_position) value_type( PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER) );\
																\
		this->m_end=new_end;									\
		this->m_begin=new_begin;								\
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));\
		ASSERT(old_size+1==this->size());						\
	}
	BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_EMPLACE_FRONT, BOOST_PP_EMPTY())//Если видишь access violation, то это просто сработали ассерты. Вынеси их в отдельные функции для удобства отладки (см GENERATE_EMPLACE_FRONT). Мне пока не понадобилось, а сейчас это делать лениво.
	#undef GENERATE_EMPLACE_FRONT

	#undef AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER
	
	void pop_front(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(!(this->empty()));//Underflow
		if(this->empty())
			return;//throw std::out_of_range();
		const size_type old_size=this->size();
		size_type new_begin;
		size_type new_end=this->m_end;
		if(this->m_begin>=this->capacity()-1)
		{
			new_begin=0;
			if(this->m_end>=this->capacity())//if(m_begin указывает на последний элемент в конце массива)
				new_end=0;
			else
				new_end=this->m_end+1;
		}
		else
			new_begin=this->m_begin+1;
		ASSERT(new_begin>=0);
		ASSERT(new_begin<this->capacity());
		const size_type destruction_position=this->m_begin;
		ASSERT(destruction_position>=0);
		ASSERT(destruction_position<this->capacity());
		ASSERT(new_end>=0);
		ASSERT(new_end<=this->capacity());
		this->m_begin=new_begin;
		this->m_end=new_end;
		this->destroy_by_position(destruction_position);
		ASSERT((this->m_end==this->m_begin)==(this->size()==0));
		ASSERT(old_size-1==this->size());
	}

	const_reference front(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return (*this)[0];
	}

	reference front(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return (*this)[0];
	}

	const_reference back(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(this->size()>0);
		return (*this)[this->size()-1];
	}

	reference back(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		ASSERT(this->size()>0);
		return (*this)[this->size()-1];
	}

	iterator begin(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return iterator(this, 0);
	}

	iterator end(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return iterator(this, this->size());
	}

	const_iterator begin(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_iterator(this, 0);
	}

	const_iterator end(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_iterator(this, this->size());
	}

	const_iterator cbegin(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this->begin();
	}

	const_iterator cend(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this->end();
	}

	reverse_iterator rbegin(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return reverse_iterator(this->end());
	}

	reverse_iterator rend(void) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return reverse_iterator(this->begin());
	}

	const_reverse_iterator rbegin(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_reverse_iterator(this->end());
	}

	const_reverse_iterator rend(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return const_reverse_iterator(this->begin());
	}

	const_reverse_iterator crbegin(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this->rbegin();
	}

	const_reverse_iterator crend(void) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		return this->rend();
	}
};

template<class T>
class inplace_fixed_deque<T, 0>;

template<class Source>
typename inplace_fixed_deque_detail::
	iterator_implementation_template<Source>::
		reference 

	inplace_fixed_deque_detail::
	iterator_implementation_template<Source>::
			dereference(void) const BOOST_NOEXCEPT_OR_NOTHROW
{
	ASSERT(this!=NULL);
	ASSERT(this->m_source!=NULL);
	ASSERT(this->m_index>=0&&this->m_index<(this->m_source->size()));
	return (*(this->m_source))[this->m_index];
}

template<class Source>
void 
	inplace_fixed_deque_detail::
	iterator_implementation_template<Source>::
		increment(void) BOOST_NOEXCEPT_OR_NOTHROW
{
	ASSERT(this!=NULL);
	if(this==NULL)
		return;
	ASSERT(this->m_source!=NULL);
	if(this->m_source==NULL)
		return;
	ASSERT(this->m_index<=this->m_source->size());
	if(this->m_index>=this->m_source->size())
		return;
	++(this->m_index);
}

template<class Source>
void 
	inplace_fixed_deque_detail::
	iterator_implementation_template<Source>::
		advance(difference_type added_to_index) BOOST_NOEXCEPT_OR_NOTHROW
{
	ASSERT(this!=NULL);
	if(this==NULL)
		return;
	ASSERT(this->m_source!=NULL);
	if(this->m_source==NULL)
		return;
	if(added_to_index >= 0)
	{
		const size_type added_to_index_unsigned = added_to_index;
		ASSERT(this->m_source->size() >= added_to_index_unsigned);
		ASSERT(this->m_source->overflow_check().is_not_overflow_addition(this->m_index, added_to_index_unsigned));
		const size_type new_index = this->m_index + added_to_index_unsigned;
		ASSERT(this->m_source->size() >= new_index);
		if(new_index > this->m_source->size())
		{
			this->m_index = this->m_source->size();
			return;
		}
		this->m_index = new_index;
		return;
	}
	const size_type subtracted_from_index_unsigned = - added_to_index;
	ASSERT(this->m_source->size() >= subtracted_from_index_unsigned);
	ASSERT(this->m_index >= subtracted_from_index_unsigned);
	if(this->m_index < subtracted_from_index_unsigned)
	{
		this->m_index = 0;
		return;
	}
	const size_type new_index = this->m_index - subtracted_from_index_unsigned;
	ASSERT(this->m_source->size() >= new_index);
	this->m_index = new_index;
	return;
}

template<class Source>
void 
	inplace_fixed_deque_detail::
	iterator_implementation_template<Source>::
		decrement(void) BOOST_NOEXCEPT_OR_NOTHROW
{
	ASSERT(this!=NULL);
	if(this==NULL)
		return;
	ASSERT(this->m_source!=NULL);
	if(this->m_source==NULL)
		return;
	ASSERT(this->m_index>0);
	if(this->m_index<=0)
		return;
	if(this->m_index==0)
		this->m_index=this->m_source->size();
	else
		--(this->m_index);
}

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)  
# pragma warning(pop)  
#endif 


#endif // inplace_fixed_deque_h__
