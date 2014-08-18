#ifndef MemoryPoolInternalClasses_h__
#define MemoryPoolInternalClasses_h__

#include <Common\accessory\accessory.h>
#include <string.h>
#include <algorithm>

#if((_MSC_VER<1400) && defined(_MSC_VER)) // только <MSVS2005, а именно на ms vc 6.0.
namespace std
{
	using ::memset;	
};
#endif

#if (_MSC_VER<1400) && defined(_MSC_VER) //for MS VC6.0
	//Поменяй и в файле ProjectConfiguration.h, если нужно поменять!!!
	#define TPoolCheckMemoryCorruptionOfItemInternalStructures TPCMCOIIS
	#define TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures TPDIFAPACMCOIIS
#endif

struct TPoolNoCheck{};
struct TPoolCheckMemoryCorruptionOfItemInternalStructures: TPoolNoCheck{};
struct TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures: TPoolCheckMemoryCorruptionOfItemInternalStructures{};

namespace NMemoryPoolsInternals
{
	struct TPoolChekingConsts
	{
		static const char 
			SIGNATURE_AND_POOL_ITEM_FREE,
			SIGNATURE_AND_POOL_ITEM_ALLOCATED,
			ALIGNMENT_VOIDS_FILL,
			ITEM_VALUE;
	};

	template<class TMemoryAlignmentStrategy>
	struct TItemMaps
	{

		template<class TCheckStrategy>
		class TFreeItemMap;

		template<class TCheckStrategy>
		class TAllocatedItemMap;



		class TItemMap
		{
		protected:
			size_t _alignedItemSize;
			bool _endConstruct;

		public:
			TItemMap():
				_alignedItemSize(0),
				_endConstruct(false)
			{}

			size_t GetAlignedItemSize(void) const
			{
				return _alignedItemSize;
			}

			bool ItemBelongBuffer(void* const Base, void* const BufferFirstItem, const size_t BufferSizeAtBytes) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedSize(BufferSizeAtBytes));
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(BufferFirstItem, TItemMap::GetAlignedItemSize()));
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));//Paranoic mode
				return BufferFirstItem<=Base && ((char*)BufferFirstItem+BufferSizeAtBytes)>=((char*)Base+_alignedItemSize);
			}
		};




		template<>
		class TFreeItemMap<TPoolNoCheck>: public TItemMap
		{
		private:
			typedef TFreeItemMap<TPoolNoCheck> TMe;
			
		public:
			TFreeItemMap<TPoolNoCheck>(void)
			{
				_alignedItemSize=sizeof(void*);
			}

			void endConstruct(const size_t RealAlignedItemSize)
			{
				ASSERT(!_endConstruct);
				ASSERT(_alignedItemSize<=RealAlignedItemSize);
				ASSERT(TMemoryAlignmentStrategy::isAlignedSize(RealAlignedItemSize));
				_alignedItemSize=RealAlignedItemSize;
				_endConstruct=true;
			}

			void Check(void* const Base) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
			}

			void* GetNextFreeItem(void* const Base) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				return *(void**)Base;
			}

			void SetNextFreeItem(void* const Base, void* const NextBase)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				*(void**)Base=NextBase;
			}

			void MakeItem(void* const Base)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				this->SetNextFreeItem(Base, NULL);
			}
		};

		template<>
		class TAllocatedItemMap<TPoolNoCheck>: public TItemMap
		{
		private:
			typedef TAllocatedItemMap<TPoolNoCheck> TMe; 

		public:
			explicit TMe(const size_t MaxItemSize)				
			{
				_alignedItemSize=TMemoryAlignmentStrategy::alignSize(MaxItemSize);
				ASSERT(MaxItemSize!=0);
			}

			void endConstruct(const size_t RealAlignedItemSize)
			{
				ASSERT(!_endConstruct);
				ASSERT(_alignedItemSize<=RealAlignedItemSize);
				ASSERT(TMemoryAlignmentStrategy::isAlignedSize(RealAlignedItemSize));
				_alignedItemSize=RealAlignedItemSize;
				_endConstruct=true;
			}

			void Check(void* const Base) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
			}

			void AllocatedItemBelongPoolCheck(void* const Base, void* const PoolBufferFirstItem, const size_t PoolBufferSizeAtBytes) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
			}

			void* GetItemBase(void* const AllocatedItemPointer) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(AllocatedItemPointer, this->TItemMap::GetAlignedItemSize()));//Only at TPoolNoCheck
				return AllocatedItemPointer;
			}

			void* GetAllocatedPointer(void* const Base) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				return Base;
			}

			void MakeItem(void* const Base)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
			}
		};


		template<>
		class TFreeItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures>: public TItemMap
		{
		private:
			enum 
			{
				SIZE_OF_SIGNATURE=1,
				SIZE_OF_BEGIN_VOIDS=3,//Поправить бы нужно для x64, тк выравнивание не подходящее, но как-нибудь в другой раз, когда занятся будет нечем.
				SIZE_OF_NEXT_ITEM=sizeof(void*),
				SIZE_OF_SIGNATURE_AND_BEGIN_VOIDS=SIZE_OF_SIGNATURE+SIZE_OF_BEGIN_VOIDS,
				SIZE_OF_ITEM_WITHOUT_END_VOIDS=SIZE_OF_SIGNATURE_AND_BEGIN_VOIDS+SIZE_OF_NEXT_ITEM,
				
				SIGNATURE_OFFSET=0,
				BEGIN_VOIDS_OFFSET=SIZE_OF_SIGNATURE,
				NEXT_ITEM_OFFSET=SIZE_OF_SIGNATURE_AND_BEGIN_VOIDS,
				END_VOIDS_OFFSET=SIZE_OF_ITEM_WITHOUT_END_VOIDS
			};
			size_t _endVoidsSize;
			
			typedef TFreeItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures> TMe;

		public:
			TMe(void):				
				_endVoidsSize(0)
			{
				_alignedItemSize=SIZE_OF_ITEM_WITHOUT_END_VOIDS;
			}

			void endConstruct(const size_t RealAlignedItemSize)
			{
				ASSERT(!_endConstruct);
				ASSERT(_alignedItemSize<=RealAlignedItemSize);
				ASSERT(TMemoryAlignmentStrategy::isAlignedSize(RealAlignedItemSize));
				ASSERT(RealAlignedItemSize>=_alignedItemSize);
				_alignedItemSize=RealAlignedItemSize;
				ASSERT(RealAlignedItemSize>=SIZE_OF_ITEM_WITHOUT_END_VOIDS);
				_endVoidsSize=RealAlignedItemSize-SIZE_OF_ITEM_WITHOUT_END_VOIDS;
				_endConstruct=true;
			}

			void Check(void* const Base_) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base_, this->TItemMap::GetAlignedItemSize()));
				const char* const Base=(char*)Base_;
				ASSERT(*(Base+SIGNATURE_OFFSET)!=TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_ALLOCATED);//Элемент занят.
				ASSERT(*(Base+SIGNATURE_OFFSET)==TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_FREE);//Порча памяти.
				const char* const BeginVoids=Base+BEGIN_VOIDS_OFFSET;
				ASSERT(SIZE_OF_BEGIN_VOIDS==std::count(BeginVoids, BeginVoids+SIZE_OF_BEGIN_VOIDS, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL));//Порча памяти.
				const char* const EndVoids=Base+END_VOIDS_OFFSET;
				ASSERT(_endVoidsSize==std::count(EndVoids, EndVoids+_endVoidsSize, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL));//Порча памяти.
			}

			void* GetNextFreeItem(void* const Base) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				return *(void**)((char*)Base+NEXT_ITEM_OFFSET);
			}

			void SetNextFreeItem(void* const Base, void* const NextBase)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				*(void**)((char*)Base+NEXT_ITEM_OFFSET)=NextBase;
			}

			void MakeItem(void* const Base_)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base_, this->TItemMap::GetAlignedItemSize()));
				char* const Base=(char*)Base_;
				*(Base+SIGNATURE_OFFSET)=TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_FREE;
				std::memset(Base+BEGIN_VOIDS_OFFSET, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL, SIZE_OF_BEGIN_VOIDS);
				this->SetNextFreeItem(Base, NULL);
				ASSERT(_endVoidsSize>=0);
				ASSERT(_endVoidsSize<_alignedItemSize);				
				std::memset(Base+END_VOIDS_OFFSET, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL, _endVoidsSize);
			}
		};

		template<>
		class TAllocatedItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures>: public TItemMap
		{
		private:
			enum 
			{
				SIZE_OF_SIGNATURE=1,
				SIGNATURE_OFFSET=0,
				BEGIN_VOIDS_OFFSET=SIZE_OF_SIGNATURE
			};

			size_t _beginVoidsSize, _itemSizeWithoutEndVoids, _endVoidsSize, _itemValueSize;
			ptrdiff_t _endVoidsDiff, _itemValueDiff;

			typedef TAllocatedItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures> TMe;
			
		public:
			explicit TMe(const size_t MaxItemSize)
			{
				ASSERT(MaxItemSize>0);
				_itemValueDiff=(ptrdiff_t)TMemoryAlignmentStrategy::alignAddressFor((void*)SIZE_OF_SIGNATURE, MaxItemSize);
				ASSERT(_itemValueDiff>=SIZE_OF_SIGNATURE);
				_beginVoidsSize=_itemValueDiff-SIZE_OF_SIGNATURE;
				_itemSizeWithoutEndVoids=_itemValueDiff+MaxItemSize;
				_itemValueSize=MaxItemSize;
				_alignedItemSize=TMemoryAlignmentStrategy::alignSize(_itemSizeWithoutEndVoids);
				_endVoidsSize=_alignedItemSize-_itemSizeWithoutEndVoids;
				_endVoidsDiff=_itemSizeWithoutEndVoids;
			}

			void endConstruct(const size_t RealAlignedItemSize)
			{
				ASSERT(!_endConstruct);
				ASSERT(_alignedItemSize<=RealAlignedItemSize);
				ASSERT(TMemoryAlignmentStrategy::isAlignedSize(RealAlignedItemSize));
				_alignedItemSize=RealAlignedItemSize;
				ASSERT(RealAlignedItemSize>=_alignedItemSize);
				if(RealAlignedItemSize>_alignedItemSize)
				{
					_alignedItemSize=RealAlignedItemSize;
					_endVoidsSize=_alignedItemSize-_itemSizeWithoutEndVoids;
				}
				_endConstruct=true;
			}

			void Check(void* const Base_) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base_, this->TItemMap::GetAlignedItemSize()));

				const char* const Base=(char*)Base_;
				ASSERT(*(Base+SIGNATURE_OFFSET)!=TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_FREE);//Элемент уже свободен. Удаляем повторно.
				ASSERT(*(Base+SIGNATURE_OFFSET)==TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_ALLOCATED);//Порча памяти.

				const char* const BeginVoids=Base+BEGIN_VOIDS_OFFSET;
				ASSERT(_beginVoidsSize==std::count(BeginVoids, BeginVoids+_beginVoidsSize, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL));//Порча памяти.
				const char* const EndVoids=Base+_endVoidsDiff;
				ASSERT(_endVoidsSize==std::count(EndVoids, EndVoids+_endVoidsSize, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL));//Порча памяти.				
			}			

			void AllocatedItemBelongPoolCheck(void* const Base, void* const PoolBufferFirstItem, const size_t PoolBufferSizeAtBytes) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
			}

			void* GetItemBase(void* const AllocatedItemPointer) const
			{
				ASSERT(_endConstruct);
				return (char*)AllocatedItemPointer-_itemValueDiff;
			}

			void* GetAllocatedPointer(void* const Base) const
			{
				ASSERT(_endConstruct);
				return (char*)Base+_itemValueDiff;
			}

			void MakeItem(void* const Base_)
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base_, this->TItemMap::GetAlignedItemSize()));
				char* const Base=(char*)Base_;
				*(Base+SIGNATURE_OFFSET)=TPoolChekingConsts::SIGNATURE_AND_POOL_ITEM_ALLOCATED;
				ASSERT(_endVoidsSize>=0);
				ASSERT(_endVoidsSize<_alignedItemSize);
				ASSERT(_beginVoidsSize>=0);
				ASSERT(_beginVoidsSize<_alignedItemSize);
				ASSERT(_endVoidsDiff>=0);
				ASSERT(size_t(_endVoidsDiff)<=_alignedItemSize);
				ASSERT(_itemValueSize>=0);
				ASSERT(_itemValueSize<_alignedItemSize);
				ASSERT(_itemValueDiff>=0);
				ASSERT(size_t(_itemValueDiff)<_alignedItemSize);
				std::memset(Base+BEGIN_VOIDS_OFFSET, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL, _beginVoidsSize);
				std::memset(Base+_itemValueDiff, TPoolChekingConsts::ITEM_VALUE, _itemValueSize);
				std::memset(Base+_endVoidsDiff, TPoolChekingConsts::ALIGNMENT_VOIDS_FILL, _endVoidsSize);
			}
		};



		template<>
		class TFreeItemMap<TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures>: public TFreeItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures>
		{
		private:
			typedef TFreeItemMap<TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures> TMe;
			typedef TFreeItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures> TBase;

		public:
			TMe(void):
				TBase()
			{}
		};

		template<>
		class TAllocatedItemMap<TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures>: public TAllocatedItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures>
		{
		private:
			typedef TAllocatedItemMap<TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures> TMe;
			typedef TAllocatedItemMap<TPoolCheckMemoryCorruptionOfItemInternalStructures> TBase;

		public:			
			explicit TMe(const size_t MaxItemSize):
				TBase(MaxItemSize)
			{}

			void AllocatedItemBelongPoolCheck(void* const Base, void* const PoolBufferFirstItem, const size_t PoolBufferSizeAtBytes) const
			{
				ASSERT(_endConstruct);
				ASSERT(TMemoryAlignmentStrategy::isAlignedAddressFor(Base, this->TItemMap::GetAlignedItemSize()));
				ASSERT(this->ItemBelongBuffer(Base, PoolBufferFirstItem, PoolBufferSizeAtBytes));//Элемент не из этого пула. Чтобы убрать проверку, измени TMaxPoolCheckStrategy.				
			}
		};
	};
};


#endif // MemoryPoolInternalClasses_h__
