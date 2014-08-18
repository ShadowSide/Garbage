
#include <Common\UnitTests\UnitTests.h>
#include <Common\MemoryPool.h>
#include <Common\ObjectPool.h>

class TWithoutParams: public TPoolObject
{
public:
	TWithoutParams(void){};
};

class TOneParams: public TPoolObject
{
public:
	TOneParams(int a){};
};

class TTwoParams: public TPoolObject
{
public:
	TTwoParams(int a, double* b){};
};

class TThrowConstructor: public TPoolObject
{
public:
	TThrowConstructor(void){throw std::exception();};
};

UNIT_TEST_MODULE_BEGIN(MEMORY_POOLS_TEST, 5.0)

	UNIT_TEST_ENTRY_POINT
	{
		static const TName PoolName="SystemTestPool";
		{//Без выравнивания:
			typedef TWithoutMemoryAlignment TTestMemoryAlignment;
			{//Простая проверка для одноэлементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=1;
					const size_t AmountOfItem=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Простая проверка для n-элементного пула TMemoryPoolSimple. С проверкой статистики, тк используется в менеджере памяти.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==0);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==0);
					ASSERT(pool.MaxItemsAllocated()==0);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==1);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==2);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==3);
					ASSERT(pool.InquiryDenied()==1);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==0);
					ASSERT(pool.MaxItemsAllocated()==2);

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==5);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==2);

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==6);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					void* const Pointer=pool2.allocate(Size2);
					ASSERT(Pointer!=NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool2.allocate(Size2)==NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка упрощённой передачи элементов для n-элементного пула TMemoryPoolSimple. Немного проверяется статистика.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					ASSERT(pool.AmountOfItem()==AmountOfItem);
					pool.transferFreeItems(pool2);
					ASSERT(pool.AmountOfItem()==AmountOfItem+AmountOfItem2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==0);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			//////////////////////////////////////////////////////////////////////////

			{//Простая проверка для одноэлементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=1;
					const size_t AmountOfItem=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Простая проверка для n-элементного пула TMemoryPoolSimple. С проверкой статистики, тк используется в менеджере памяти.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==0);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==0);
					ASSERT(pool.MaxItemsAllocated()==0);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==1);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==2);
					ASSERT(pool.InquiryDenied()==0);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==3);
					ASSERT(pool.InquiryDenied()==1);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==4);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==0);
					ASSERT(pool.MaxItemsAllocated()==2);

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==5);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==2);

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.InquiryCount()==6);
					ASSERT(pool.InquiryDenied()==2);
					ASSERT(pool.ItemsAllocated()==2);
					ASSERT(pool.MaxItemsAllocated()==2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(pool.AmountOfItem()==AmountOfItem);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					void* const Pointer=pool2.allocate(Size2);
					ASSERT(Pointer!=NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool2.allocate(Size2)==NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка упрощённой передачи элементов для n-элементного пула TMemoryPoolSimple. Немного проверяется статистика.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					ASSERT(pool.AmountOfItem()==AmountOfItem);
					pool.transferFreeItems(pool2);
					ASSERT(pool.AmountOfItem()==AmountOfItem+AmountOfItem2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==0);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};
		};

		{//С выравниванием:
			typedef TCPUSuitableMemoryAlignment TTestMemoryAlignment;
			{//Простая проверка для одноэлементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=1;
					const size_t AmountOfItem=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Простая проверка для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					void* const Pointer=pool2.allocate(Size2);
					ASSERT(Pointer!=NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool2.allocate(Size2)==NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка упрощённой передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolCheckMemoryCorruptionOfItemInternalStructures, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					pool.transferFreeItems(pool2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==0);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			//С выравниванием по умолчанию:
			{//Простая проверка для одноэлементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=1;
					const size_t AmountOfItem=1;
					TMemoryPoolSimple<> pool(Size,AmountOfItem,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			//////////////////////////////////////////////////////////////////////////

			{//Простая проверка для одноэлементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=1;
					const size_t AmountOfItem=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Простая проверка для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer=pool.allocate(Size);
					ASSERT(Pointer!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					void* const Pointer=pool2.allocate(Size2);
					ASSERT(Pointer!=NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

					ASSERT(pool2.allocate(Size2)==NULL);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2-1);

					pool.deallocate(Pointer);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};

			{//Проверка упрощённой передачи элементов для n-элементного пула TMemoryPoolSimple.
				try
				{
					const size_t Size=5;
					const size_t AmountOfItem=2;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool(Size,AmountOfItem,PoolName);
					const size_t Size2=Size;
					const size_t AmountOfItem2=1;
					TMemoryPoolSimple<TSystemMemoryAllocator, TPoolNoCheck, TTestMemoryAlignment> pool2(Size2,AmountOfItem2,PoolName);

					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==AmountOfItem2);

					pool.transferFreeItems(pool2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);
					ASSERT(pool2.MaxItemSize()==Size2);
					ASSERT(pool2.AmountOfFreeItem()==0);

					void* const Pointer2=pool.allocate(Size);
					ASSERT(Pointer2!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer2, Size));

					void* const Pointer3=pool.allocate(Size);
					ASSERT(Pointer3!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer3, Size));

					void* const Pointer4=pool.allocate(Size);
					ASSERT(Pointer4!=NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);
					ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer4, Size));

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					ASSERT(pool.allocate(Size)==NULL);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-2);

					pool.deallocate(Pointer2);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);

					pool.deallocate(Pointer3);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);

					pool.deallocate(Pointer4);
					ASSERT(pool.MaxItemSize()==Size);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem+1);

				}
				catch (TAllocateException&)
				{
					SHOULD_NEVER_REACH_HERE;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
			};
		};
		while(false)
		{
			typedef TSystemMemoryAligment TTestMemoryAlignment;
			try//Проверка TMemoryPool с исключениями и статистикой.
			{
				const size_t Size=1;
				const size_t AmountOfItem=1;
				TMemoryPool<> pool(Size,AmountOfItem,PoolName);

				ASSERT(pool.MaxItemSize()==Size);
				ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
				ASSERT(pool.AmountOfItem()==AmountOfItem);
				ASSERT(pool.InquiryCount()==0);
				ASSERT(pool.InquiryDenied()==0);
				ASSERT(pool.ItemsAllocated()==0);
				ASSERT(pool.MaxItemsAllocated()==0);

				void* const Pointer=pool.allocate(Size);
				ASSERT(pool.InquiryCount()==1);
				ASSERT(pool.InquiryDenied()==0);
				ASSERT(pool.ItemsAllocated()==1);
				ASSERT(pool.MaxItemsAllocated()==1);
				ASSERT(Pointer!=NULL);
				ASSERT(pool.MaxItemSize()==Size);
				ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
				ASSERT(pool.AmountOfItem()==AmountOfItem);
				ASSERT(TTestMemoryAlignment::isAlignedAddressFor(Pointer, Size));

				try
				{
					pool.allocate(Size);
				}
				catch (TAllocateException&)
				{
					ASSERT(pool.AmountOfItem()==AmountOfItem);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
					ASSERT(pool.InquiryCount()==2);
					ASSERT(pool.InquiryDenied()==1);
					ASSERT(pool.ItemsAllocated()==1);
					ASSERT(pool.MaxItemsAllocated()==1);
					break;	
				}
				catch (std::exception&)
				{
					SHOULD_NEVER_REACH_HERE;
				};
				SHOULD_NEVER_REACH_HERE;
			}
			catch (TAllocateException&)
			{
				SHOULD_NEVER_REACH_HERE;	
			}
			catch (std::exception&)
			{
				SHOULD_NEVER_REACH_HERE;
			};
		};

		{//Тест объектного пула.
			try
			{
				{//Проверка пула для объектов без параметров в конструкторе.
					const size_t AmountOfItem=1;
					TObjectPool<TWithoutParams> pool(AmountOfItem,PoolName);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					TWithoutParams* obj1;
					try
					{
						obj1=pool.get();
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
						obj1=pool.get();
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						try
						{
							pool.get();
							SHOULD_NEVER_REACH_HERE;
						}
						catch(TAllocateException&)
						{
						};
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
					}
					catch (std::exception&)
					{
						SHOULD_NEVER_REACH_HERE;
					};
				};

				{//Проверка пула для объектов с одним параметром в конструкторе.
					const size_t AmountOfItem=1;
					TObjectPool<TOneParams> pool(AmountOfItem,PoolName);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					TOneParams* obj1;
					try
					{
						obj1=pool.get(0);
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
						obj1=pool.get(1);
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						try
						{
							pool.get(2);
							SHOULD_NEVER_REACH_HERE;
						}
						catch(TAllocateException&)
						{
						};
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
					}
					catch (std::exception&)
					{
						SHOULD_NEVER_REACH_HERE;
					};
				};

				{//Проверка пула для объектов с двумя параметрами в конструкторе.
					const size_t AmountOfItem=1;
					TObjectPool<TTwoParams> pool(AmountOfItem,PoolName);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					TTwoParams* obj1;
					double tmp=5.0;
					try
					{
						obj1=pool.get(0,&tmp);
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
						obj1=pool.get(1,&tmp);
						ASSERT(obj1!=NULL);
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						try
						{
							pool.get(2,&tmp);
							SHOULD_NEVER_REACH_HERE;
						}
						catch(TAllocateException&)
						{
						};
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem-1);
						delete obj1;
						ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
						obj1=NULL;
					}
					catch (std::exception&)
					{
						SHOULD_NEVER_REACH_HERE;
					};
				};
				{//Проверка пула для объектов без параметров в конструкторе.
					const size_t AmountOfItem=1;
					TObjectPool<TThrowConstructor> pool(AmountOfItem,PoolName);
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
					TThrowConstructor* obj1;
					try
					{
						obj1=pool.get();
						SHOULD_NEVER_REACH_HERE;				
					}
					catch (std::exception&)
					{
					};
					ASSERT(pool.AmountOfFreeItem()==AmountOfItem);
				};
			}
			catch (TAllocateException&)
			{
				SHOULD_NEVER_REACH_HERE;	
			}
			catch (std::exception&)
			{
				SHOULD_NEVER_REACH_HERE;
			};
		};
	}

UNIT_TEST_MODULE_END