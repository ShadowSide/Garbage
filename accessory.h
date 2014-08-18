#ifndef ACCESSORY_H
#define ACCESSORY_H

#include "setting.cfg"
#include "on_time_rtos\TargetWrapper.h"
#include "Common\accessory\MainTypes.h"
/*#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) && defined(RTOS_WRAPPER_LOKI_ALLOW))// for >=MSVS2005 and for not Microsoft compilers и при разрешении использовать Loki в RTOS_Wrapper
	#include "TypeManip.h"
#endif*/

#define COMMENT( comment )

COMMENT((
Вспомогательные механизмы программирования.
COMMENT - блок комментариев. Поддерживает вложенность, остается вклеченной подсветка ключевых слов.
Формат вызова:
COMMENT((
 тело комментария
))

ASSERT - утверждение этапа выполнения.
Формат вызова:
ASSERT(условие);
условие проверяется только при если определена директива компиляции DEBUG_CNC.
Если условие ложно программа выдает исключение по ошибке доступа к памяти.
При этом окружение проверки доступно для анализа: переменные, стек, само условие.

SHOULD_NEVER_REACH_HERE - всегда ложное утверждение.

TBitSize<TInterest> - размер в битах представления типа (только для беззнаковых простых типов)
Формат вызова:
enum {SizeOfBool = BitSize<bool>::Result};

TCutUnsigned<SIZE> - контролируемая потеря точности. Используется при десериализации. SIZE - размер в битах итогового значения.
Формат вызова:
unsigned inputValue = getFormSomewhere();
bool myFlag = TCutUnsigned<1>::cast(inputValue); // эквивалентно myFlag = ( (inputValue & 1) != 0 );

LengthOfArray - получение длины массива
Формат вызова:
LengthOfArray(массив);
Бывают случаи, когда для данного типа массива или в данном контексте получить данные о длине массива через LengthOfArray нельзя, тк массив уже преобразован в указатель и информация о длине потеряна.
Во избежания проблемы получения длины массива из-за контекста - следует непосредственно после объявления массива заводить рядом переменную с его длиной, например:
char exampleArray[] = {99, 78, 56};
const size_t LengthOfExampleArray=LengthOfArray(exampleArray);//Далее безбоязнено используем эту переменную для получения длины массива.
В MSVC 6.0 для случая вызова LengthOfArray, когда информация о длине массива не известна, получаем на этапе компиляции следующую ошибку:
/*error C2667: 'LengthOfArray' : none of 2 overload have a best conversion
error C2668: 'LengthOfArray' : ambiguous call to overloaded function*/
В MSVS 2008 получаем ошибку времени компиляции через STATIC_CHECK. Для более лёгкой локализации ошибки можно временно закомментировать STATIC_CHECK и получить ошибку времени выполнения. Дальше через стек вызовов получить информацию о месте ошибочного вызова функции LengthOfArray.
Проблема получения длины массива из-за контескта возможна в случае, когда передают массив в функцию. Для избежания этой проблемы массив в функцию следует передавать по ссылке.

selectType - механизм этапа компиляции для выбора типа.
Формат вызова:
selectType<condition, TOnRelize, TOnFail>::Result
Если condition "истина" результат выражения TOnRelize, иначе TOnFail.

TTypeChecker - механизм анализа типа этапа компиляции
Формат вызова:
TTypeChecker<TUser>::IS_POINTER - истина, если TUser указатель (Внимание! Также возращает истинну, если проверяемый тип - массив не базовых типов(объектов или массивов) и при этом размер всего массива равен 4.)
TTypeChecker<TUser>::IS_ARRAY - истина, если TUser массив (Внимание! Возращает ложь, если проверяемый тип - массив не базовых типов(объектов или массивов) и при этом размер всего массива равен 4.)
TTypeChecker<TUser>::IS_SIMPLE - истина, если TUser базовый тип (int, char, bool, double и т.п.)
TTypeChecker<TUser>::IS_OBJECT - истина, если TUser объект

TConversionAllowed - механизм проверки наследования
Формат вызова:
TConversionAllowed<TDerived, TParent>::Result
истина, если TDerived наследник TParent

STATIC_CHECK - проверка времени компиляции
Формат вызова:
STATIC_CHECK(condition, information_message);
condition - условие, которое может быть вычесленно на этапе компиляции ( например ( sizeof(void*) > sizeof(char) ) )
information_message - часть сообщения об ошибке. Должно подчиняться правилам формирования имен C++.
Например допутимо такое: TElement_must_inherit_from_TReferencedObject
если условие ложно, во время компиляции генерируется ошибка в описании которой присутствует ERROR_information_message

class_FINAL - определение класса, от которого нельзя наследовать и создавать объект.
Статическое наследование все равно возможно.
Формат вызова:
class_FINAL(TFinal) //определение класса TFinal
{};
class TStatic: public TFinal
{
	static void foo(void);
};
TFinal object; // Нормально
TStatic illObject; // Ошибка
void test(void){ TStatic::foo(); } // Нормально

template<class T1, int I1>
class_FINAL(TFinal1,<T1, I1>), public TParent1, public TParent2 //определение класса TFinal1 c 2мя шаблонными параметрами и относледованного от TParent1 и TParent2
{};
//Потомка TFinal1 создать нельзя, хотя TFinal1 cсоздается.
//Шаблонные параметры в MSVC 6.0 как у TFinal1 в class_FINAL не поддерживаются.

))

#define OUT // синтаксический "сахар" - возможность явно указывать выходные параметры. Не влияет ни на что.

#define PRIVATE // то же, что и OUT, но нужен для помечания класса, не предназначенного для использования за пределами пакета.
#define PUBLIC // помечает стуктуры данных, предназначенных для использования за пределами пакета.
#define NONHERRITABLE //класс не предназначен для наследования.

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
	{ // Ошибка при попытке наследования от класса, наследование от которого запрещено явно
		friend T;
		TNonheritableMaker() {}
	public:
		virtual ~TNonheritableMaker(void) {}
	};
#endif
class TNonheritablePlug
{ // Заглушка для сохранения синтаксиса в релизе
};

#if !(defined(_RELEASE)||defined(__GNUC__))//(не RELEASE) и (не gcc)
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

// утверждение
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

#define UN_IMPLEMENTED do{ SHOULD_NEVER_REACH_HERE; throw TUnImplemented(); AssertionFailedUnreachableCodeWarningSuppress(); }while(false)//Вставлять в нереализованные методы.

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

// макрос для объявления функций для TypeChecker, определяющих наличие массива элементов типа Type.
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
			// указатель - любой тип, который можно преобразовать к void*
			IS_POINTER = ( sizeof( pointerCheck(createObject()) )==sizeof(TSmall) )&&(!IS_ARRAY),
			// массив также преобразуется к void*, но, как правило, имеет отличный от указателя размер
			// для случаев когда размер указателя совпадает с размером массива
			// ( например sizeof(void*) == sizeof(char[4]) для большинства процессоров )
			// написаны явные проверки (arrayCheck)
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

// Проверка на один и тот же тип
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
	struct TUnConst				//Получает не константный тип. На MS VS 6.0 не работает.
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

// Проверка выражения, вычисляемого на этапе компиляции
// expression - выражение
// message - часть сообщения об ошибке. Должно подчиняться правилам формирования имен C++.
// Например допутимо такое: TElement_must_inherit_from_TReferencedObject
#define STATIC_CHECK( expression, message ) \
do{ \
	class ERROR_##message {}; \
	(void)( \
		sizeof( ( TCompileTimeError< (expression) != 0 >(ERROR_##message()) ) ) \
		); \
}while(false)

#define CONCAT_MACRO_IMPLEMENT(BEGIN__, END__) BEGIN__##END__


//Правильная конкатенация выражений в макросах:
#define CONCAT_MACRO(BEGIN__, END__) CONCAT_MACRO_IMPLEMENT(BEGIN__,END__)

//Тоже что и STATIC_CHECK (ASSERT времени компиялции) и членн класса:
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
size_t LengthOfArray(TArray& objectArray)//размер массива в элементах(в штуках)
{
	const size_t SizeOfArray=sizeof(TArray);
	const size_t SizeOfItem=sizeof(objectArray[0]);
	STATIC_CHECK(TTypeChecker<TArray>::IS_ARRAY, ThisIsNotArray);
	return SizeOfArray / SizeOfItem;
}

template <class TArrayItem>
size_t LengthOfArray(TArrayItem*& objectArray)//Смотри комментарии к функции в начале файла!!!
{
	//Для данного типа массива или в данном контексте получить данные о длине массива через LengthOfArray нельзя, тк массив уже преобразован в указатель и информация о длине потеряна:
	STATIC_CHECK(false, ForThisArrayCantGetLength);
	SHOULD_NEVER_REACH_HERE;
	return 0;
}

#ifdef _MANAGED
	template <class TArrayItem>
	const size_t LengthOfArray(array< TArrayItem^ >^ objectArray)//Для управляемых массивов получить кол-во элементов в нём можно всегда.
	{
		return objectArray->Length;
	}
#endif

//--------------------------------------------------------------------------------------------------

#define IteratorAtPositionOfArray(Array, Position) (&(Array[Position]))							//Получаем итератор STL из указанной позиции массива.
#define EndIteratorOfArray(Array) (IteratorAtPositionOfArray(Array, LengthOfArray(Array)))		//Получаем итератор конца массива.

//--------------------------------------------------------------------------------------------------

/*#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) && defined(RTOS_WRAPPER_LOKI_ALLOW))// for >=MSVS2005 and for not Microsoft compilers и при разрешении использовать Loki в RTOS_Wrapper
	using Loki::Int2Type;	//Используем из Loki для отсутсвия нарушения ODR.
	using Loki::Type2Type;	//
#else*/
	template <int v>
	struct Int2Type//Не изменять эту структуру. Скопировал из Loki для отсутствия нарушения ODR.
	{
		enum { value = v };
	};

	template <typename T>
	struct Type2Type//Не изменять эту структуру. Скопировал из Loki для отсутствия нарушения ODR.
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
	//Пример использования:
	class TNotCreateableClass
	{
		NOT_CREATEABLE_CLASS(TNotCreateableClass);//Позиция макросов в коде не важна, но рекомендуется в конец.
	};

	class TNotCreateableClassToo : public TNotCreateableClass
	{

	};
	TNotCreateableClass a;//Ошибка компиляции
	TNotCreateableClassToo b;//Ошибка компиляции
	
	class TNotCreateableClass1
	{
		NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED(TNotCreateableClass1);
		public://Вручную востанавливаем видимость полей класса, тк макрос NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED на неё влияет, переводя в private.
	};

	class TCreateableClass : public TNotCreateableClass1
	{

	};
	TNotCreateableClass1 a;//Ошибка компиляции
	TCreateableClass b;//Нет ошибки компиляции. Объект создается. Деструктор класса TNotCreateableClass1 виртуальный, но изменить его реализацию нет возможности.
	//NOT_CREATEABLE_CLASS_BUT_FOR_DERIVED скорее всего не работает в MS VC 6.0
))

////////////////////////////////////////////////////////////////////////

#if (!defined(_MSC_VER)) || (defined(_MSC_VER) && _MSC_VER>=1400)//>=MSVC 2005 или любой другой
//=================================================================================================================================
//Создает класс вида TIsHaveTypedef_TypedefName, который используют так: TIsHaveTypedef_TypedefName<TClassMaybeWithTypedef>::Result
//Созданный класс вида IsHaveTypedef_"TypedefName" определяет наличие указанного typedef в классе, переданном шаблонным параметром.
//Принцип работы - googleIt: SFINAE.
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
//Позволяет создавать (инстанцировать) ленивый вичислитель этапа компиляции в зависмости от условия. boost::eval_if_c требует обязательного вычисленния и инстанцирования обоих веток.
//То есть boost::eval_if вычисляет оба выражения, что должны вычисляться как для ложного, так и истинного условия.
//Данная конкретная реализация вычисляет только ту ветку EvaluateExpressionIfFalse или EvaluateExpressionIfFalse, что требуется для получения результата.
//Я не делал вычислитель для возможности вычисления типов ( пока только целочисленных значений value), тк мне он сейчас мне не нужен, но я думаю без труда каждый желающий сможет создать копию и заменить в данной реализации enum на typedef.
//MAKE_LAZY_CONDITIONAL_VALUE_EVALUATE и MAKE_LAZY_CONDITIONAL_VALUE_NONPARAMETRIZED_EVALUATE - создает некий класс, что позволяет проверить некоторое условие.
//Интерфейс созданного класса вычислителя:
//TNameOfEvaluater<условие, параметры_через_запятую__передаваймые_в_вычисляемую_ветку>::Result 

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
//Пример создания вычислителя TEval0 вида TEval0<Условие>::Result, который вернет (sizeof(char)), если условие положительно, иначе (sizeof(double)):
MAKE_LAZY_CONDITIONAL_VALUE_NONPARAMETRIZED_EVALUATER(TEval0, (sizeof(char)), (sizeof(double)));
//Пример создания вычислителя TEval1 вида TEval1<Условие, bool, T>::Result, который вернет (IsSameType<T, char>::Result), если условие положительно, иначе (sizeof(T)):
MAKE_LAZY_CONDITIONAL_VALUE_EVALUATER(TEval1, PARAMETERS_WITH_TYPES(int a, class T), PARAMETERS_WITHOUT_TYPES(a, T), (IsSameType<T, char>::Result), (sizeof(T)+a));

//Пример использования созданного вычислителя:
enum
{ 
	e1 = TEval0<true>::Result,
	e2 = TEval1<true, true, char>::Result,
	e3 = TEval1<false, false, char>::Result
};
))
//=================================================================================================================================
#endif
//Такое размещение гарантирует, что созданный таким образом статический объект в
//локальной функции не станет регистрировать деструктор вызовом atexit.
//Мы конечно взломали atexit и поэтому эти макросы не особо нужны,
//тк это теперь не страшно, в тех ветках, где происходит взлом стандартной библиотеки,
//через patchCRT (CRT Happy).
COMMENT((
	//Пример использования:
	STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_переменной);
	STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_переменной)(параметры конструктора);
	STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_константы);
	STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_константы)(параметры конструктора);
	//Притом вот так нельзя:
	if(condition)
		STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_переменной);
	//тк макрос STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT разворачивается в 2 строчки.
	//Зато так можно:
	if(condition)
	{
		STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TClass, имя_переменной);
	}
 ))
#define STATIC_PLACEMENT_WITHOUT_CALL_ATEXIT(TType, NewInstance)					\
	static char NewInstance##Buffer__[sizeof(TType)+1];								\
	static TType& NewInstance=*::new(NewInstance##Buffer__) TType

#define STATIC_CONST_PLACEMENT_WITHOUT_CALL_ATEXIT(TType, NewInstance)				\
	static char NewInstance##Buffer__[sizeof(TType)+1];								\
	static const TType& NewInstance=*::new(NewInstance##Buffer__) TType


//////////////////////////////////////////////////////////////////////////
//OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH следует использовать взамен typename при определении статических членов классов, в которых используются шаблоны.
//В старших версиях компилятора Microsoft - typename нужен, в младших, например в MSVC6.0 - нет.
//--------------------------------------------------------------------------------------------------
#ifndef _MSC_VER//for not Microsoft compilers
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH typename
#elif _MSC_VER>=1400// for >=MSVS2005
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH typename
#else//for <MSVS2005
	#define OUTSIDE_TEMPLATE_DECLARATION_TYPENAME_PATCH
#endif

//--------------------------------------------------------------------------------------------------
//FULL_INLINE - Для максимального "заинлайнивания" встроеных функций:
#ifndef _MSC_VER
#define FULL_INLINE inline
#elif _MSC_VER>=1200 //>=MS_VC6.0
#define FULL_INLINE __forceinline//Компилятор Microsoft.
#else
#define FULL_INLINE inline
#endif//Для очень маленьких функций повышается быстродействие.

#else
	#include "assertion.h"
#endif

#endif
