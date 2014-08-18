#ifndef ACCESSORY_H
#define ACCESSORY_H

#include "setting.cfg"
#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\MainTypes.h"
/*#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) && defined(RTOS_WRAPPER_LOKI_ALLOW))// for >=MSVS2005 and for not Microsoft compilers � ��� ���������� ������������ Loki � RTOS_Wrapper
	#include "TypeManip.h"
#endif*/

#define COMMENT( comment )

COMMENT((
��������������� ��������� ����������������.
COMMENT - ���� ������������. ������������ �����������, �������� ���������� ��������� �������� ����.
������ ������:
COMMENT((
 ���� �����������
))

ASSERT - ����������� ����� ����������.
������ ������:
ASSERT(�������);
������� ����������� ������ ��� ���� ���������� ��������� ���������� DEBUG_CNC.
���� ������� ����� ��������� ������ ���������� �� ������ ������� � ������.
��� ���� ��������� �������� �������� ��� �������: ����������, ����, ���� �������.

SHOULD_NEVER_REACH_HERE - ������ ������ �����������.

TBitSize<TInterest> - ������ � ����� ������������� ���� (������ ��� ����������� ������� �����)
������ ������:
enum {SizeOfBool = BitSize<bool>::Result};

TCutUnsigned<SIZE> - �������������� ������ ��������. ������������ ��� ��������������. SIZE - ������ � ����� ��������� ��������.
������ ������:
unsigned inputValue = getFormSomewhere();
bool myFlag = TCutUnsigned<1>::cast(inputValue); // ������������ myFlag = ( (inputValue & 1) != 0 );

LengthOfArray - ��������� ����� �������
������ ������:
LengthOfArray(������);
������ ������, ����� ��� ������� ���� ������� ��� � ������ ��������� �������� ������ � ����� ������� ����� LengthOfArray ������, �� ������ ��� ������������ � ��������� � ���������� � ����� ��������.
�� ��������� �������� ��������� ����� ������� ��-�� ��������� - ������� ��������������� ����� ���������� ������� �������� ����� ���������� � ��� ������, ��������:
char exampleArray[] = {99, 78, 56};
const size_t LengthOfExampleArray=LengthOfArray(exampleArray);//����� ����������� ���������� ��� ���������� ��� ��������� ����� �������.
� MSVC 6.0 ��� ������ ������ LengthOfArray, ����� ���������� � ����� ������� �� ��������, �������� �� ����� ���������� ��������� ������:
/*error C2667: 'LengthOfArray' : none of 2 overload have a best conversion
error C2668: 'LengthOfArray' : ambiguous call to overloaded function*/
� MSVS 2008 �������� ������ ������� ���������� ����� STATIC_CHECK. ��� ����� ����� ����������� ������ ����� �������� ���������������� STATIC_CHECK � �������� ������ ������� ����������. ������ ����� ���� ������� �������� ���������� � ����� ���������� ������ ������� LengthOfArray.
�������� ��������� ����� ������� ��-�� ��������� �������� � ������, ����� �������� ������ � �������. ��� ��������� ���� �������� ������ � ������� ������� ���������� �� ������.

selectType - �������� ����� ���������� ��� ������ ����.
������ ������:
selectType<condition, TOnRelize, TOnFail>::Result
���� condition "������" ��������� ��������� TOnRelize, ����� TOnFail.

TTypeChecker - �������� ������� ���� ����� ����������
������ ������:
TTypeChecker<TUser>::IS_POINTER - ������, ���� TUser ��������� (��������! ����� ��������� �������, ���� ����������� ��� - ������ �� ������� �����(�������� ��� ��������) � ��� ���� ������ ����� ������� ����� 4.)
TTypeChecker<TUser>::IS_ARRAY - ������, ���� TUser ������ (��������! ��������� ����, ���� ����������� ��� - ������ �� ������� �����(�������� ��� ��������) � ��� ���� ������ ����� ������� ����� 4.)
TTypeChecker<TUser>::IS_SIMPLE - ������, ���� TUser ������� ��� (int, char, bool, double � �.�.)
TTypeChecker<TUser>::IS_OBJECT - ������, ���� TUser ������

TConversionAllowed - �������� �������� ������������
������ ������:
TConversionAllowed<TDerived, TParent>::Result
������, ���� TDerived ��������� TParent

STATIC_CHECK - �������� ������� ����������
������ ������:
STATIC_CHECK(condition, information_message);
condition - �������, ������� ����� ���� ���������� �� ����� ���������� ( �������� ( sizeof(void*) > sizeof(char) ) )
information_message - ����� ��������� �� ������. ������ ����������� �������� ������������ ���� C++.
�������� �������� �����: TElement_must_inherit_from_TReferencedObject
���� ������� �����, �� ����� ���������� ������������ ������ � �������� ������� ������������ ERROR_information_message

class_FINAL - ����������� ������, �� �������� ������ ����������� � ��������� ������.
����������� ������������ ��� ����� ��������.
������ ������:
class_FINAL(TFinal) //����������� ������ TFinal
{};
class TStatic: public TFinal
{
	static void foo(void);
};
TFinal object; // ���������
TStatic illObject; // ������
void test(void){ TStatic::foo(); } // ���������

template<class T1, int I1>
class_FINAL(TFinal1,<T1, I1>), public TParent1, public TParent2 //����������� ������ TFinal1 c 2�� ���������� ����������� � ���������������� �� TParent1 � TParent2
{};
//������� TFinal1 ������� ������, ���� TFinal1 c���������.
//��������� ��������� � MSVC 6.0 ��� � TFinal1 � class_FINAL �� ��������������.

))

#define OUT // �������������� "�����" - ����������� ���� ��������� �������� ���������. �� ������ �� �� ���.

#define PRIVATE // �� ��, ��� � OUT, �� ����� ��� ��������� ������, �� ���������������� ��� ������������� �� ��������� ������.
#define PUBLIC // �������� �������� ������, ��������������� ��� ������������� �� ��������� ������.
#define NONHERRITABLE //����� �� ������������ ��� ������������.

#ifndef __PARADIGM__
template <typename TInterest> struct TBitSize {};
template <> struct TBitSize<bool> { enum {Result = 1};};
template <> struct TBitSize<byte> { enum {Result = 8};};
template <> struct TBitSize<WORD> { enum {Result = 16};};
template <> struct TBitSize<unsigned int> { enum {Result = 32};};
template <> struct TBitSize<unsigned long> { enum {Result = 32};};

template<unsigned BYTE_SIZE> struct TCutInternal;
template<> struct TCutInternal<1> { typedef INT8U TResult;};
template<> struct TCutInternal<2> { typedef INT16U TResult;};
template<> struct TCutInternal<3> { typedef INT32U TResult;};
template<> struct TCutInternal<4> { typedef INT32U TResult;};

template<unsigned BIT_SIZE>
struct TCutUnsigned
{
	typedef typename TCutInternal< (BIT_SIZE-1)/8+1 >::TResult TResult;
	static TResult cast(unsigned value){return static_cast<TResult>(value);}
};
template<>
struct TCutUnsigned<1>
{
	typedef bool TResult;
	static TResult cast(unsigned value){return (value!=0);}
};
#endif

#ifndef __GNUC__    //if not GCC
	template<class T>
	class TNonheritableMaker
	{ // ������ ��� ������� ������������ �� ������, ������������ �� �������� ��������� ����
		friend T;
		TNonheritableMaker() {}
	public:
		virtual ~TNonheritableMaker(void) {}
	};
#endif
class TNonheritablePlug
{ // �������� ��� ���������� ���������� � ������
};

#if !(defined(_RELEASE)||defined(__GNUC__))//(�� RELEASE) � (�� gcc)
	#if (defined(_MSC_VER) && (_MSC_VER>=1400)) //>=MSVS2005
		#define class_FINAL(className,...) class className: virtual public TNonheritableMaker<className##__VA_ARGS__>
	#else
		#define class_FINAL(className) class className: virtual public TNonheritableMaker<className>
	#endif
#else
	#if (defined(_MSC_VER) && (_MSC_VER>=1400)) //>=MSVS2005
		#define class_FINAL(className,...) class className: public TNonheritablePlug
	#else
		#define class_FINAL(className) class className: public TNonheritablePlug
	#endif
#endif

#ifndef __PARADIGM__

#define NOTHROW_FUNCTION __declspec(nothrow)
#if (_MSC_VER>=1400) && defined(_MSC_VER) //for >=MSVS2005
	#define UNIQUE_POINTED __declspec(restrict)
#else
	#define UNIQUE_POINTED
#endif

// �����������
#ifdef ENABLE_ASSERTION
#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _MSC_VER //if >= MS VC
	__declspec(noreturn) void AssertionFailedUnreachableCodeWarningSuppress(void);
#elif defined(__GNUC__) //if GCC
	void AssertionFailedUnreachableCodeWarningSuppress(void) __attribute__((noreturn));
#else
	void AssertionFailedUnreachableCodeWarningSuppress(void);
#endif

	//For "do{ ... }while(false)" see C++ Idiom: Multi-statement Macro
	#define ASSERT( condition )			                     \
	do{									                     \
		if ( !(condition) )				                     \
		{					 			                     \
			*((int*)0) = *((int*)1);	                     \
			AssertionFailedUnreachableCodeWarningSuppress(); \
		}								                     \
	}while(false)
	#define SHOULD_NEVER_REACH_HERE do{ASSERT(false); AssertionFailedUnreachableCodeWarningSuppress();}while(false)
#else
	#define ASSERT( condition ) do{}while(false)
	#define SHOULD_NEVER_REACH_HERE do{}while(false)
#endif

struct TUnImplemented {};

#define UN_IMPLEMENTED do{ SHOULD_NEVER_REACH_HERE; throw TUnImplemented(); AssertionFailedUnreachableCodeWarningSuppress(); }while(false)//��������� � ��������������� ������.

#define COMMENT( comment )

#ifdef _MSC_VER
template <bool condition, class onRelize, class onFail>
struct selectType
{
	template <bool>
	struct _selecter
	{
		typedef onRelize Result;
	};

	template<>
	struct _selecter<false>
	{
		typedef onFail Result;
	};

	typedef typename _selecter<condition>::Result Result;
};
#else
	template <bool condition, class onRelize, class onFail>
	struct selectType
	{
		typedef onFail Result;
	};

	template <class onRelize, class onFail>
	struct selectType<true, onRelize, onFail>
	{
		typedef onRelize Result;
	};
#endif

// ������ ��� ���������� ������� ��� TypeChecker, ������������ ������� ������� ��������� ���� Type.
#define SMALL_RESULT_IF_ARRAY_OF( Type, TYPE_NAME ) \
	typedef Type TArrayOf_##TYPE_NAME[sizeof(void*)/sizeof(Type)]; \
	static TSmall __cdecl arrayCheck( TArrayOf_##TYPE_NAME & ); \
	static TSmall __cdecl arrayCheck( const TArrayOf_##TYPE_NAME & ); \
	static TSmall __cdecl arrayCheck( volatile TArrayOf_##TYPE_NAME & ); \
	static TSmall __cdecl arrayCheck( volatile const TArrayOf_##TYPE_NAME & )

template <class TInterest>
class TTypeChecker
{
	typedef char TSmall;
	typedef struct
	{
		char _unnamed[2];
	} TBig;
	static TSmall __cdecl pointerCheck(void*);
	static TSmall __cdecl pointerCheck(const void*);
	static TSmall __cdecl pointerCheck(volatile void*);
	static TSmall __cdecl pointerCheck(volatile const void*);

	static TBig __cdecl pointerCheck(...);

	SMALL_RESULT_IF_ARRAY_OF(bool, bool);

	SMALL_RESULT_IF_ARRAY_OF(char, char);
	SMALL_RESULT_IF_ARRAY_OF(signed char, signed_char);
	SMALL_RESULT_IF_ARRAY_OF(unsigned char, unsigned_char);

	SMALL_RESULT_IF_ARRAY_OF(signed short, signed_short);
	SMALL_RESULT_IF_ARRAY_OF(unsigned short, unsigned_short);

	SMALL_RESULT_IF_ARRAY_OF(signed long, signed_long);
	SMALL_RESULT_IF_ARRAY_OF(unsigned long, unsigned_long);

	SMALL_RESULT_IF_ARRAY_OF(signed int, signed_int);
	SMALL_RESULT_IF_ARRAY_OF(unsigned int, unsigned_int);

	SMALL_RESULT_IF_ARRAY_OF(float, float);

	static TBig __cdecl arrayCheck(...);

	static TInterest& createObject(void);
public:
	enum {
			IS_ARRAY = ( ( (sizeof( pointerCheck(createObject()) )==sizeof(TSmall)) &&
						(sizeof( void* ) != sizeof(TInterest)) ) ||
							(sizeof( arrayCheck(createObject()) )==sizeof(TSmall)) ),
			// ��������� - ����� ���, ������� ����� ������������� � void*
			IS_POINTER = ( sizeof( pointerCheck(createObject()) )==sizeof(TSmall) )&&(!IS_ARRAY),
			// ������ ����� ������������� � void*, ��, ��� �������, ����� �������� �� ��������� ������
			// ��� ������� ����� ������ ��������� ��������� � �������� �������
			// ( �������� sizeof(void*) == sizeof(char[4]) ��� ����������� ����������� )
			// �������� ����� �������� (arrayCheck)
			IS_SIMPLE = 0,
			IS_FLOAT = 0
		};
	enum {
			IS_OBJECT = !(IS_SIMPLE || IS_POINTER || IS_ARRAY)
		};
};

#define SIMPLE_TYPE( Type ) \
template <> \
class TTypeChecker<Type> \
{ \
public: \
	enum {	IS_POINTER = 0, \
			IS_ARRAY = 0, \
			IS_SIMPLE = 1, \
			IS_OBJECT = 0, \
			IS_FLOAT = 0 \
		}; \
}

#define FLOAT_TYPE( Type ) \
template <> \
class TTypeChecker<Type> \
{ \
public: \
	enum {	IS_POINTER = 0, \
	IS_ARRAY = 0, \
	IS_SIMPLE = 1, \
	IS_OBJECT = 0, \
	IS_FLOAT = 1 \
}; \
}

SIMPLE_TYPE(bool);
SIMPLE_TYPE(volatile bool);

SIMPLE_TYPE(char);
SIMPLE_TYPE(volatile char);

SIMPLE_TYPE(signed char);
SIMPLE_TYPE(volatile signed char);

SIMPLE_TYPE(unsigned char);
SIMPLE_TYPE(volatile unsigned char);

SIMPLE_TYPE(signed short);
SIMPLE_TYPE(volatile signed short);

SIMPLE_TYPE(unsigned short);
SIMPLE_TYPE(volatile unsigned short);

SIMPLE_TYPE(signed long);
SIMPLE_TYPE(volatile unsigned long);

SIMPLE_TYPE(signed int);
SIMPLE_TYPE(volatile signed int);

SIMPLE_TYPE(unsigned int);
SIMPLE_TYPE(volatile unsigned int);

SIMPLE_TYPE(unsigned long);
SIMPLE_TYPE(volatile signed long);

FLOAT_TYPE(float);
FLOAT_TYPE(volatile float);

FLOAT_TYPE(double);
FLOAT_TYPE(volatile double);

FLOAT_TYPE(long double);
FLOAT_TYPE(volatile long double);


template <class TDerived, class TParent>
class TConversionAllowed
{
	typedef typename selectType<
		( (TTypeChecker<TParent>::IS_POINTER) || (TTypeChecker<TParent>::IS_SIMPLE) ),
		TParent, TParent& >::Result TParentArgument;
	typedef char TSmall;
	typedef struct
	{
		char _unnamed[2];
	} TBig;
	static TSmall __cdecl test(TParentArgument);
	static TBig __cdecl test(...);
	static TDerived& createDerived(void);
public:
	enum { Result = ( sizeof( test(createDerived()) )==sizeof(TSmall) ) };
};

// �������� �� ���� � ��� �� ���
#ifdef _MSC_VER
template <class TVerified, class TModel>
class IsSameType
{
private:
	template <class T>
	struct TChecker
	{
		enum { Result = false };
	};
	template <>
	struct TChecker<TModel>
	{
		enum { Result = true };
	};
public:
	enum { Result = TChecker<TVerified>::Result };
};
#else
	template <typename T, typename U>
	struct IsSameType
	{
		enum { Result = false };
	};

	template <typename T>
	struct IsSameType<T,T>
	{
		enum { Result = true };
	};
#endif

// -----------------------------
#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) )// for >=MSVS2005 and for not Microsoft compilers

	#ifdef RTOS_WRAPPER_BOOST_ALLOW//if allowed boost
		#include<boost\type_traits\is_class.hpp>
	#endif

	template <class TType>
	struct TUnConst				//�������� �� ����������� ���. �� MS VS 6.0 �� ��������.
	{
		typedef TType TResult;
		enum { IsConst = 0 };
	};

	template <class TType>
	struct TUnConst<const TType>
	{
		typedef TType TResult;
		enum { IsConst = 1 };
	};

	template <class TType>
	struct TUnConst<const TType&>
	{
		typedef TType& TResult;
		enum { IsConst = 1 };
	};
#endif
// -----------------------------
template <bool Condition>
struct TCompileTimeError;

#ifndef __GNUC__
	#pragma warning (disable : 4096)
#endif
template <>
struct TCompileTimeError<true>
{
	TCompileTimeError(...);
};
#ifndef __GNUC__
	#pragma warning (default : 4096)
#endif

template <>
struct TCompileTimeError<false>
{};

// �������� ���������, ������������ �� ����� ����������
// expression - ���������
// message - ����� ��������� �� ������. ������ ����������� �������� ������������ ���� C++.
// �������� �������� �����: TElement_must_inherit_from_TReferencedObject
#define STATIC_CHECK( expression, message ) \
do{ \
	class ERROR_##message {}; \
	(void)( \
		sizeof( ( TCompileTimeError< (expression) != 0 >(ERROR_##message()) ) ) \
		); \
}while(false)

#define CONCAT_MACRO_IMPLEMENT(BEGIN__, END__) BEGIN__##END__


//���������� ������������ ��������� � ��������:
#define CONCAT_MACRO(BEGIN__, END__) CONCAT_MACRO_IMPLEMENT(BEGIN__,END__)

//���� ��� � STATIC_CHECK (ASSERT ������� ����������) � ����� ������:
#define MEMBER_STATIC_CHECK(expression, message) char CONCAT_MACRO(CONCAT_MACRO(CONCAT_MACRO(CompileTimeError_atLine, __LINE__), _with_error_), message)[1/((expression)!=0?1:0)]

template <class TType1, class TType2>
struct TSymmetricConversable
{
	enum
	{
		value = TConversionAllowed<TType1, TType2>::Result || TConversionAllowed<TType2, TType1>::Result
	};
};


template <class TTestedToLessDerived, class TAnother>
struct TIsLessDerived
{
private:
	typedef TIsLessDerived<TTestedToLessDerived, TAnother> TMe;
	typedef char TSmall;
	struct TBig
	{
		char _unnamed[2];
	};

	typedef TSmall	TTestedIsLessDerivedThanAnother;
	typedef TBig	TTestedIsMoreOrEqualDerivedThanAnother;

	static TTestedIsMoreOrEqualDerivedThanAnother __cdecl test(const TAnother&);
	static TTestedIsLessDerivedThanAnother __cdecl test(...);
	static const TTestedToLessDerived& createTestedToLessDerived(void);

public:
	enum
	{
		value = ( sizeof( TMe::test(TMe::createTestedToLessDerived()) )==sizeof(TTestedIsLessDerivedThanAnother) )
	};	

private:
	MEMBER_STATIC_CHECK((TSymmetricConversable<TTestedToLessDerived, TAnother>::value), assertionComparableClassesNotDerived);
#if(( (_MSC_VER>=1400) || (!defined(_MSC_VER)) ) && defined(RTOS_WRAPPER_BOOST_ALLOW))// for >=MSVS2005 and for not Microsoft compilers, if allowed boost
	MEMBER_STATIC_CHECK((boost::is_class<TTestedToLessDerived>::value), TTestedToLessDerivedIsNotClass);
	MEMBER_STATIC_CHECK((boost::is_class<TAnother>::value), TAnotherIsNotClass);
#endif
};

template <class TType1, class TType2>
struct TSelectLessDerived
{
	typedef typename selectType<TIsLessDerived<TType1, TType2>::value, TType1, TType2>::Result type;
};


template <class TArray>
size_t LengthOfArray(TArray& objectArray)//������ ������� � ���������(� ������)
{
	const size_t SizeOfArray=sizeof(TArray);
	const size_t SizeOfItem=sizeof(objectArray[0]);
	STATIC_CHECK(TTypeChecker<TArray>::IS_ARRAY, ThisIsNotArray);
	return SizeOfArray / SizeOfItem;
}

template <class TArrayItem>
size_t LengthOfArray(TArrayItem*& objectArray)//������ ����������� � ������� � ������ �����!!!
{
	//��� ������� ���� ������� ��� � ������ ��������� �������� ������ � ����� ������� ����� LengthOfArray ������, �� ������ ��� ������������ � ��������� � ���������� � ����� ��������:
	STATIC_CHECK(false, ForThisArrayCantGetLength);
	SHOULD_NEVER_REACH_HERE;
	return 0;
}

#ifdef _MANAGED
	template <class TArrayItem>
	const size_t LengthOfArray(array< TArrayItem^ >^ objectArray)//��� ����������� �������� �������� ���-�� ��������� � �� ����� ������.
	{
		return objectArray->Length;
	}
#endif

//--------------------------------------------------------------------------------------------------

#define IteratorAtPositionOfArray(Array, Position) (&(Array[Position]))							//�������� �������� STL �� ��������� ������� �������.
#define EndIteratorOfArray(Array) (IteratorAtPositionOfArray(Array, LengthOfArray(Array)))		//�������� �������� ����� �������.

//--------------------------------------------------------------------------------------------------

/*#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) && defined(RTOS_WRAPPER_LOKI_ALLOW))// for >=MSVS2005 and for not Microsoft compilers � ��� ���������� ������������ Loki � RTOS_Wrapper
	using Loki::Int2Type;	//���������� �� Loki ��� ��������� ��������� ODR.
	using Loki::Type2Type;	//
#else*/
	template <int v>
	struct Int2Type//�� �������� ��� ���������. ���������� �� Loki ��� ���������� ��������� ODR.
	{
		enum { value = v };
	};

	template <typename T>
	struct Type2Type//�� �������� ��� ���������. ���������� �� Loki ��� ���������� ��������� ODR.
	{
		typedef T OriginalType;
	};
//#endif

//--------------------------------------------------------------------------------------------------

namespace NReference_to
{
	template <typename Type>
	class reference_to
	{
		public:
			reference_to(Type& reference) : _reference(reference){}

			operator Type&()
			{
				return _reference;
			}

		private:
			Type& _reference;
	};

	template <class Type>
	reference_to<Type> ref(Type& r)
	{
		return reference_to<Type>(r);
	}
}

//////////////////////////////////////////////////////////////////////////

template<class TFrom, class TTo>
TTo optimizedDynamicCastIfWithoutCasting(TFrom from, Type2Type<TTo>)
{
	return dynamic_cast<TTo>(from);
}

template<class TFrom>
TFrom optimizedDynamicCastIfWithoutCasting(TFrom from, Type2Type<TFrom>)
{
	return from;
}

/////////////////////////////////////////////////////////////////////////
#define NOT_CREATEABLE_CLASS(CurrentClassName) virtual ~CurrentClassName(void) = 0
#define NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED(CurrentClassName) public: NOT_CREATEABLE_CLASS(CurrentClassName) {} private: 

COMMENT((
	//������ �������������:
	class TNotCreateableClass
	{
		NOT_CREATEABLE_CLASS(TNotCreateableClass);//������� �������� � ���� �� �����, �� ������������� � �����.
	};

	class TNotCreateableClassToo : public TNotCreateableClass
	{

	};
	TNotCreateableClass a;//������ ����������
	TNotCreateableClassToo b;//������ ����������
	
	class TNotCreateableClass1
	{
		NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED(TNotCreateableClass1);
		public://������� �������������� ��������� ����� ������, �� ������ NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED �� �� ������, �������� � private.
	};

	class TCreateableClass : public TNotCreateableClass1
	{

	};
	TNotCreateableClass1 a;//������ ����������
	TCreateableClass b;//��� ������ ����������. ������ ���������. ���������� ������ TNotCreateableClass1 �����������, �� �������� ��� ���������� ��� �����������.
	//NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED ������ ����� �� �������� � MS VC 6.0
))

////////////////////////////////////////////////////////////////////////

#if (!defined(_MSC_VER)) || (defined(_MSC_VER) && _MSC_VER>=1400)//>=MSVC 2005 ��� ����� ������
//=================================================================================================================================
//������� ����� ���� TIsHaveTypedef_TypedefName, ������� ���������� ���: TIsHaveTypedef_TypedefName<TClassMaybeWithTypedef>::Result
//��������� ����� ���� IsHaveTypedef_"TypedefName" ���������� ������� ���������� typedef � ������, ���������� ��������� ����������.
//������� ������ - googleIt: SFINAE.
#define MAKE_TYPEDEF_CHECKER(TypedefName)															\
template<class TClassMaybeWithTypedef>                                                              \
struct TIsHaveTypedef_##TypedefName                                                                 \
{                                                                                                   \
private:                                                                                            \
	typedef char TFalseType;                                                                        \
	typedef char TTrueType[2];                                                                      \
	template <class T>                                                                              \
	static TTrueType& isHaveTypedef(typename T::TypedefName*);                                      \
	template <typename>                                                                             \
	static TFalseType& isHaveTypedef(...);                                                          \
public:                                                                                             \
	enum {Result=sizeof(isHaveTypedef<TClassMaybeWithTypedef>(NULL))==sizeof(TTrueType)};			\
}
//=================================================================================================================================
//��������� ��������� (��������������) ������� ����������� ����� ���������� � ���������� �� �������. boost::eval_if_c ������� ������������� ����������� � ��������������� ����� �����.
//�� ���� boost::eval_if ��������� ��� ���������, ��� ������ ����������� ��� ��� �������, ��� � ��������� �������.
//������ ���������� ���������� ��������� ������ �� ����� EvaluateExpressionIfFalse ��� EvaluateExpressionIfFalse, ��� ��������� ��� ��������� ����������.
//� �� ����� ����������� ��� ����������� ���������� ����� ( ���� ������ ������������� �������� value), �� ��� �� ������ ��� �� �����, �� � ����� ��� ����� ������ �������� ������ ������� ����� � �������� � ������ ���������� enum �� typedef.
//MAKE_LAZY_CONDITIONAL_VALUE_EVALUATE � MAKE_LAZY_CONDITIONAL_VALUE_NONPARAMETRIZED_EVALUATE - ������� ����� �����, ��� ��������� ��������� ��������� �������.
//��������� ���������� ������ �����������:
//TNameOfEvaluater<�������, ���������_�����_�������__������������_�_�����������_�����>::Result 

#if defined(_MSC_VER) && _MSC_VER>=1400//>=MSVC 2005

#define PARAMETERS_WITH_TYPES(...) __VA_ARGS__
#define PARAMETERS_WITHOUT_TYPES(...) __VA_ARGS__

#endif

#define MAKE_LAZY_CONDITIONAL_VALUE_EVALUATER(TNameOfEvaluater, ParametersForEvaluationWithTypes, ParametersForEvaluationWithoutTypes, EvaluateExpressionIfTrue, EvaluateExpressionIfFalse)	\
template<bool Condition, ParametersForEvaluationWithTypes>																						\
struct TNameOfEvaluater																															\
{																																				\
	enum {Result=(EvaluateExpressionIfTrue)};																									\
};																																				\
template<ParametersForEvaluationWithTypes>																										\
struct TNameOfEvaluater<false, ParametersForEvaluationWithoutTypes>																				\
{																																				\
	enum {Result=(EvaluateExpressionIfFalse)};																									\
}

#define MAKE_LAZY_CONDITIONAL_VALUE_NONPARAMETRIZED_EVALUATER(TNameOfEvaluater, EvaluateExpressionIfTrue, EvaluateExpressionIfFalse)			\
template<bool Condition>																														\
struct TNameOfEvaluater																															\
{																																				\
	enum {Result=(EvaluateExpressionIfTrue)};																									\
};																																				\
template<>																																		\
struct TNameOfEvaluater<false>																													\
{																																				\
	enum {Result=(EvaluateExpressionIfFalse)};																									\
}

COMMENT((
//������ �������� ����������� TEval0 ���� TEval0<�������>::Result, ������� ������ (sizeof(char)), ���� ������� ������������, ����� (sizeof(double)):
MAKE_LAZY_CONDITIONAL_VALUE_NONPARAMETRIZED_EVALUATER(TEval0, (sizeof(char)), (sizeof(double)));
//������ �������� ����������� TEval1 ���� TEval1<�������, bool, T>::Result, ������� ������ (IsSameType<T, char>::Result), ���� ������� ������������, ����� (sizeof(T)):
MAKE_LAZY_CONDITIONAL_VALUE_EVALUATER(TEval1, PARAMETERS_WITH_TYPES(int a, class T), PARAMETERS_WITHOUT_TYPES(a, T), (IsSameType<T, char>::Result), (sizeof(T)+a));

//������ ������������� ���������� �����������:
enum
{ 
	e1 = TEval0<true>::Result,
	e2 = TEval1<true, true, char>::Result,
	e3 = TEval1<false, false, char>::Result
};
))
//=================================================================================================================================
#endif
//����� ���������� �����������, ��� ��������� ����� ������� ����������� ������ �
//��������� ������� �� ������ �������������� ���������� ������� atexit.
//�� ������� �������� atexit � ������� ��� ������� �� ����� �����,
//�� ��� ������ �� �������, � ��� ������, ��� ���������� ����� ����������� ����������,
//����� patchCRT (CRT Happy).
COMMENT((
	//������ �������������:
	STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_����������);
	STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_����������)(��������� ������������);
	STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_���������);
	STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_���������)(��������� ������������);
	//������ ��� ��� ������:
	if(condition)
		STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_����������);
	//�� ������ STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT ��������������� � 2 �������.
	//���� ��� �����:
	if(condition)
	{
		STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, ���_����������);
	}
 ))
#define STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TType, NewInstance)					\
	static char NewInstance##Buffer__[sizeof(TType)+1];								\
	static TType& NewInstance=*::new(NewInstance##Buffer__) TType

#define STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TType, NewInstance)				\
	static char NewInstance##Buffer__[sizeof(TType)+1];								\
	static const TType& NewInstance=*::new(NewInstance##Buffer__) TType


//////////////////////////////////////////////////////////////////////////
//OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH ������� ������������ ������ typename ��� ����������� ����������� ������ �������, � ������� ������������ �������.
//� ������� ������� ����������� Microsoft - typename �����, � �������, �������� � MSVC6.0 - ���.
//--------------------------------------------------------------------------------------------------
#ifndef _MSC_VER//for not Microsoft compilers
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH typename
#elif _MSC_VER>=1400// for >=MSVS2005
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH typename
#else//for <MSVS2005
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH
#endif

//--------------------------------------------------------------------------------------------------
//FULL_INLINE - ��� ������������� "��������������" ��������� �������:
#ifndef _MSC_VER
#define FULL_INLINE inline
#elif _MSC_VER>=1200 //>=MS_VC6.0
#define FULL_INLINE __forceinline//���������� Microsoft.
#else
#define FULL_INLINE inline
#endif//��� ����� ��������� ������� ���������� ��������������.

#else
	#include "assertion.h"
#endif

#endif
