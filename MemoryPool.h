#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include "Common\accessory\accessory.h"
#include "ProjectConfiguration.h"
#include "Common\Wrappers\SystemMemoryAllocator.h"
#include "Common\InternalClasses\MemoryAligment.h"
#include "Common\accessory\ExistanceChecker.h"
#include "on_time_rtos\AllocateException.h"
#include "Common\InternalClasses\MemoryPoolInternalClasses.h"

//------------------------------------------------------------------------------------------

COMMENT((

TAbstractMemoryPool - абстрактный интерфейс для обращения к пулу
TMemoryPool - реализация пула
TMemoryPool генерирует исключение TAllocateException в конструкторе и функции выделения памяти allocate.
Параметры конструктора: размер элемента, количество элементов, имя пула
allocate - выделение памяти из пула
deallocate - возврат памяти в пул
InquiryCount - количество обращений к пулу
InquiryDenied - количество отклоненных обращений к пулу(например, из-за отсутствия свободных буферов)
AmountOfFreeItem - количество свободных элементов в пуле
AmountOfItem - количество элементов в пуле
ItemsAllocated - кол-во выделеных элементов пула
MaxItemsAllocated - кол-во максимально выделяемых элементов пула
MaxItemSize - размер элемента
transferFreeItems - пересылка всех свободных элементов из другого пула

)) // end of COMMENT
//-------------------------------------------------------------------------------------------

typedef class TAbstractMemoryPool
{
private:
	enum {MEMORY_POOL_CHECK_SIGNATURE=0xA2d2ef67};
	const TExistanceChecker<MEMORY_POOL_CHECK_SIGNATURE> _PoolExistanceChecker;

public:
	TAbstractMemoryPool(void);

	virtual UNIQUE_POINTED void* allocate(size_t Size)=0;
	virtual void deallocate(const void* const Pointer)=0;
	
	virtual void transferFreeItems(TAbstractMemoryPool& sourcePool)=0;
	virtual ~TAbstractMemoryPool(void){};
	virtual NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size)=0;
	virtual bool tryFreeAll(void) = 0;
	
//Статистика:
	virtual unsigned int AmountOfFreeItem(void) const=0;
	virtual unsigned int AmountOfItem(void) const=0;
	virtual size_t MaxItemSize(void) const=0;
	virtual size_t InitialAmountOfItem(void) const=0;
	virtual INT32U InquiryCount(void) const=0;
	virtual INT32U InquiryDenied(void) const=0;
	virtual INT32U ItemsAllocated(void) const=0;
	virtual INT32U MaxItemsAllocated(void) const=0;
	virtual TName Name(void) const=0;
	virtual size_t AlignedItemSize(void) const=0;
	bool empty(void) const;
	void CheckSignatureThatThisIsPool(void) const;
	virtual void checkAllFreeItemsAtPoolIfCheckTurnedOn(void) = 0;
} TAbstractMemoryPoolUsedAtShadowMemoryInformation, *PAbstractMemoryPoolUsedAtShadowMemoryInformation;

//TMemoryAlignmentStrategy - стратегия выравнивания элементов по размеру и адресам для увеличения скорости работы приложения.
//TMemoryAllocator - источник памяти для буфера элементов.
//Стратегия TMemoryAllocator для TMemoryPoolSimple должна обязательно обеспечить выравнивание выделяемого блока в соответствие с TMemoryAlignmentStrategy.
//TMaxPoolCheckStrategy отвечает за детализацию проверки пулов:
//TPoolNoCheck - Нет проверок пула памяти, кроме ассертов, если они включены.
//TPoolCheckMemoryCorruptionOfItemInternalStructures - Проверяется порча элемента пула порчей памяти и его повторные удаления.
//TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures - Проверяется удаления элемента не в тот пул, из которого он был порожден. Также порча элемента пула из-за порчи памяти и его повторные удаления.
//TMaxPoolCheckStrategy - это лишь максимально возможная проверка. На деле она не будет превышать параметр установленный в ProjectConfiguration.h в трейте TNoneRTOS32PoolsChecking.

//TMemoryPoolSimple не генерирует исключений не в конструкторе (тк используется по умолчанию TSystemMemoryAllocator), не в allocate (ну а это всегда так).
//Если заменить TMemoryAllocator на TSystemMemoryAllocatorCanThrow, то конструктор начнет генерировать исключение TAllocateException.
template< class TMemoryAllocator = TSystemMemoryAllocator, class TMaxPoolCheckStrategy=TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, class TMemoryAlignmentStrategy=TSystemMemoryAligment >
class TMemoryPoolSimple: public TAbstractMemoryPool
{
private:
	typedef typename TSelectLessDerived<TMaxPoolCheckStrategy, CURRENT_CONFIGURATION_TRAIT(TNoneRTOS32PoolsChecking)::TStatus>::type TPoolCheckStrategy;

public:
	TMemoryPoolSimple(const size_t MaxItemSize, const size_t InitialAmountOfItem, TName Name);
	virtual ~TMemoryPoolSimple(void);

	virtual NOTHROW_FUNCTION UNIQUE_POINTED void* allocate(size_t Size);
	virtual void deallocate(const void* const Pointer);

	virtual void transferFreeItems(TAbstractMemoryPool& sourcePool);

	virtual bool tryFreeAll(void);//Не удалит элементы, добавленные в пул с помощью transferFreeItems

//Статистика:
	virtual size_t AmountOfFreeItem(void) const;
	virtual size_t AmountOfItem(void) const;
	virtual size_t InitialAmountOfItem(void) const;
	virtual size_t MaxItemSize(void) const;
	virtual INT32U InquiryCount(void) const;
	virtual INT32U InquiryDenied(void) const;
	virtual INT32U ItemsAllocated(void) const;
	virtual INT32U MaxItemsAllocated(void) const;
	virtual TName Name(void) const;
	virtual void checkAllFreeItemsAtPoolIfCheckTurnedOn(void);

protected:
	virtual size_t AlignedItemSize(void) const;
	NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size);

private:
	void* _allocationSourceBuffer;
	const size_t _MaxItemSize;
	size_t _allocationSourceBufferSize;
	void* _allocationSourceBufferFirstItem;
	void* _topFreeItem;
		
	typename NMemoryPoolsInternals::TItemMaps<TMemoryAlignmentStrategy>::TFreeItemMap<TPoolCheckStrategy>				_freeItemMap;
	typename NMemoryPoolsInternals::TItemMaps<TMemoryAlignmentStrategy>::TAllocatedItemMap<TPoolCheckStrategy>		_allocatedItemMap;

	const size_t _AlignedItemSize;
	const size_t _PoolBufferSize;

	//Статистика:
	size_t _amountOfFreeItem;
	size_t _amountOfItem;
	size_t _InitialAmountOfItem;
	size_t _maxItemsAllocated;
	INT32U _inquiryCount;
	INT32U _inquiryDenied;
	TName _Name;

	void CheckItemBelongPoolIfCheckActivated(void* const Base) const;
};

//TMemoryPool генерирует исключение TAllocateException в конструкторе (тк используется по умолчанию TSystemMemoryAllocatorCanThrow) и функции выделения памяти allocate (ну а это всегда так).
//Если заменить TMemoryAllocator на TSystemMemoryAllocator, то контруктор перестанет генерировать исключения.
//По параметрам шаблона смотри TMemoryPoolSimple.
template< class TMemoryAllocator = TSystemMemoryAllocatorCanThrow, class TMaxPoolCheckStrategy=TPoolDisallowItemFromAnotherPoolAndCheckMemoryCorruptionOfItemInternalStructures, class TMemoryAlignmentStrategy=TSystemMemoryAligment >
class TMemoryPool: public TMemoryPoolSimple<TMemoryAllocator, TMaxPoolCheckStrategy, TMemoryAlignmentStrategy>
{
public:
	TMemoryPool(const size_t MaxItemSize, const size_t InitialAmountOfItem, TName Name);
	virtual ~TMemoryPool(void){};
	virtual UNIQUE_POINTED void* allocate(size_t Size);
};


COMMENT((
	Все нижесказанное верно при TPoolCheckStrategy в TPoolCheckMemoryCorruptionOfItemInternalStructures. Внимание TMaxPoolCheckStrategy обрезается в соответствии с настройками из ProjectConfiguration.h
	и не смотря на указание уровня проверок в параметре шаблона TPoolCheckMemoryCorruptionOfItemInternalStructures или выше - уровень проверок возмется минимальный из параметра шаблона и файла настроек.
	Теперь TMemoryPool и TMemoryPoolSimple легко отлаживать направив вывод памяти отладчика (Вид\Окна отладки\Память) на адрес из переменной в пуле _allocationSourceBufferFirstItem
	Первый байт элемента пула в 16тиричном коде отвечает за то свободен он или выделен из пула
	SIGNATURE_AND_POOL_ITEM_FREE=0xFE (FreE)
	SIGNATURE_AND_POOL_ITEM_ALLOCATED=0xA1 (ALlocated)
	Дальше идет пустота на выравнивание памяти, если потребуется
	ALIGNMENT_VOIDS_FILL=0xAF (Alignment_Fill)
	Дальше сами данные и в пустом не выделенном элементе пула (а также пока в элемент ничего не записали) они выглядят как:
	ITEM_VALUE=0x7F (Число удобно для отлавливания ошибок неинициализированной памяти, так что не используется говорящая 16тиричная нотация).
	Дальше снова идет пустота ALIGNMENT_VOIDS_FILL на выравнивание памяти, если потребуется.*/
))


class TAbstractPoolForNew
{
private:
	virtual PAbstractMemoryPoolUsedAtShadowMemoryInformation GetMemoryPool(void) =0;	

public:
	virtual ~TAbstractPoolForNew(void){}
	NOTHROW_FUNCTION UNIQUE_POINTED void* allocateWithoutException(size_t Size);
	UNIQUE_POINTED void* allocate(size_t Size);
	void deallocate(const void* const Pointer);
	size_t ItemSize(void);
	
	static void setShadowMemoryInformation(void* allocatedNowBuffer, TAbstractMemoryPool* owner);
	static void* skipShadowMemoryInformation(void* allocatedNowBuffer);
	static void* getRealAllocatedBuffer(void* buffer);
	static PAbstractMemoryPoolUsedAtShadowMemoryInformation getMemoryPoolFromShadowMemoryInformation(void* buffer);
	static size_t getSizeOfAllocatedBufferWithShadowMemoryInformation(const size_t Size);
	
	typedef PAbstractMemoryPoolUsedAtShadowMemoryInformation* TShadowMemoryInformation;

private:
	template<class TFunctor>
	UNIQUE_POINTED void* allocateUniversal(size_t Size, TFunctor cantAllocateHandler/*for example: void (*)(void)*/)
	{
		ASSERT(Size!=0);
		if(Size==0)
			Size=1;
		ASSERT(Size<=this->ItemSize());
		ASSERT(cantAllocateHandler!=NULL);
		if(Size>this->ItemSize())
		{
			cantAllocateHandler();
			return NULL;
		}
		PAbstractMemoryPoolUsedAtShadowMemoryInformation MemoryPool=this->GetMemoryPool();
		ASSERT(MemoryPool!=NULL);
		void* allocatedBuffer=MemoryPool->allocateWithoutException(Size);
		if(allocatedBuffer==NULL)
		{
			cantAllocateHandler();
			return NULL;
		}
		TAbstractPoolForNew::setShadowMemoryInformation(allocatedBuffer, MemoryPool);//Указатель на пул, из которого удалять.
		return TAbstractPoolForNew::skipShadowMemoryInformation(allocatedBuffer);
	}
};

template <class TPoolRealization=TMemoryPool<> >
class TPoolForNew: public TAbstractPoolForNew
{
private:
	TPoolRealization _pool;

	PAbstractMemoryPoolUsedAtShadowMemoryInformation GetMemoryPool(void);

public:
	TPoolForNew(const size_t ItemSize, const size_t AmountOfItem, TName Name);
};

template <class TTypeForAllocate, class TPoolRealization=TMemoryPool<> >
class TPoolForNewConcrete: public TPoolForNew<TPoolRealization>
{
public:
	TPoolForNewConcrete(const size_t AmountOfItem, TName Name);
};

#include "Common\Realization\memoryPoolRealization.h"

#endif
