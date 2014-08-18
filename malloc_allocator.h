#ifndef malloc_allocator_h__
#define malloc_allocator_h__

#include <cstdlib>
#include <common/containers/allocator_base.h>

template <typename T>
struct malloc_allocator: allocator_base<T, malloc_allocator, ALLOCATOR_ALWAYS_EQUAL>
{
  pointer allocate(const size_type n, const void* = 0)
  {
	const pointer ret = (pointer) ::std::malloc(n * sizeof(T));
    if (ret == 0)
		throw ::std::bad_alloc();
    return ret;
  }
  void deallocate(const pointer p, const size_type)
  { 
	  ::std::free(p); 
  }
  malloc_allocator() BOOST_CONTAINER_NOEXCEPT { }
  template <typename U>
  malloc_allocator(const malloc_allocator<U> & a) BOOST_CONTAINER_NOEXCEPT :TBase(a) { }
  malloc_allocator(const malloc_allocator & a) BOOST_CONTAINER_NOEXCEPT :TBase(a)  { }
};
template <>
struct malloc_allocator<void>: allocator_base<void, malloc_allocator, ALLOCATOR_ALWAYS_EQUAL>{
};
#endif // malloc_allocator_h__