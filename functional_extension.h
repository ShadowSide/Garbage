#ifndef functional_extension_h__
#define functional_extension_h__
//Расширение стандартной библиотеки <functional>.

#include <Common\accessory\accessory.h>
#include <functional>
#include <algorithm>
#include <Common\SystemTime.h>
#include <utility>
#include <map>
#include <Common\Enumerator\Enumerator.h>
#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) )
#ifdef RTOS_WRAPPER_BOOST_ALLOW
	#include <boost\type_traits.hpp>
	#include <boost\mpl\if.hpp>
	#include <boost\ref.hpp>
	#include <boost\preprocessor.hpp>
	#include <boost\type.hpp>
	#include <boost\optional\optional.hpp>
#endif
#endif

//reverse_iterator_patched - в MSVC6.0 не верно определён std::reverse_iterator. Не по стандарту. Следует использовать reverse_iterator_patched взамен std::reverse_iterator.
#if( (_MSC_VER>=1400) || (!defined(_MSC_VER)) )// for >=MSVS2005 and for not Microsoft compilers
	template<class TIterator, class TValueType>
	struct reverse_iterator_patched
	{
		typedef std::reverse_iterator<TIterator> TResult;
	};
#else//for <MSVS2005
	template<class TIterator, class TValueType>
	struct reverse_iterator_patched
	{
		typedef std::reverse_iterator<TIterator, TValueType> TResult;
	};
#endif

//Замена класса std::pointer_to_unary_function, "превращающего" указатель на унарную функцию (один аргумент) в функциональный объект.
//В MSVS 6.0 не работает std::pointer_to_unary_function для функций, возвращающих значение void.
template<class TArgument1>
class TPointerToUnaryFunctionWithVoidResult: public std::unary_function<TArgument1, void>
{
protected:
	typedef void (* const TFunction)(TArgument1 argument1);
	TFunction _Function;
	
public:
	explicit TPointerToUnaryFunctionWithVoidResult(TFunction Function):
		_Function(Function)
	{};
	
	void operator()(TArgument1 argument1) const
	{
		(void)(_Function(argument1));
	};
};

//Замена класса std::pointer_to_binary_function, "превращающего" указатель на бинарную функцию (два аргумента) в функциональный объект.
//В MSVS 6.0 не работает std::pointer_to_binary_function для функций, возвращающих значение void.
template<class TArgument1, class TArgument2>
class TPointerToBinaryFunctionWithVoidResult: public std::binary_function<TArgument1, TArgument2, void>
{
protected:
	typedef void (* const TFunction)(TArgument1 argument1, TArgument2 argument2);
	TFunction _Function;
	
public:
	explicit TPointerToBinaryFunctionWithVoidResult(TFunction Function):
		_Function(Function)
	{};
	
	void operator()(TArgument1 argument1, TArgument2 argument2) const
	{
		(void)(_Function(argument1, argument2));
	};
};

//makePointerToFunctionWithVoidResult - функция генератор для облегчения использования класса TPointerToUnaryFunctionWithVoidResult. Предназначена для автоматического вывода шаблонов.
//Замена функции std::ptr_fun для функций, возвращающих тип void.
template<class TArgument1>
inline TPointerToUnaryFunctionWithVoidResult<TArgument1> makePointerToFunctionWithVoidResult(void (* Functor)(TArgument1))
{	// return TPointerToUnaryFunctionWithVoidResult functor adapter
	return (TPointerToUnaryFunctionWithVoidResult<TArgument1>(Functor));
};

template<class TArgument1, class TResult>
inline std::pointer_to_unary_function<TArgument1, TResult> makePointerToFunction(TResult (__cdecl * Functor)(TArgument1))
{	// return pointer_to_unary_function functor adapter
	return std::ptr_fun(Functor);
};

//makePointerToFunctionWithVoidResult - функция генератор для облегчения использования класса TPointerToBinaryFunctionWithVoidResult. Предназначена для автоматического вывода шаблонов.
//Замена функции std::ptr_fun для функций, возвращающих тип void.
template<class TArgument1, class TArgument2>
inline TPointerToBinaryFunctionWithVoidResult<TArgument1, TArgument2> makePointerToFunctionWithVoidResult(void (* Functor)(TArgument1, TArgument2))
{	// return TPointerToBinaryFunctionWithVoidResult functor adapter
	return (TPointerToBinaryFunctionWithVoidResult<TArgument1, TArgument2>(Functor));
};

template<class TArgument1, class TArgument2, class TResult>
inline std::pointer_to_binary_function<TArgument1, TArgument2, TResult> makePointerToFunction(TResult (__cdecl * Functor)(TArgument1, TArgument2))
{	// return pointer_to_unary_function functor adapter
	return std::ptr_fun(Functor);
};

//Аналог std::binder1st (для бинарных функций), но для "превращения" унарной функции (с одним параметром), в функцию без параметров.
template<class TUnaryFunction>
class TBinderUnaryFunctionsArgument
{	// functor adapter _Func(stored)
private:
	TUnaryFunction _function;	// the functor to apply
	typename TUnaryFunction::argument_type _storedArgument1;	// the operand
	typedef typename TBinderUnaryFunctionsArgument<TUnaryFunction> TMyType;
	
public:
	typedef typename TUnaryFunction::result_type result_type;
	
	TBinderUnaryFunctionsArgument(const TUnaryFunction& storeUnaryFunction, const typename TUnaryFunction::argument_type& storeArgument1):
		_function(storeUnaryFunction),
		_storedArgument1(storeArgument1)
	{	// construct from functor and operand
	};
	
	result_type operator()(void) const
	{	// apply functor to operands
		return (_function(_storedArgument1));
	};
};

//Аналог std::bind1st (bind1st для бинарных функций), но для "превращения" унарной функции (с одним параметром), в функцию без параметров.
//bindUnaryFunctionsArgument - функция генератор для облегчения использования класса TBinderUnaryFunctionsArgument. Предназначена для автоматического вывода шаблонов.
template<class TUnaryFunction, class TArgument1>
inline TBinderUnaryFunctionsArgument<TUnaryFunction> bindUnaryFunctionsArgument(const TUnaryFunction& unaryFunction, const TArgument1& storeArgument1)
{	// return a TBinderUnaryFunctionsArgument functor adapter
	return (TBinderUnaryFunctionsArgument<TUnaryFunction>(unaryFunction, storeArgument1));
};

//Аналог TBinderUnaryFunctionsArgument, но для функции, возвращающий тип void.
template<class TUnaryFunction>
class TBinderUnaryFunctionsArgumentWithVoidResult
{	// functor adapter _Func(stored)
private:
	TUnaryFunction _function;	// the functor to apply
	typename typename TUnaryFunction::argument_type _storedArgument1;	// the operand
	
public:
	typedef typename void result_type;
	typedef typename TBinderUnaryFunctionsArgumentWithVoidResult<TUnaryFunction> TMyType;
	
	TBinderUnaryFunctionsArgumentWithVoidResult(const TUnaryFunction& storeUnaryFunction, const typename TUnaryFunction::argument_type& storeArgument1):
		_function(storeUnaryFunction),
		_storedArgument1(storeArgument1)
	{	// construct from functor and operand
	};
	
	void operator()(void) const
	{	// apply functor to operands
		(void)(_function(_storedArgument1));
	};
};

//Аналог bindUnaryFunctionsArgumentWithVoidResult, но для функции, возвращающий тип void.
template<class TUnaryFunction, class TArgument1>
inline TBinderUnaryFunctionsArgumentWithVoidResult<TUnaryFunction> bindUnaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument1& storeArgument1)
{	// return a TBinderUnaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinderUnaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument1));
};

//Аналог std::binder2nd (для бинарных функций) для "превращения" бинарных функции (с двумя параметрами), в функцию с одним параметром, но для функций возвращающих тип void.
//В MSVS 6.0 не работает std::bind2nd для функций, возвращающих значение void.
template<class TBinaryFunction>
class TBinder2ndBinaryFunctionsArgumentWithVoidResult : public std::unary_function<typename TBinaryFunction::first_argument_type, typename TBinaryFunction::result_type>
{	// functor adapter _Func(left, stored)
private:
	typedef typename TBinaryFunction::second_argument_type second_argument_type;
	
	const TBinaryFunction& _function;	// the functor to apply
	second_argument_type _storedArgument2;	// the right operand
	
public:
	typedef typename TBinaryFunction::first_argument_type argument_type;
	typedef typename TBinaryFunction::result_type result_type;
	
	TBinder2ndBinaryFunctionsArgumentWithVoidResult(const TBinaryFunction& storedFunction, const second_argument_type& storeArgument2):
		_function(storedFunction), _storedArgument2(storeArgument2)
	{	// construct from functor and right operand
	};
	
	result_type operator()(const argument_type& Argument1) const
	{	// apply functor to operands
		return (_function(Argument1, _storedArgument2));
	};
	
	result_type operator()(argument_type& Argument1) const
	{	// apply functor to operands
		return (_function(Argument1, _storedArgument2));
	};
};

//Аналог std::bind2nd (для бинарных функций) для "превращения" бинарных функции (с двумя параметрами), в функцию с одним параметром.
//В отличии от оригинала, биндит параметры функции по ссылке, без сообщения об ошибки компиляции "ссылка на ссылку".
//bind2ndBinaryFunctionsArgumentWithVoidResult - функция генератор для облегчения использования класса TBinder2ndBinaryFunctionsArgumentByReferenceWithVoidResult. Предназначена для автоматического вывода шаблонов.
//В MSVS 6.0 не работает std::bind2nd для функций, возвращающих значение void.
template<class TUnaryFunction, class TArgument2>
inline TBinder2ndBinaryFunctionsArgumentWithVoidResult<TUnaryFunction> bind2ndBinaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument2& storeArgument2)
{	// return a TBinder2ndBinaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinder2ndBinaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument2));
};

//Аналог std::binder1st (для бинарных функций) для "превращения" бинарных функции (с двумя параметрами), в функцию с одним параметром, но для функций возвращающих тип void.
//В MSVS 6.0 не работает std::bind1st для функций, возвращающих значение void.
template<class TBinaryFunction>
class TBinder1stBinaryFunctionsArgumentWithVoidResult : public std::unary_function<typename TBinaryFunction::first_argument_type, typename TBinaryFunction::result_type>
{	// functor adapter _Func(left, stored)
private:
	typedef typename TBinaryFunction::first_argument_type first_argument_type;
	
	const TBinaryFunction& _function;	// the functor to apply
	first_argument_type _storedArgument1;	// the right operand
	
public:
	typedef typename TBinaryFunction::second_argument_type argument_type;
	typedef typename TBinaryFunction::result_type result_type;
	
	TBinder1stBinaryFunctionsArgumentWithVoidResult(const TBinaryFunction& storedFunction, const first_argument_type& storeArgument1):
		_function(storedFunction), _storedArgument1(storeArgument1)
	{	// construct from functor and right operand
	};
	
	result_type operator()(const argument_type& Argument2) const
	{	// apply functor to operands
		return (_function(_storedArgument1, Argument2));
	};
	
	result_type operator()(argument_type& Argument2) const
	{	// apply functor to operands
		return (_function(_storedArgument1, Argument2));
	};
};

//Аналог std::bind1st (для бинарных функций) для "превращения" бинарных функции (с двумя параметрами), в функцию с одним параметром.
//В отличии от оригинала, биндит параметры функции по ссылке, без сообщения об ошибки компиляции "ссылка на ссылку".
//bind1stBinaryFunctionsArgumentWithVoidResult - функция генератор для облегчения использования класса TBinder2ndBinaryFunctionsArgumentWithVoidResult. Предназначена для автоматического вывода шаблонов.
//В MSVS 6.0 не работает std::bind1st для функций, возвращающих значение void.
template<class TUnaryFunction, class TArgument2>
inline TBinder1stBinaryFunctionsArgumentWithVoidResult<TUnaryFunction> bind1stBinaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument2& storeArgument2)
{	// return a TBinder1stBinaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinder1stBinaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument2));
};

#if (_MSC_VER>=1400) || !defined(_MSC_VER)
#ifdef RTOS_WRAPPER_BOOST_ALLOW
	//Не используется. Можно удалить.
	template<class TMayBeFunctionDirty>
	class TRemoveReferenceAndPointerFromFunctionType
	{
	private:
		typedef typename selectType<boost::is_reference<TMayBeFunctionDirty>::value, typename boost::remove_reference<TMayBeFunctionDirty>::type, TMayBeFunctionDirty>::Result TFunctionOrFunctionPointerWithoutReference;
		
	public:
		typedef typename selectType<boost::is_pointer<TFunctionOrFunctionPointerWithoutReference>::value, typename boost::remove_pointer<TFunctionOrFunctionPointerWithoutReference>::type, TFunctionOrFunctionPointerWithoutReference>::Result TResult;
	};
	
	template<class TType>
	struct TIsFunctionalType
	{
		enum{Result=boost::is_function<typename TRemoveReferenceAndPointerFromFunctionType<TType>::TResult>::value};
	};
	
	/*template<class TType>//Не используется. Можно удалить.
	struct TIsCantBeConstType
	{
		enum{Result=boost::is_function<TType>::value};
	};

	template<class TType>
	struct TAddConstAtTypeIfCan
	{
	private:
		template<bool NeedAddConst, class TType>
		struct TConditionAddConstAtType
		{
			typedef const TType TResult;
		};
		
		template<class TType>
		struct TConditionAddConstAtType<false, TType>
		{
			typedef TType TResult;
		};
		
	public:
		typedef typename TConditionAddConstAtType<!TIsCantBeConstType<TType>::Result, TType>::TResult TResult;
	};	*/

//////////////////////////////////////////////////////////////////////////
//DEFINE_MEMBER_APPLY_CLASS объявляет аналог boost::apply, но для произвольного члена, а не только вызывающий operator() первого аргумента с указанными параметрами.
//Необходимо для лексического (по имени) захвата члена, для тех случаев, когда другие варианты не возможны из-за перегрузки члена или его шаблонности.
#define INTERNAL_GENERATE_MEMBER_APPLY_CLASS(_, ARG_COUNTER, MEMBER_NAME_WITH_INDERECTION)							\
	template<class TClassWithMember BOOST_PP_COMMA_IF(ARG_COUNTER) BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)>	\
	result_type operator()(																							\
		TClassWithMember& classWithMember BOOST_PP_COMMA_IF(ARG_COUNTER)											\
		BOOST_PP_ENUM_BINARY_PARAMS(ARG_COUNTER, TArg, &arg)) const													\
	{																												\
		return classWithMember MEMBER_NAME_WITH_INDERECTION(BOOST_PP_ENUM_PARAMS(ARG_COUNTER, arg));				\
	}
	
#define DEFINE_MEMBER_APPLY_CLASS_OR_CLASS_PTR_N(TMemberApplyClassName, inderection_symbol, MemberName, MaxAmountOfArgumentForMember)			\
	template<class TResultType>																													\
	struct TMemberApplyClassName																												\
	{																																			\
		typedef TResultType result_type;																										\
		BOOST_PP_REPEAT(MaxAmountOfArgumentForMember, INTERNAL_GENERATE_MEMBER_APPLY_CLASS, BOOST_PP_CAT(inderection_symbol, MemberName))		\
	};

#define DEFINE_MEMBER_APPLY_CLASS_N(TMemberApplyClassName, MemberName, MaxAmountOfArgumentForMember) DEFINE_MEMBER_APPLY_CLASS_OR_CLASS_PTR_N(TMemberApplyClassName, .,MemberName, MaxAmountOfArgumentForMember)
#define DEFINE_MEMBER_APPLY_CLASS_PTR_N(TMemberApplyClassName, MemberName, MaxAmountOfArgumentForMember) DEFINE_MEMBER_APPLY_CLASS_OR_CLASS_PTR_N(TMemberApplyClassName, ->, MemberName, MaxAmountOfArgumentForMember)
#define DEFINE_MEMBER_APPLY_CLASS(TMemberApplyClassName, MemberName) DEFINE_MEMBER_APPLY_CLASS_N(TMemberApplyClassName, MemberName, 9)
#define DEFINE_MEMBER_APPLY_CLASS_PTR(TMemberApplyClassName, MemberName) DEFINE_MEMBER_APPLY_CLASS_N(TMemberApplyClassName, MemberName, 9)

	
	/*DEFINE_MEMBER_APPLY_CLASS(TMemberApplyClassName, MemberName)
	
	- разворачивается в:
	
	template<class TResultType>
	struct TMemberApplyClassName
	{
		typedef TResultType result_type;
		template<class TClassWithMember, class... TArgs>
		TResultType operator()(TClassWithMember& ClassWithMember, TArgs&... args) const
		{
		  return ClassWithMember.MemberName(args...);
		}
	}*/
COMMENT((
			struct TFooBar
			{
				void foo(void){cout<<"foo()"<<endl;}
				void foo(int a){cout<<"foo(int a)"<<endl;}
				void foo(float a, double b){cout<<"foo(float a, double b)"<<endl;}
				int go(string a){return 1;}
			};
			DEFINE_MEMBER_APPLY_CLASS(TCaller, foo)
			TFooBar fb1;
			TCaller<void> FooBarCall;
			FooBarCall(fb1);
			FooBarCall(fb1, 10);
			FooBarCall(fb1, 1.f, 1.);
			/*Три последние строки напечатают:
			foo()
			foo(int a)
			foo(float a, double b)*/
			//Полезно как более универсальная замена для boost::apply для boost::bind
			DEFINE_MEMBER_APPLY_CLASS(TCallerGo, go)
			cout<<boost::bind(TCallerGo<int>(), fb1, "str");
			//Напечатает 1.
			
			//DEFINE_MEMBER_APPLY_CLASS_N можно использовать для экономии времени компиляции, чтобы указать максимально возможное число принимаемых членом параметров, и не генерировать лишнего с запасом.
			//DEFINE_MEMBER_APPLY_CLASS_PTR_N - вариант, в которым вместо . для обращения к методу используется -> для передачи вместо ссылки указателя.
		))
//////////////////////////////////////////////////////////////////////////
//TReferenceWrapperToReference - метафункция, возвращающая ссылку на тип W (то есть W&), если T == boost::reference_wrapper<W>, иначе возвращает T.
template<class T>
struct TReferenceWrapperToReference: boost::mpl::if_<boost::is_reference_wrapper<T>, typename boost::unwrap_reference<T>::type&, T>
{};
//////////////////////////////////////////////////////////////////////////
// make_result_type_provider<TResultType>(функциональный_объект1) оборачивает функциональный_объект1 в другой функциональный объект result_type_provider,
//в котором есть член typedef result_type, равный TResultType.
//Полезно при использовании boost::protect, который требует этот член.
//Необходимо при использовании boost::protect в обощенных алгоритмах.
//boost::protect необходим в обощенных алгоритмах чтобы
//внешний boost::bind не превращался по ошибке в "композицию" с внутренним 
//boost::bind, который используется внутри обобщённого алгоритма (если используется конечно).
//
//result_type_provider не предоставляет arguments forwarding. Даже самый basic.
//Поэтому нельзя передавать временные объекты в качестве аргументов result_type_provider::operator().
//Ничего страшного, но просто не скомпилируется.
//Можно потом будет решить это частично или полностью, но пока не понадобилось.
//Правильно сделать result_type_provider как специализацию и в тех случаях
//если от TFunctor можно отнаследоваться, то это нужно сделать, чтобы сохранить прочие члены
//или тайпдефы (как это сделано в boost::forward_adapter). Но пока оставим так, тк пока тоже не понадобилось.
//
//Если будет проблема с arguments forwarding, то можно потом будет завернуть result_type_provider в
//boost::forward_adapter - это решит проблему.
//
//Передача make_result_type_provider аргумента boost::reference_wrapper (например через boost::cref/boost::ref) -
//позволит избежать копирования тяжелых объектов, тк он будет сохранен в result_type_provider по ссылке.
//В этом случае пользователь сам отвечает за валидность этой ссылки.
template<class TResultType, class TFunctor>
class result_type_provider
{
private:
	TFunctor _functor;
	
public:
	typedef TResultType result_type;
	
	#define MAX_ARGUMENTS_FOR_CALLABLE_OPERATOR 11
	
	explicit result_type_provider(TFunctor Functor):
		_functor(Functor)
	{}
	
	result_type operator()(void) const
	{
		return this->_functor();
	}
	
	result_type operator()(void)
	{
		return this->_functor();
	}
	
	#define INTERNAL_GENERATE_CALLABLE_OPERATOR(_, ARG_COUNTER, __)									\
		template<BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)>										\
		result_type operator()(BOOST_PP_ENUM_BINARY_PARAMS(ARG_COUNTER, TArg, &arg)) const			\
		{																							\
			return this->_functor(BOOST_PP_ENUM_PARAMS(ARG_COUNTER, arg));							\
		}																							\
																									\
		template<BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)>										\
		result_type operator()(BOOST_PP_ENUM_BINARY_PARAMS(ARG_COUNTER, TArg, &arg))				\
		{																							\
			return this->_functor(BOOST_PP_ENUM_PARAMS(ARG_COUNTER, arg));							\
		}
	BOOST_PP_REPEAT_FROM_TO(1, MAX_ARGUMENTS_FOR_CALLABLE_OPERATOR, INTERNAL_GENERATE_CALLABLE_OPERATOR, BOOST_PP_EMPTY())
	#undef INTERNAL_GENERATE_CALLABLE_OPERATOR
};

template<class TFunctor>
class result_type_provider<void, TFunctor>
{
private:
	TFunctor _functor;

public:
	typedef void result_type;

	explicit result_type_provider(TFunctor Functor):
		_functor(Functor)
	{}

	void operator()(void) const
	{
		this->_functor();
	}

	void operator()(void)
	{
		return this->_functor();
	}

#define INTERNAL_GENERATE_CALLABLE_OPERATOR(_, ARG_COUNTER, __)									\
	template<BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)>										\
	void operator()(BOOST_PP_ENUM_BINARY_PARAMS(ARG_COUNTER, TArg, &arg)) const					\
	{																							\
		this->_functor(BOOST_PP_ENUM_PARAMS(ARG_COUNTER, arg));									\
	}																							\
																								\
	template<BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)>										\
	void operator()(BOOST_PP_ENUM_BINARY_PARAMS(ARG_COUNTER, TArg, &arg))						\
	{																							\
		this->_functor(BOOST_PP_ENUM_PARAMS(ARG_COUNTER, arg));									\
	}
	BOOST_PP_REPEAT_FROM_TO(1, MAX_ARGUMENTS_FOR_CALLABLE_OPERATOR, INTERNAL_GENERATE_CALLABLE_OPERATOR, BOOST_PP_EMPTY())
#undef INTERNAL_GENERATE_CALLABLE_OPERATOR
#undef MAX_ARGUMENTS_FOR_CALLABLE_OPERATOR
};

template<class TResultType, class TFunctor>
result_type_provider<TResultType, typename TReferenceWrapperToReference<TFunctor>::type > make_result_type_provider(TFunctor Functor)
{
	typedef result_type_provider<TResultType, typename TReferenceWrapperToReference<TFunctor>::type > TResult;
	return TResult(Functor);
}


//////////////////////////////////////////////////////////////////////////
//const boost::optional<int> Value = some_callable_return_optional_int();
//int a = get_value_or_call_value(Value, some_callable_return_int);
//double b = get_value_or_call_value(boost::type<double>() Value, some_callable_return_int);
//- разворачивает значение из boost::optional и если его нет, то возвращает значение из функции Call_value.
template<class R, class T, class F>
R get_value_or_call_value(boost::type<R>, const boost::optional<T> Value, const F& Call_value)
{
	if(Value)
		return *Value;
	else
		return Call_value();
}

template<class T, class F>
T /*typename boost::result_of<F()>::type*/ get_value_or_call_value(const boost::optional<T> Value, const F& Call_value)
{
	if(Value)
		return *Value;
	else
		return Call_value();
}
#endif
//////////////////////////////////////////////////////////////////////////
//Необходимо для более лёгкой инициализации массивов std::pair через списки инициализации Си:
	
	template<class TKey, class TValue>
	struct TAggregatePair
	{
		typedef TAggregatePair<TKey, TValue> _Myt;			//Скорее это не нужно, но на всякий случай оставим для совместимости с VS.
		
		typedef TKey first_type;
		typedef TValue second_type;
		
		typedef std::pair<TKey, TValue> TPair;
		typedef std::map<TKey, TValue> TMap;
		
		typedef typename TUnConst<TKey>::TResult TUnConstedKey;
		typedef typename TUnConst<TValue>::TResult TUnConstedValue;
		
		TKey first;		// the first stored value
		TValue second;	// the second stored value
		
		operator std::pair<TUnConstedKey, TUnConstedValue>(void) const
		{
			return std::make_pair(first, second);
		};
		
		operator std::pair<const TUnConstedKey, TUnConstedValue>(void) const			//Реализация map в MSVS 2008 требует const у ключа TKeyType.
		{
			return std::make_pair(first, second);
		};
		
		operator std::pair<TUnConstedKey, const TUnConstedValue>(void) const
		{
			return std::make_pair(first, second);
		};
		
		operator std::pair<const TUnConstedKey, const TUnConstedValue>(void) const
		{
			return std::make_pair(first, second);
		};
	};
#endif

	COMMENT((
			//Пример использования TAggregatePair:
			
			#include "Common\accessory\functional_extension.h"
			#include <utility>
			#include <map>
			#include <Common\accessory\MainTypes.h>
			
			using namespace std;
			
			typedef TAggregatePair<const int, PChar> TStrToIntPairs;
			const TStrToIntPairs StrToIntPairs[] =				//Используем TAggregatePair<,> взамен std::pair<,> и инициализируем массив в декларативном Си стиле:
			{
				{1, "str1"},
				{2, "str2"},
				{3, "str3"}
			};
			
			static TStrToIntPairs::TMap StrToIntMap;							//Можно написать просто "map<string, int> StrToIntMap;", но так всеж красивее.
			copy(StrToIntPairs, EndIteratorOfArray(StrToIntPairs), inserter(StrToIntMap, StrToIntMap.begin()));		//Заполняем map StrToIntMap из массива пар.
			//Теперь map StrToIntMap содержит все пары ключ-значение из массива StrToIntPairs.
			//И к ним можно обращаться через StrToIntMap["str1"]
			//Массив пар и вызов copy должны находится в одной единице трансялции, иначе невозможно использовать EndIteratorOfArray, тк не известна длина массива в точке использования.
			//MSVS 6.0 похоже не умеет инициализировать структуры в Си стиле... о_О
		))

//////////////////////////////////////////////////////////////////////////
/*//Комментирую, тк пока не понадобилось. Нужно удалить.
#define DEFINE_UNWRAP_TRAIT(TUnwrapName)	\
	template<class TType>					\
		struct TUnwrapName					\
		{									\
			typedef TType TResult;			\
		}

#define DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TUnwrapName, TSourceType)		\
	template<class TType>												\
		struct TUnwrapName<TSourceType>									\
		{																\
			typedef TType TResult;										\
		}

//Объявляет трейт, что извлекает тип из завернутого типа.
COMMENT((
	DEFINE_UNWRAP_TRAIT(TRemove_reference_wrapper);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType>);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> const);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> volatile const);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> volatile);
	//Теперь:
	TRemove_reference_wrapper<boost::reference_wrapper<TAnyType> const>::TResult //- вернет TAnyType
	TRemove_reference_wrapper<boost::reference_wrapper<TAnyType> >::TResult //- вернет TAnyType
	TRemove_reference_wrapper<std::vector<TAnyType> >::TResult //- вернет std::vector<TAnyType>
))*/

/////////////////////////////////////////////////////////////////////////
#if (_MSC_VER>=1400) || !defined(_MSC_VER)//Думаю и на 6ке работает. Не проверял.
//#ifdef RTOS_WRAPPER_BOOST_ALLOW

COMMENT((
	//Пример использования:
	void func(const int Val = ::DefaultValue(), const std::wstring Val2 = ::DefaultValue())	
	//...
	func();
	func(::DefaultValue(), ::DefaultValue());
	func(DEFAULT_VALUE_FE, DEFAULT_VALUE_FE);
	//func(::DefaultValue<int>(), ::DefaultValue<TStruct>());
	func(5);
	const TStruct Struct1(2,5,7);
	func(5, Struct1);
	func(::DefaultValue(), Struct1);
	func(DEFAULT_VALUE_FE, Struct1);

	void func0(const int Val = ::DefaultValue(), const std::string Val2 = ::DefaultValue())
	{
		//Пользуемся Val и Val2
		//...
	}

	//-----------------------------------------------------------------------------------------------------------
	template<class T1, class T2>
	void func1(const T1 Val = ::DefaultValue(), const T2&/*передали TStruct по ссылке*/ Val2 = ::DefaultValue())
	{
		const int Val_ = ::getValueFromDefault<int>(Val);
		const TStruct& Val2_ = ::getValueFromDefault<TStruct>(Val2);
		//Пользуемся Val_ и Val2_
		//...
	}
	//Но есть одна особенность шаблонных параметров и значений поумолчанию в С++. Нужно создть ещё перегрузки с меньшим числом шаблонов:
	template<class T1/*На один шаблон меньше*/>
	void func1(const T1 Val = ::DefaultValue())
	{
		const int Val_ = ::getValueFromDefault<int>(Val);
		const TStruct Val2_;
		//Пользуемся Val_ и Val2_
		//...
	}
	//Это по причине того, что для функции template<class T> void f(T arg=5) - при вызове f(10) T выведется например в int, а если вызвать f(), то T не выведется, тк неоткуда!
	//То есть параметры поумолчанию в таких функциях носят чисто анотационный характер для людей, чтобы знали какое реально значение примет arg, если вызвать f(::DefaultValue()).
	//-----------------------------------------------------------------------------------------------------------
	
	template<class T2>
	void func2(/*...*/, const T2& Val2 = ::DefaultValue())
	{
		//Извлекаем из возможного значения поумолчанию подругому:		
		const TStruct& Val2_ = ::getValueFromDefault<TStruct>(Val2, TStruct()/*Конструируем громоздкий TStruct для дефолтного значения*/);
		//Пользуемся Val2_
		//...
	}

	template<class T2>
	void func3(/*...*/, const T2& Val2 = ::DefaultValue())
	{
		struct _ 
		{
			static TStruct StructMaker(void)
			{
				return TStruct();
			}
		};
		//Извлекаем из возможного значения поумолчанию подругому:		
		const TStruct& Val2_ = ::getValueFromDefaultMaker<TStruct>(Val2, &(_::StructMaker)/*Не конструируем громоздкий TStruct, пока не потребуется.*/);
		//Пользуемся Val2_
		//...
	}
))
struct TDefaultValue
{
	template<class TResult>
	operator TResult(void) const
	{
		return TResult();
	}
};

#define DEFAULT_VALUE_FE ::DefaultValue()
//Используем постфикс _FE во избежание возможных коллизий. Постфикс, а не префекс, тк IdeAutoComplit-frendly.

/*template<class T> //Не используется. Удали.
struct TIsDefaultValue
{
private:
	typedef typename ::boost::remove_cv<typename ::boost::remove_reference<T>::type>::type TWithoutCVReference;

public:
	enum {value = ::boost::is_same<TWithoutCVReference, TDefaultValue>::value };
};*/

template<class TValueType, class TValueTypeOrTDefaultValue>
TValueType getValueFromDefault(const TValueTypeOrTDefaultValue& Value, const TValueType&)
{
	return Value;
}

template<class TValueType>
TValueType getValueFromDefault(const TDefaultValue&, const TValueType& DefaultValue)
{
	return DefaultValue;
}

/*template<class TValueType>//Мусор. Удалить.
TValueType getValueFromDefault(const TDefaultValue*, const TValueType& DefaultValue)
{
	return DefaultValue;
}*/

template<class TValueType, class TValueTypeOrTDefaultValue>
TValueType getValueFromDefault(const TValueTypeOrTDefaultValue& Value)
{
	return Value;
}

template<class TValueType>
TValueType getValueFromDefault(const TDefaultValue&)
{
	return TValueType();
}

/*template<class TValueType>//Мусор. Удалить.
TValueType getValueFromDefault(const TDefaultValue*)
{
	return TValueType();
}*/

template<class TValueType, class TValueTypeMakerFunctor, class TValueTypeOrTDefaultValue>
TValueType getValueFromDefaultMaker(const TValueTypeOrTDefaultValue& Value, const TValueTypeMakerFunctor&)
{
	return Value;
}

template<class TValueType, class TValueTypeMakerFunctor>
TValueType getValueFromDefaultMaker(const TDefaultValue&, const TValueTypeMakerFunctor& DefaultValueMakerFunctor)
{
	return DefaultValueMakerFunctor();
}
//Тут бы перемещающие варианты функций добавить, но пока не будем.

/*template<class TValueType, class TValueTypeMakerFunctor>//Мусор. Удалить.
TValueType getValueFromDefaultMaker(const TDefaultValue*, const TValueTypeMakerFunctor& DefaultValueMakerFunctor)
{
	return DefaultValueMakerFunctor();
}*/

namespace
{
	/*template<class T>
	T DefaultValue(void)
	{
		return T();
	}*/
	
	TDefaultValue DefaultValue(void)
	{
		return TDefaultValue();
	}
	
	/*template<class T>
	T DefaultValue(const T& value)
	{
		return T(value);
	}*/
};
//#endif
#endif
/////////////////////////////////////////////////////////////////////////
#endif // functional_extension_h__