#ifndef functional_extension_h__
#define functional_extension_h__
//���������� ����������� ���������� <functional>.

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

//reverse_iterator_patched - � MSVC6.0 �� ����� �������� std::reverse_iterator. �� �� ���������. ������� ������������ reverse_iterator_patched ������ std::reverse_iterator.
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

//������ ������ std::pointer_to_unary_function, "�������������" ��������� �� ������� ������� (���� ��������) � �������������� ������.
//� MSVS 6.0 �� �������� std::pointer_to_unary_function ��� �������, ������������ �������� void.
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

//������ ������ std::pointer_to_binary_function, "�������������" ��������� �� �������� ������� (��� ���������) � �������������� ������.
//� MSVS 6.0 �� �������� std::pointer_to_binary_function ��� �������, ������������ �������� void.
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

//makePointerToFunctionWithVoidResult - ������� ��������� ��� ���������� ������������� ������ TPointerToUnaryFunctionWithVoidResult. ������������� ��� ��������������� ������ ��������.
//������ ������� std::ptr_fun ��� �������, ������������ ��� void.
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

//makePointerToFunctionWithVoidResult - ������� ��������� ��� ���������� ������������� ������ TPointerToBinaryFunctionWithVoidResult. ������������� ��� ��������������� ������ ��������.
//������ ������� std::ptr_fun ��� �������, ������������ ��� void.
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

//������ std::binder1st (��� �������� �������), �� ��� "�����������" ������� ������� (� ����� ����������), � ������� ��� ����������.
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

//������ std::bind1st (bind1st ��� �������� �������), �� ��� "�����������" ������� ������� (� ����� ����������), � ������� ��� ����������.
//bindUnaryFunctionsArgument - ������� ��������� ��� ���������� ������������� ������ TBinderUnaryFunctionsArgument. ������������� ��� ��������������� ������ ��������.
template<class TUnaryFunction, class TArgument1>
inline TBinderUnaryFunctionsArgument<TUnaryFunction> bindUnaryFunctionsArgument(const TUnaryFunction& unaryFunction, const TArgument1& storeArgument1)
{	// return a TBinderUnaryFunctionsArgument functor adapter
	return (TBinderUnaryFunctionsArgument<TUnaryFunction>(unaryFunction, storeArgument1));
};

//������ TBinderUnaryFunctionsArgument, �� ��� �������, ������������ ��� void.
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

//������ bindUnaryFunctionsArgumentWithVoidResult, �� ��� �������, ������������ ��� void.
template<class TUnaryFunction, class TArgument1>
inline TBinderUnaryFunctionsArgumentWithVoidResult<TUnaryFunction> bindUnaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument1& storeArgument1)
{	// return a TBinderUnaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinderUnaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument1));
};

//������ std::binder2nd (��� �������� �������) ��� "�����������" �������� ������� (� ����� �����������), � ������� � ����� ����������, �� ��� ������� ������������ ��� void.
//� MSVS 6.0 �� �������� std::bind2nd ��� �������, ������������ �������� void.
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

//������ std::bind2nd (��� �������� �������) ��� "�����������" �������� ������� (� ����� �����������), � ������� � ����� ����������.
//� ������� �� ���������, ������ ��������� ������� �� ������, ��� ��������� �� ������ ���������� "������ �� ������".
//bind2ndBinaryFunctionsArgumentWithVoidResult - ������� ��������� ��� ���������� ������������� ������ TBinder2ndBinaryFunctionsArgumentByReferenceWithVoidResult. ������������� ��� ��������������� ������ ��������.
//� MSVS 6.0 �� �������� std::bind2nd ��� �������, ������������ �������� void.
template<class TUnaryFunction, class TArgument2>
inline TBinder2ndBinaryFunctionsArgumentWithVoidResult<TUnaryFunction> bind2ndBinaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument2& storeArgument2)
{	// return a TBinder2ndBinaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinder2ndBinaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument2));
};

//������ std::binder1st (��� �������� �������) ��� "�����������" �������� ������� (� ����� �����������), � ������� � ����� ����������, �� ��� ������� ������������ ��� void.
//� MSVS 6.0 �� �������� std::bind1st ��� �������, ������������ �������� void.
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

//������ std::bind1st (��� �������� �������) ��� "�����������" �������� ������� (� ����� �����������), � ������� � ����� ����������.
//� ������� �� ���������, ������ ��������� ������� �� ������, ��� ��������� �� ������ ���������� "������ �� ������".
//bind1stBinaryFunctionsArgumentWithVoidResult - ������� ��������� ��� ���������� ������������� ������ TBinder2ndBinaryFunctionsArgumentWithVoidResult. ������������� ��� ��������������� ������ ��������.
//� MSVS 6.0 �� �������� std::bind1st ��� �������, ������������ �������� void.
template<class TUnaryFunction, class TArgument2>
inline TBinder1stBinaryFunctionsArgumentWithVoidResult<TUnaryFunction> bind1stBinaryFunctionsArgumentWithVoidResult(const TUnaryFunction& unaryFunction, const TArgument2& storeArgument2)
{	// return a TBinder1stBinaryFunctionsArgumentWithVoidResult functor adapter
	return (TBinder1stBinaryFunctionsArgumentWithVoidResult<TUnaryFunction>(unaryFunction, storeArgument2));
};

#if (_MSC_VER>=1400) || !defined(_MSC_VER)
#ifdef RTOS_WRAPPER_BOOST_ALLOW
	//�� ������������. ����� �������.
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
	
	/*template<class TType>//�� ������������. ����� �������.
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
//DEFINE_MEMBER_APPLY_CLASS ��������� ������ boost::apply, �� ��� ������������� �����, � �� ������ ���������� operator() ������� ��������� � ���������� �����������.
//���������� ��� ������������ (�� �����) ������� �����, ��� ��� �������, ����� ������ �������� �� �������� ��-�� ���������� ����� ��� ��� �����������.
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
	
	- ��������������� �:
	
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
			/*��� ��������� ������ ����������:
			foo()
			foo(int a)
			foo(float a, double b)*/
			//������� ��� ����� ������������� ������ ��� boost::apply ��� boost::bind
			DEFINE_MEMBER_APPLY_CLASS(TCallerGo, go)
			cout<<boost::bind(TCallerGo<int>(), fb1, "str");
			//���������� 1.
			
			//DEFINE_MEMBER_APPLY_CLASS_N ����� ������������ ��� �������� ������� ����������, ����� ������� ����������� ��������� ����� ����������� ������ ����������, � �� ������������ ������� � �������.
			//DEFINE_MEMBER_APPLY_CLASS_PTR_N - �������, � ������� ������ . ��� ��������� � ������ ������������ -> ��� �������� ������ ������ ���������.
		))
//////////////////////////////////////////////////////////////////////////
//TReferenceWrapperToReference - �����������, ������������ ������ �� ��� W (�� ���� W&), ���� T == boost::reference_wrapper<W>, ����� ���������� T.
template<class T>
struct TReferenceWrapperToReference: boost::mpl::if_<boost::is_reference_wrapper<T>, typename boost::unwrap_reference<T>::type&, T>
{};
//////////////////////////////////////////////////////////////////////////
// make_result_type_provider<TResultType>(��������������_������1) ����������� ��������������_������1 � ������ �������������� ������ result_type_provider,
//� ������� ���� ���� typedef result_type, ������ TResultType.
//������� ��� ������������� boost::protect, ������� ������� ���� ����.
//���������� ��� ������������� boost::protect � ��������� ����������.
//boost::protect ��������� � ��������� ���������� �����
//������� boost::bind �� ����������� �� ������ � "����������" � ���������� 
//boost::bind, ������� ������������ ������ ����������� ��������� (���� ������������ �������).
//
//result_type_provider �� ������������� arguments forwarding. ���� ����� basic.
//������� ������ ���������� ��������� ������� � �������� ���������� result_type_provider::operator().
//������ ���������, �� ������ �� ��������������.
//����� ����� ����� ������ ��� �������� ��� ���������, �� ���� �� ������������.
//��������� ������� result_type_provider ��� ������������� � � ��� �������
//���� �� TFunctor ����� ���������������, �� ��� ����� �������, ����� ��������� ������ �����
//��� �������� (��� ��� ������� � boost::forward_adapter). �� ���� ������� ���, �� ���� ���� �� ������������.
//
//���� ����� �������� � arguments forwarding, �� ����� ����� ����� ��������� result_type_provider �
//boost::forward_adapter - ��� ����� ��������.
//
//�������� make_result_type_provider ��������� boost::reference_wrapper (�������� ����� boost::cref/boost::ref) -
//�������� �������� ����������� ������� ��������, �� �� ����� �������� � result_type_provider �� ������.
//� ���� ������ ������������ ��� �������� �� ���������� ���� ������.
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
//- ������������� �������� �� boost::optional � ���� ��� ���, �� ���������� �������� �� ������� Call_value.
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
//���������� ��� ����� ����� ������������� �������� std::pair ����� ������ ������������� ��:
	
	template<class TKey, class TValue>
	struct TAggregatePair
	{
		typedef TAggregatePair<TKey, TValue> _Myt;			//������ ��� �� �����, �� �� ������ ������ ������� ��� ������������� � VS.
		
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
		
		operator std::pair<const TUnConstedKey, TUnConstedValue>(void) const			//���������� map � MSVS 2008 ������� const � ����� TKeyType.
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
			//������ ������������� TAggregatePair:
			
			#include "Common\accessory\functional_extension.h"
			#include <utility>
			#include <map>
			#include <Common\accessory\MainTypes.h>
			
			using namespace std;
			
			typedef TAggregatePair<const int, PChar> TStrToIntPairs;
			const TStrToIntPairs StrToIntPairs[] =				//���������� TAggregatePair<,> ������ std::pair<,> � �������������� ������ � ������������� �� �����:
			{
				{1, "str1"},
				{2, "str2"},
				{3, "str3"}
			};
			
			static TStrToIntPairs::TMap StrToIntMap;							//����� �������� ������ "map<string, int> StrToIntMap;", �� ��� ���� ��������.
			copy(StrToIntPairs, EndIteratorOfArray(StrToIntPairs), inserter(StrToIntMap, StrToIntMap.begin()));		//��������� map StrToIntMap �� ������� ���.
			//������ map StrToIntMap �������� ��� ���� ����-�������� �� ������� StrToIntPairs.
			//� � ��� ����� ���������� ����� StrToIntMap["str1"]
			//������ ��� � ����� copy ������ ��������� � ����� ������� ����������, ����� ���������� ������������ EndIteratorOfArray, �� �� �������� ����� ������� � ����� �������������.
			//MSVS 6.0 ������ �� ����� ���������������� ��������� � �� �����... �_�
		))

//////////////////////////////////////////////////////////////////////////
/*//�����������, �� ���� �� ������������. ����� �������.
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

//��������� �����, ��� ��������� ��� �� ����������� ����.
COMMENT((
	DEFINE_UNWRAP_TRAIT(TRemove_reference_wrapper);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType>);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> const);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> volatile const);
	DEFINE_UNWRAP_TRAIT_SPECIALIZTION(TRemove_reference_wrapper, boost::reference_wrapper<TType> volatile);
	//������:
	TRemove_reference_wrapper<boost::reference_wrapper<TAnyType> const>::TResult //- ������ TAnyType
	TRemove_reference_wrapper<boost::reference_wrapper<TAnyType> >::TResult //- ������ TAnyType
	TRemove_reference_wrapper<std::vector<TAnyType> >::TResult //- ������ std::vector<TAnyType>
))*/

/////////////////////////////////////////////////////////////////////////
#if (_MSC_VER>=1400) || !defined(_MSC_VER)//����� � �� 6�� ��������. �� ��������.
//#ifdef RTOS_WRAPPER_BOOST_ALLOW

COMMENT((
	//������ �������������:
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
		//���������� Val � Val2
		//...
	}

	//-----------------------------------------------------------------------------------------------------------
	template<class T1, class T2>
	void func1(const T1 Val = ::DefaultValue(), const T2&/*�������� TStruct �� ������*/ Val2 = ::DefaultValue())
	{
		const int Val_ = ::getValueFromDefault<int>(Val);
		const TStruct& Val2_ = ::getValueFromDefault<TStruct>(Val2);
		//���������� Val_ � Val2_
		//...
	}
	//�� ���� ���� ����������� ��������� ���������� � �������� ����������� � �++. ����� ������ ��� ���������� � ������� ������ ��������:
	template<class T1/*�� ���� ������ ������*/>
	void func1(const T1 Val = ::DefaultValue())
	{
		const int Val_ = ::getValueFromDefault<int>(Val);
		const TStruct Val2_;
		//���������� Val_ � Val2_
		//...
	}
	//��� �� ������� ����, ��� ��� ������� template<class T> void f(T arg=5) - ��� ������ f(10) T ��������� �������� � int, � ���� ������� f(), �� T �� ���������, �� ��������!
	//�� ���� ��������� ����������� � ����� �������� ����� ����� ������������ �������� ��� �����, ����� ����� ����� ������� �������� ������ arg, ���� ������� f(::DefaultValue()).
	//-----------------------------------------------------------------------------------------------------------
	
	template<class T2>
	void func2(/*...*/, const T2& Val2 = ::DefaultValue())
	{
		//��������� �� ���������� �������� ����������� ���������:		
		const TStruct& Val2_ = ::getValueFromDefault<TStruct>(Val2, TStruct()/*������������ ���������� TStruct ��� ���������� ��������*/);
		//���������� Val2_
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
		//��������� �� ���������� �������� ����������� ���������:		
		const TStruct& Val2_ = ::getValueFromDefaultMaker<TStruct>(Val2, &(_::StructMaker)/*�� ������������ ���������� TStruct, ���� �� �����������.*/);
		//���������� Val2_
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
//���������� �������� _FE �� ��������� ��������� ��������. ��������, � �� �������, �� IdeAutoComplit-frendly.

/*template<class T> //�� ������������. �����.
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

/*template<class TValueType>//�����. �������.
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

/*template<class TValueType>//�����. �������.
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
//��� �� ������������ �������� ������� ��������, �� ���� �� �����.

/*template<class TValueType, class TValueTypeMakerFunctor>//�����. �������.
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