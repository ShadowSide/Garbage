#ifndef intrusive_ptr_support_stateless_allocators_h__
#define intrusive_ptr_support_stateless_allocators_h__

#include <Common\Accessory\Accessory.h>
#include <boost/intrusive_ptr.hpp>

namespace NTMemoryAllocatorDeleterSupportForBoostIntrusivePtr{
	template<class TStatelessMemoryAllocator, typename Derived>
	struct counted_base_access;

	template<class TStatelessMemoryAllocator, typename Derived>
	struct intrusive_ref_counter_with__TStatelessMemoryAllocator
	{
		long use_count() const
		{
			return this->count_;
		}

	protected:
		intrusive_ref_counter_with__TStatelessMemoryAllocator()
			: count_(0)
		{
		}

		intrusive_ref_counter_with__TStatelessMemoryAllocator(intrusive_ref_counter_with__TStatelessMemoryAllocator const &)
			: count_(0)
		{
		}

		intrusive_ref_counter_with__TStatelessMemoryAllocator &operator =(intrusive_ref_counter_with__TStatelessMemoryAllocator const &)
		{
			return *this;
		}

	private:
		friend struct counted_base_access<TStatelessMemoryAllocator, Derived>;
		mutable volatile ::std::size_t count_;
	};

	//////////////////////////////////////////////////////////////////////////////
	// counted_base_access
	template<class TStatelessMemoryAllocator, typename Derived>
	struct counted_base_access
	{
		static void add_ref(intrusive_ref_counter_with__TStatelessMemoryAllocator<TStatelessMemoryAllocator, Derived> const *that)
		{
			++that->count_;
		}

		static void release(intrusive_ref_counter_with__TStatelessMemoryAllocator<TStatelessMemoryAllocator, Derived> const *that)
		{
			ASSERT(0 < that->count_);
			if(0 == --that->count_)
			{
				TStatelessMemoryAllocator::deallocate(static_cast<Derived const *>(that));
			}
		}
	};

	template<typename TStatelessMemoryAllocator, typename Derived>
	inline void intrusive_ptr_add_ref(intrusive_ref_counter_with__TStatelessMemoryAllocator<TStatelessMemoryAllocator, Derived> const *that)
	{
		counted_base_access<TStatelessMemoryAllocator, Derived>::add_ref(that);
	}

	template<typename TStatelessMemoryAllocator, typename Derived>
	inline void intrusive_ptr_release(intrusive_ref_counter_with__TStatelessMemoryAllocator<TStatelessMemoryAllocator, Derived> const *that)
	{
		counted_base_access<TStatelessMemoryAllocator, Derived>::release(that);
	}
}

#endif // intrusive_ptr_support_stateless_allocators_h__
