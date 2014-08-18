#ifndef LowestLevelAllocator_h__
#define LowestLevelAllocator_h__

#include "accessory.h"

namespace NCanWorkAtCPL0
{
	/*template<class TValue>
	class TCheckedValue
	{
	public:
		TCheckedValue(const TCheckedValue& Value)
		{
		STATIC_CHECK((boost::is_POD<TValue>::value), TValue_must_be_POD);
			STATIC_CHECK((sizeof(TValue)<=sizeof(uintptr_t)), TValue_must_be_simple);
		}
		TCheckedValue(const TValue Value){}
		operator TValue(void){}
		const TValue get(void){}
	};*/
/*#pragma pack (1, push)

	template<class ErrorCodeCheckingMode>
	struct TErrorDetectionCode;

	class TEnableErrorCodeChecking{};
	class TDisableErrorCodeChecking{};
	
	class TErrorDetectionCode<TEnableErrorCodeChecking>
	{
	private:
		TCode _code;

	public:
		typedef INT32U TCode;

		TCode callculate(const void* const Begin, const void* const End) const
		{
			CPL0_ASSERT(Begin%sizeof(TCode)==0, "Begin is not alligned");
			CPL0_ASSERT(End%sizeof(TCode)==0, "End is not alligned");
			CPL0_ASSERT(Begin<=End, "Error order");
			TCode result=0;
			for(const TCode* const Current=Begin;Current!=End;++Current)
				if(&_code!=Current)
					result^=*Current;
			return result;
		}

		template<class TType>
		TCode callculate(const TType& CalculatedBlock) const
		{
			return callculate(&CalculatedBlock, &CalculatedBlock+sizeof(TType));
		}

		void callculateMe(const void* const Begin, const void* const End)
		{
			_code=callculate(Begin, End);
		}

		void callculateMe(const TType& CalculatedBlock)
		{
			_code=callculate(CalculatedBlock);
		}

		bool IsRightCode(const TCode Code) const
		{
			return _code==Code;
		}
	};

	class TErrorDetectionCode<TDisableErrorCodeChecking>
	{
	public:
		typedef INT32U TCode;

		TCode callculate(const void* const Begin, const void* const End) const
		{
			return 0;
		}

		template<class TType>
		TCode callculate(const TType& CalculatedBlock) const
		{
			return 0;
		}

		void callculateMe(const void* const Begin, const void* const End)
		{
			_code=0;
		}

		void callculateMe(const TType& CalculatedBlock)
		{
			_code=0;
		}

		bool IsRightCode(const TCode Code) const
		{
			return true;
		}
	};*/

	void SetPagesAccess(void* const Begin, void* const End, const int Access)
	{
		CPL0_ASSERT(Begin!=NULL, "Address should be more than zero");
		if(Begin==NULL)
			return;
		CPL0_ASSERT(Begin<=End, "Error order");
		CPL0_ASSERT(Begin<End, "Size should be more than zero");
		if(Begin<=End)
			return;
		for(char* current=Begin; current<End; current+=RAM_PAGE_SIZE)
			RTSetPageAccess(current, Access);
		if((char*)End%RAM_PAGE_SIZE!=0)
			RTSetPageAccess(End, Access);
	}

	void SetPagesAccess(void* const Address, const size_t Size, const int Access)
	{
		CPL0_ASSERT(Address!=NULL, "Address should be more than zero");
		if(Address==NULL)
			return;
		CPL0_ASSERT(Size>0, "Size should be more than zero");
		if(Size<=0)
			return;
		char *const EndOfRegion=Address+Size;
		SetPagesAccess(Address, EndOfRegion, Access);
	}

	void SetPagesAccessInPages(void* const Address, const size_t AmountOfPage, const int Access)
	{
		CPL0_ASSERT(Address!=NULL, "Address should be more than zero");
		if(Address==NULL)
			return;
		CPL0_ASSERT(AmountOfPage>0, "AmountOfPage should be more than zero");
		if(AmountOfPage<=0)
			return;
		SetPagesAccess(Address, (char*)AmountOfPage*RAM_PAGE_SIZE, Access);
	}

	void SetPagesAccessAfterReservation(void* const Begin, void* const End, const int Access)
	{
		CPL0_ASSERT(Begin!=NULL, "Address should be more than zero");
		if(Begin==NULL)
			return;
		CPL0_ASSERT(Begin<=End, "Error order");
		CPL0_ASSERT(Begin<End, "Size should be more than zero");
		if(Begin<=End)
			return;
		if(Access!=RT_PG_NOACCESS)
			SetPagesAccess(Begin, End, Access);
	}

	void SetPagesAccessAfterReservation(void* const Address, const size_t Size, const int Access)
	{
		CPL0_ASSERT(Address!=NULL, "Address should be more than zero");
		if(Address==NULL)
			return;
		CPL0_ASSERT(Size>0, "AmountOfPage should be more than zero");
		if(Size<=0)
			return;
		if(Access!=RT_PG_NOACCESS)
			SetPagesAccess(Address, Size, Access);
	}

	void SetPagesAccessInPagesAfterReservation(void* const Address, const size_t AmountOfPage, const int Access)
	{
		CPL0_ASSERT(Address!=NULL, "Address should be more than zero");
		if(Address==NULL)
			return;
		CPL0_ASSERT(AmountOfPage>0, "AmountOfPage should be more than zero");
		if(AmountOfPage<=0)
			return;
		if(Access!=RT_PG_NOACCESS)
			SetPagesAccessInPages(Address, AmountOfPage, Access);
	}

	template<class T>
	void TestIsAligmentSuitableType(Type2Type<T>)
	{
		struct _
		{
			static T& makeT(...);
		}

		/*enum 
		{
			IsPointer=typename boost::is_pointer<T>::value
			IsIntegral=typename boost::is_integral<T>::value
		}
		STATIC_CHECK((IsPointer&&sizeof(boost::remove_pointer<T>::type)==1||IsIntegral), T_must_be_pointer_to_one_size_type_or_integral_type);*/

		STATIC_CHECK((sizeof(T)==sizeof(void*)), platform_portability_recomendation__size_of_T_should_be_is_equal_size_of_pointer);
		STATIC_CHECK((sizeof(T)==sizeof(uintptr_t)), platform_portability_recomendation__size_of_T_should_be_is_equal_size_of_pointer);
		STATIC_CHECK((TTypeChecker<T>::IS_POINTER && sizeof(*(_::makeT()))==1 || (TTypeChecker<T>::IS_SIMPLE && !(TTypeChecker<T>::IS_POINTER))), T_must_be_pointer_to_one_size_type_or_integral_type);
		STATIC_CHECK((TTypeChecker<T>::IS_POINTER || !std::numeric_limits<T>::is_signed), T_must_be_pointer_or_unsigned);
	}
	
	template<class T>
	T Align(const T SizeOrAddress, const size_t AlignStep)
	{	
		const uintptr_t SizeOrAddress_=static_cast<uintptr_t>(SizeOrAddress);
		TestIsAligmentSuitableType(Type2Type<T>());
		CPL0_ASSERT(AlignStep>0, "AlignStep is not correct");
		if(AlignStep<=1)
			return SizeOrAddress;
		const T result = SizeOrAddress_;
		if(SizeOrAddress_%AlignStep!=0)
			result = SizeOrAddress_+(AlignStep-(SizeOrAddress_%AlignStep));
		CPL0_ASSERT(result>=SizeOrAddress_, "Logical error");
		CPL0_ASSERT(result-SizeOrAddress_<AlignStep, "Logical error");
		return result;
	}

	template<class T>
	bool IsAligned(T SizeOrAddress, const size_t AlignStep)
	{
		const uintptr_t SizeOrAddress_=static_cast<uintptr_t>(SizeOrAddress);
		TestIsAligmentSuitableType(Type2Type<T>());
		CPL0_ASSERT(AlignStep>0, "AlignStep is not correct");
		if(AlignStep<=1)
			return true;
		const bool Result = SizeOrAddress_%AlignStep==0;
		CPL0_ASSERT(Result == (SizeOrAddress_==Align(SizeOrAddress_, AlignStep)), "Logical error");
		return Result;
	}

	struct TEnumeratorFunctorResult
	{
		enum E 
		{
			STOP_ENUMERATION=1,
			CONTINUE_ENUMERATION
		};
	};	

	template<class TItem/*, class ErrorCodeCheckingMode*/>
	class TIntrusiveList
	{
	public:
		TItem _head;
		TIntrusiveList* _tail;
		//TErrorDetectionCode<ErrorCodeCheckingMode> _errorDetectionCode;
		
	private:
		TIntrusiveList(const TIntrusiveList&);
		TIntrusiveList(void);
		const TIntrusiveList& operator=(const TIntrusiveList&);
		
	public:
		TIntrusiveList(const TItem& Head, TIntrusiveList* const Tail=NULL)
			:_head(Head)
			,_tail(Tail)
		{}

		/*void* This(void) const
		{
			return this;
		}

		void* AfterList(const size_t Align) const
		{
			return this->This()+sizeof(*this);
		}*/

		/*template<class TFunctor>
		void forEachPair(TFunctor& functor)
		{
			if(this==NULL)
				return;
			TIntrusiveList *current=this, *next=this->_tail;
			while(next==NULL || TEnumeratorFunctorResult::STOP_ENUMERATION==functor(current, next))
			{
				current=next;
				CPL0_ASSERT(next!=NULL, "Logical error");
				next=next->_tail;
			}
		}*/
		
		template<class TFunctor>
		void forEach(TFunctor& functor)
		{
			if(this==NULL)
				return;
			TIntrusiveList* current=this;
			while(current==NULL || TEnumeratorFunctorResult::STOP_ENUMERATION==functor(current))
				current=current->_tail;
		}
	};

	struct TResultOfExtendArea
	{
		enum E
		{
			SUCCESS=true,
			FAILURE_THAT_AND_FURTHER_EXTENSION=false
		};
	};

	struct TAreaHeader
	{
		TAreaHeader(void* const Begin, void* const End, const size_t MaxPagesForExtend, const int PageAccess, const size_t Aligment)
			:_begin((char*)Begin)
			,_end((char*)End)
			,_maxPagesForExtend(MaxPagesForExtend)
			,_PageAccess(PageAccess)
			,_Aligment(Aligment)
		{
			this->CheckInvariant();
			CPL0_ASSERT(_Aligment>=1, "Aligment must be more than zero");
			CPL0_ASSERT(_maxPagesForExtend>=1, "MaxPagesForExtend must be more than zero");
			CPL0_ASSERT(_maxPagesForExtend%2==0, "MaxPagesForExtend need be parity");			
		}

		size_t FreeMemoryAtArea(void) const
		{
			this->CheckInvariant();
			return _end-_begin;
		}

		void* allocateOrNULLWithCantContinueExtendThisArea(const size_t Size)
		{
			this->CheckInvariant();
			CPL0_ASSERT(Size>0, "Size must be more than zero");
			while(this->FreeMemoryAtArea()<Size)
				if(TResultOfExtendArea::FAILURE_THAT_AND_FURTHER_EXTENSION==this->tryExtendArea())
					return NULL;
			return this->allocateWithoutExtend(Size);
		}
		
		void* allocateWithoutExtend(const size_t Size)
		{
			this->CheckInvariant();
			CPL0_ASSERT(this->FreeMemoryAtArea()>=Size, "Logical error");
			if(this->FreeMemoryAtArea()<Size)
				return NULL;
			void* const Result=_begin;
			_begin+=Size;
			return Result;
		}
		
	private:
		void CheckInvariant(void) const
		{
			CPL0_ASSERT(_Begin!=NULL, "Logical error");
			CPL0_ASSERT(_Begin<=_End, "Error order");
			CPL0_ASSERT(IsAligned(_begin, _Aligment), "Aligment error");
			CPL0_ASSERT(IsAligned(_end, RAM_PAGE_SIZE), "Aligment error");
		}

		TResultOfExtendArea::E tryExtendArea(void)
		{
			this->CheckInvariant();
			CPL0_ASSERT(_maxPagesForExtend<=0, "Logical error");
			if(_maxPagesForExtend<=0)
				return TResultOfExtendArea::FAILURE_THAT_AND_FURTHER_EXTENSION;
			int resultOfReservation=RT_MAP_VADDR_NOT_AVAIL;
			size_t amountPagesForTryExtend=_maxPagesForExtend;
			while((resultOfReservation=RTReserveVirtualAddress(&_end, amountPagesForTryExtend*RAM_PAGE_SIZE, RT_MAP_NO_RELOCATE))==RT_MAP_SUCCESS || amountPagesForTryExtend<=1)
				amountPagesForTryExtend/=2;
			_maxPagesForExtend=amountPagesForTryExtend;
			if(resultOfReservation==RT_MAP_SUCCESS)
			{
				const size_t SizeOfExtendedArea=amountPagesForTryExtend*RAM_PAGE_SIZE;
				SetPagesAccessInPagesAfterReservation(_end, amountPagesForTryExtend, _PageAccess);
				_end+=SizeOfExtendedArea;
				this->CheckInvariant();
				return TResultOfExtendArea::SUCCESS;
			}
			_maxPagesForExtend=0;
			return TResultOfExtendArea::FAILURE_THAT_AND_FURTHER_EXTENSION;
		}

		char* _begin, _end;
		size_t _maxPagesForExtend;
		const int _PageAccess;
		const size_t _Aligment;

		TAreaHeader(const TAreaHeader&);
		TAreaHeader(void);
		const TAreaHeader& operator=(const TAreaHeader&);		
	};
	
//#pragma pack (pop)

	//STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TDirectSystemNoAccessedMemoryAllocatorWithoutDeallocate, allocator);

	class TDirectSystemNoAccessedMemoryAllocatorWithoutDeallocate
	{
	private:
		typedef TDirectSystemNoAccessedMemoryAllocatorWithoutDeallocate TThis;

		enum 
		{
			DROP_AREA_IF_HAVE_FREE_MEMORY_LESS_THAN=4,			
			MAX_PAGES_FOR_EXTEND=64,
			ALIGMENT=4
		};
		
		TName _Name;
		TIntrusiveList<TAreaHeader>* _allocatedAreas;
		const int _AllocatedPagesAccess;
		const size_t _DropAreaIfThatHaveFreeMemoryLessThan, _SizeOfAreaHeaderAndNeededAligment, _ProbableMaximalBlockSizeForFastAllocationInPages;
				
	public:
		TDirectSystemNoAccessedMemoryAllocatorWithoutDeallocate(TName Name, const int AllocatedPagesAccess/*=RT_PG_SYSREADWRITE*/, const size_t InitialHeapSizeAtPages=MAX_PAGES_FOR_EXTEND, const size_t ProbableMaximalBlockSizeForFastAllocationInPages=MAX_PAGES_FOR_EXTEND, const size_t ProbableMinimalBlockSizeForAllocation=DROP_AREA_IF_HAVE_FREE_MEMORY_LESS_THAN, const size_t Aligment=ALIGMENT):
			_Name(Name),
			_allocatedAreas(NULL),
			_AllocatedPagesAccess(AllocatedPagesAccess),
			_DropAreaIfThatHaveFreeMemoryLessThan(ProbableMinimalBlockSizeForAllocation),
			_SizeOfAreaHeaderAndNeededAligment(sizeof(*_allocatedAreas)+Align(sizeof(*_allocatedAreas), Aligment)),
			_ProbableMaximalBlockSizeForFastAllocationInPages(ProbableMaximalBlockSizeForFastAllocationInPages)
		{
			char* reservedAddress=NULL;
			const size_t SizeForAllocation=InitialHeapSizeAtPages*RAM_PAGE_SIZE;
			int reservationResult=RTReserveVirtualAddress(&reservedAddress, SizeForAllocation, NULL);
			... for repeat if small memory
			CPL0_ERROR_IF_NOT(reservedAddress!=NULL && reservationResult==RT_MAP_SUCCESS, "Out of memory");
			if(reservedAddress==NULL || reservationResult!=RT_MAP_SUCCESS)
				return NULL:
			SetPagesAccessAfterReservation(reservedAddress, SizeForAllocation, AllocatedPagesAccess);
			void* const BeginOfArea=reservedAddress+_SizeOfAreaHeaderAndNeededAligment;
			void* const EndOfArea=reservedAddress+SizeForAllocation;
			_allocatedAreas=::new(reservedAddress) TIntrusiveList<TAreaHeader>(TAreaHeader(BeginOfArea, EndOfArea, _ProbableMaximalBlockSizeForFastAllocationInPages, AllocatedPagesAccess, Aligment)));
		}

	private:
		class TFindMostSuitableArea
		{
			const size_t _NeadedSize;
			TIntrusiveList<TAreaHeader>* _mostSuitableArea, _beforeMostSuitableArea, _lastObservedArea;
			bool _cantReUseOldObject;

		public:
			TFindMostSuitableArea(const size_t NeadedSize):
			  _NeadedSize(NeadedSize),
				  _mostSuitableArea(NULL),
				  _beforeMostSuitableArea(NULL),
				  _lastObservedArea(NULL),
				  _cantReUseOldObject(false)
			  {}

			  TIntrusiveList<TAreaHeader>* getMostSuitableArea(void) const
			  {
				  return _mostSuitableArea;
			  }

			  TIntrusiveList<TAreaHeader>* getAreaBeforeMostSuitableArea(void) const
			  {
				  return _beforeMostSuitableArea;
			  }

			  TEnumeratorFunctorResult::E operator()(TIntrusiveList<TAreaHeader>* area)
			  {
				  CPL0_ASSERT(!_cantReUseOldObject, "TFindMostSuitableArea must be destructed and constructed again");
				  _cantReUseOldObject=true;
				  CPL0_ASSERT(area!=NULL, "Logical error");
				  if(area->FreeMemoryAtArea()>=_NeadedSize)
					  if(_mostSuitableArea==NULL || area->FreeMemoryAtArea()<_mostSuitableArea->FreeMemoryAtArea())
						  setMostSuitableArea(area);
				  _lastObservedArea=area;
			  }

		private:
			void setMostSuitableArea(TIntrusiveList<TAreaHeader>* area)
			{
				CPL0_ASSERT(area!=NULL, "Logical error");
				_mostSuitableArea=area;
				_beforeMostSuitableArea=_lastObservedArea;
			}
		}

	public:		
		void* allocate(const size_t Size)
		{
			CPL0_ASSERT(_allocatedAreas!=NULL, "Logical error");
			if(_allocatedAreas==NULL)
				return NULL;
			void* result=NULL;
			bool isSizeMoreThanProbableMaximalBlockSizeForFastAllocation=false;
			if(Size>_ProbableMaximalBlockSizeForFastAllocationInPages*RAM_PAGE_SIZE)
			{
				isSizeMoreThanProbableMaximalBlockSizeForFastAllocation=true;
				if(RT_MAP_SUCCESS==RTReserveVirtualAddress(&result, Size, NULL) && result!=NULL)
					return result;
				CPL0_ERROR("Out of memory");
				return NULL;
			}
			TFindMostSuitableArea iterate(Size);
			_allocatedAreas.forEach(iterate);
			TIntrusiveList<TAreaHeader>* MostSuitableArea=iterate.getMostSuitableArea();
			if(MostSuitableArea!=NULL)
			{
				void* const AllocatedBuffer=MostSuitableArea->_head.allocateWithoutExtend(Size);
				CPL0_ASSERT(AllocatedBuffer!=NULL, "Logical error");
				if(MostSuitableArea->_head.FreeMemoryAtArea()<_DropAreaIfThatHaveFreeMemoryLessThan)
					if(iterate.getAreaBeforeMostSuitableArea()!=NULL)
						this->DropAreaAfter(iterate.getAreaBeforeMostSuitableArea());
				return AllocatedBuffer;
			}
			CPL0_ERROR_IF_NOT(!isSizeMoreThanProbableMaximalBlockSizeForFastAllocation, "Out of memory");
			if(isSizeMoreThanProbableMaximalBlockSizeForFastAllocation)
				return NULL;
			/*Тут бы чистку списка почти пустых арей.*/
			
			
			
		}

	private:
		void DropAreaAfter(TIntrusiveList<TAreaHeader>* This)
		{
			CPL0_ASSERT(This!=NULL, "Logical error");
			CPL0_ASSERT(This->_tail!=NULL, "Logical error");
			if(This==NULL)
				return;
			if(This->_tail==NULL)
				return;
			This=This->_tail->_tail;
		}
	}
}

#endif // LowestLevelAllocator_h__
