#ifndef allocator_base_h__
#define allocator_base_h__

#include <Common\Accessory\Accessory.h>
#include <boost/config/suffix.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/limits.hpp>

enum TAllocatorEquality{
	ALLOCATOR_ALWAYS_EQUAL = true,
	ALLOCATOR_ALWAYS_NOT_EQUAL = false
};

template<class TItemForAllocate, template<class> class TDerived, TAllocatorEquality isAlwaysEqual=ALLOCATOR_ALWAYS_NOT_EQUAL>
struct allocator_base;
template<template<class> class TDerived, TAllocatorEquality isAlwaysEqual>
struct allocator_base<void, TDerived, isAlwaysEqual>{
	typedef void value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	template <class U> struct rebind { typedef TDerived<U> other; };
protected:
	typedef allocator_base TBase;
};
template<class TItemForAllocate, template<class> class TDerived, TAllocatorEquality isAlwaysEqual>
struct allocator_base{
	typedef TItemForAllocate value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template <class U> struct rebind { typedef TDerived<U> other; };
	pointer address ( reference x ) const BOOST_CONTAINER_NOEXCEPT {return ::boost::addressof(x);}
	const_pointer address ( const_reference x ) const BOOST_CONTAINER_NOEXCEPT {return ::boost::addressof(x);}
	void construct ( pointer p, const_reference val ) {new ((void*)p) value_type (val); }
	void destroy (pointer p) {p->~value_type();}
	size_type max_size() const BOOST_CONTAINER_NOEXCEPT {return	(::std::numeric_limits<size_type>::max)();}
protected:
	allocator_base() BOOST_CONTAINER_NOEXCEPT { }
	template <template<class> class D, TAllocatorEquality AE>
	allocator_base(const allocator_base<void, D, AE> &) BOOST_CONTAINER_NOEXCEPT { }
	template <typename U, template<class> class D, TAllocatorEquality AE>
	allocator_base(const allocator_base<U, D, AE> & ) BOOST_CONTAINER_NOEXCEPT { }
	allocator_base(const allocator_base & ) BOOST_CONTAINER_NOEXCEPT { }
	typedef allocator_base TBase;
};
template<class T1, template<class> class TDerived1, class T2, template<class> class TDerived2, TAllocatorEquality isAlwaysEqual>
inline bool operator==( const allocator_base<T1, TDerived1, isAlwaysEqual>& lhs, const allocator_base<T2, TDerived2, isAlwaysEqual>& rhs ) BOOST_CONTAINER_NOEXCEPT {//override required
	return isAlwaysEqual;
}
template<class T1, template<class> class TDerived1, class T2, template<class> class TDerived2, TAllocatorEquality isAlwaysEqual>
inline bool operator!=( const allocator_base<T1, TDerived1, isAlwaysEqual>& lhs, const allocator_base<T2, TDerived2, isAlwaysEqual>& rhs ) {
	return !(static_cast<const TDerived1<T1>&>(lhs)==static_cast<const TDerived2<T2>&>(rhs));
}

#endif // allocator_base_h__
