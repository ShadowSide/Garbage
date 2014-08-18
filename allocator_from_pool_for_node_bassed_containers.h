#ifndef allocator_from_pool_for_node_bassed_containers_h__
#define allocator_from_pool_for_node_bassed_containers_h__

#include <Common\Accessory\Accessory.h>
#include <Common\Containers\allocator_base.h>
#include <Common\ForBoost\intrusive_ptr_support_stateless_allocators.h>
#include <Common/MemoryPool.h>
#include <AllocateException.h>
#include <boost/scope_exit.hpp>

namespace NSmallSingleBlockStdAllocator{
	namespace NSmallSingleBlockStdAllocatorInternal{
		template<class TStatelessMemoryAllocator, const bool checkingAtDestructor, const bool checkingAtAllocateDeallocate, const bool willFree>
		struct small_single_block_std_allocator_implementation: ::NTMemoryAllocatorDeleterSupportForBoostIntrusivePtr::intrusive_ref_counter_with__TStatelessMemoryAllocator<TStatelessMemoryAllocator, small_single_block_std_allocator_implementation<TStatelessMemoryAllocator, checkingAtDestructor, checkingAtAllocateDeallocate, willFree> >{
			typedef ::std::size_t size_type;
			pointer allocate (size_type n){
				ASSERT(n<=1);
				if(checkingAtAllocateDeallocate)
					this->check();
				pointer result = this->_pool.allocate(this->_pool.MaxItemSize());
				if(checkingAtAllocateDeallocate)
					this->check();
				return result;
			}
			void deallocate (pointer p, size_type n){
				ASSERT(n<=1);
				if(checkingAtAllocateDeallocate)
					this->check();
				if(!p)
					return;
				this->_pool.deallocate(p);
				if(checkingAtAllocateDeallocate)
					this->check();
			}
			void preallocate(void){}
			size_type max_size(void){return this->_pool.AmountOfFreeItem();}
			void check(void){
				this->_pool.checkAllFreeItemsAtPoolIfCheckTurnedOn();
			}
			small_single_block_std_allocator_implementation(const ::std::size_t MaxItemSize, const ::std::size_t MaxItemAmount):
				_pool(MaxItemSize, MaxItemAmount, "small_single_block_std_allocator")
			{}
			~small_single_block_std_allocator_implementation(void){
				if(checkingAtDestructor)
					this->check();
				if(willFree){
					const bool freed = this->_pool.tryFreeAll();
					ASSERT(freed);
					(void)freed;
				}
			}
		private:
			TMemoryPool<TStatelessMemoryAllocator> _pool;
			small_single_block_std_allocator_implementation(const small_single_block_std_allocator_implementation&);//forbidden member
			small_single_block_std_allocator_implementation(void);//forbidden member
			small_single_block_std_allocator_implementation& operator=(const small_single_block_std_allocator_implementation&);//forbidden member
		};
	}
	template<const ::std::size_t MaxItemSize, const ::std::size_t MaxItemAmount, class TStatelessMemoryAllocator = TSystemMemoryAllocatorCanThrow, const bool checkingAtDestructor = false, const bool checkingAtAllocateDeallocate = false, const bool willFree = true>
	class small_single_block_std_allocator{
		typedef TStatelessMemoryAllocator TStatelessMemoryAllocator;
		typedef ::std::size_t size_type;
		BOOST_STATIC_CONSTANT(size_type, MaxItemSize = MaxItemSize);
		BOOST_STATIC_CONSTANT(size_type, MaxItemAmount = MaxItemAmount);
		BOOST_STATIC_CONSTANT(bool, checkingAtDestructor = checkingAtDestructor);
		BOOST_STATIC_CONSTANT(bool, checkingAtAllocateDeallocate = checkingAtAllocateDeallocate);
		BOOST_STATIC_CONSTANT(bool, willFree = willFree);
	public:
		template<class TItemForAllocate>
		struct apply;
		template<>
		struct apply<void>: allocator_base<void, apply>{//Не стандарт, тк в шаблоне специализация. Переделать.
		};
		template<class TItemForAllocate>
		struct apply: allocator_base<TItemForAllocate, apply>{
		private:
			typedef NSmallSingleBlockStdAllocatorInternal::small_single_block_std_allocator_implementation<TStatelessMemoryAllocator, checkingAtDestructor, checkingAtAllocateDeallocate, willFree> TImpl;
			typedef ::boost::intrusive_ptr<TImpl> PImpl;
			PImpl _impl;
		public:
			apply() BOOST_CONTAINER_NOEXCEPT {this->silentPreallocate();}
			apply (const apply& alloc) BOOST_CONTAINER_NOEXCEPT: _impl(alloc._impl), TBase(alloc) {}
			apply (const apply<void>&) BOOST_CONTAINER_NOEXCEPT{this->silentPreallocate();}
			template <class U>
			apply (const apply<U>& alloc) BOOST_CONTAINER_NOEXCEPT : _impl(alloc.impl()), TBase(alloc) {}
			apply& operator= (const apply& alloc) BOOST_CONTAINER_NOEXCEPT {
				this->_impl = alloc._impl;
				return this;
			}
			size_type max_size(void) const BOOST_CONTAINER_NOEXCEPT {return	this->_impl ? this->_impl->max_size() : MaxItemAmount;}
			pointer allocate (size_type n, typename apply<void>::const_pointer = 0){
				ASSERT(n<=1);//unsuitable allocator for this container.
				if(!this->_impl)
					throw ::std::bad_alloc();
				pointer result = NULL;
				try{
					result = ((pointer)this->_impl->allocate(n));
				}catch(TAllocateException&){
					throw ::std::bad_alloc();
				}catch(::std::exception&){
					SHOULD_NEVER_REACH_HERE;
					throw ::std::bad_alloc();
				}
				if(!result)
					throw ::std::bad_alloc();
				return result;
			}
			void deallocate (pointer p, size_type n){
				ASSERT(n<=1);//dealocating too many.
				ASSERT(this->_impl);//deallocating that wasn't allocated.
				this->_impl->deallocate(p,n);
			}
			void silentPreallocate(void) BOOST_CONTAINER_NOEXCEPT {
				try{
					this->preallocate();
				}catch(TAllocateException&){
				}catch(::std::exception&){
					SHOULD_NEVER_REACH_HERE;
				}
			}
			void preallocate(void){
				if(this->_impl)
					return;
				TImpl* pimpl = (TImpl*)TStatelessMemoryAllocator::allocate(sizeof(TImpl));
				if(!pimpl)
					return;
				BOOST_SCOPE_EXIT(pimpl){
					if(::std::uncaught_exception())
						TStatelessMemoryAllocator::deallocate(pimpl);
				}BOOST_SCOPE_EXIT_END
					::new((void*)pimpl) TImpl(MaxItemSize, MaxItemAmount);
				this->_impl = pimpl;
				this->_impl->preallocate();
			}
			void check(void){this->_impl->check();}
			PImpl impl(void) const BOOST_CONTAINER_NOEXCEPT {return this->_impl;}
			BOOST_STATIC_CONSTANT (size_type, MAX_ITEM_SIZE = MaxItemSize);
			friend void swap(apply& l, apply& r) BOOST_CONTAINER_NOEXCEPT {
				//::boost::swap(static_cast<TBase&>(l), static_cast<TBase&>(r));
				::boost::swap(l._impl, r._impl);
			}
			template<class T2>
			friend bool operator==( const apply<TItemForAllocate>& lhs, const apply<T2>& rhs ) BOOST_CONTAINER_NOEXCEPT {
				return lhs._impl==rhs.impl();
			}
		};
	};
}


#endif // allocator_from_pool_for_node_bassed_containers_h__
