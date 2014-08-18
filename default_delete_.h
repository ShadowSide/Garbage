#ifndef default_delete__h__
#define default_delete__h__

//Аналог std::default_delete. Требуется для boost::interprocess::unique_ptr.

#include <Common\accessory\accessory.h>
#include <boost/checked_delete.hpp>

//See std::default_delete at
// http://en.cppreference.com/w/cpp/memory/default_delete

template<class T> struct default_delete_
{
	void operator()(const T* const P) const
	{
		::boost::checked_delete(P);
	}
};

template<class T> struct default_delete_<T[]>
{
	void operator()(const T* const P) const
	{
		::boost::checked_array_delete(P);
	}
};

template<class T, size_t N> struct default_delete_<T[N]>
{
	void operator()(const T* const P) const
	{
		::boost::checked_array_delete(P);
	}
};

template<class T, size_t N> struct default_delete_<T(&)[N]>
{
	void operator()(const T* const P) const
	{
		::boost::checked_array_delete(P);
	}
};

#endif // default_delete__h__
