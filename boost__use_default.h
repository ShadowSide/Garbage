#ifndef boost__use_default_h__
#define boost__use_default_h__

#include<Common\accessory\accessory.h>
#include<boost/mpl/eval_if.hpp>
#include<boost/mpl/identity.hpp>
#include<boost/type_traits/is_same.hpp>

//Требуется для организации поддержки ::boost::use_default параметров в шаблонах. Используем ::boost::use_default взамен параметра поумолчанию, таким образом можно
//организовывать параметры поумолчанию посреди цепочки параметров шаблонов.

// forward declaration
namespace boost { struct use_default; }

template<
	class TSource,
	class TDefaultedResult
>
struct TBoostDefaultTypeHelper:
	::boost::mpl::eval_if<
		::boost::is_same<TSource, ::boost::use_default>
		, ::boost::mpl::identity<TDefaultedResult>
		, ::boost::mpl::identity<TSource>
	>
{};

template<
	class TSource,
	class TDefaultedResultFunctor
>
struct TBoostDefaultTypeHelperWithTDefaultedResultFunctor:
	::boost::mpl::eval_if<
		::boost::is_same<TSource, ::boost::use_default>
		, TDefaultedResultFunctor
		, ::boost::mpl::identity<TSource>
	>
{};
#endif // boost__use_default_h__