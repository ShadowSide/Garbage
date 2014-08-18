#ifndef mplContainersCompositor_h__
#define mplContainersCompositor_h__

#include "boost/mpl/list.hpp"
#include "boost/mpl/at.hpp"
#include "boost/mpl/has_key.hpp"
#include "boost/mpl/empty.hpp"
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>

//������� TMPLMapCompositor. �� ������������ ��� ����������� boost::mpl::map � ���� ���������.
//�������� ���������� ���� � ������ ��������� ������ 20�� �� ������� ������ ����.
//��������� ���:
COMMENT((
typedef TMPLMapCompositor<boost::mpl::list<
	MapOfTypes1,
	MapOfTypes2,
	...
	MapOfTypesN
> > MapOfTypes;
))
//��� MapOfTypesN - boost::mpl::map, ������� ����� ����������.
//������ ����� ��������� ��� boost::mpl::set.
//����� ������� 3 ���������:
//MPLMapCompositor_empty - ������� boost::mpl::empty.
//MPLMapCompositor_has_key - ������� boost::mpl::has_key.
//MPLMapCompositor_at - ������� boost::mpl::at.

namespace NInternalMplContainersCompositor
{
	template<class TCurrent, class TEnd, const bool Result>
	struct MPLMapCompositor_empty
	{
		typedef typename NInternalMplContainersCompositor::MPLMapCompositor_empty<
			typename boost::mpl::next<TCurrent>::type,
			TEnd,
			boost::mpl::empty<
				typename boost::mpl::deref<TCurrent>::type
			>::type::value && Result
		>::type type;
	};

	template<class TEnd, const bool Result>
	struct MPLMapCompositor_empty<TEnd, TEnd, Result>
	{
		typedef boost::mpl::bool_<Result> type;
	};


	template<class TKey, class TCurrent, class TEnd, const bool Result>
	struct MPLMapCompositor_has_key
	{
		typedef typename NInternalMplContainersCompositor::MPLMapCompositor_has_key<
			TKey,
			typename boost::mpl::next<TCurrent>::type,
			TEnd,
			boost::mpl::has_key<
				typename boost::mpl::deref<TCurrent>::type,
				TKey
			>::type::value || Result
		>::type type;
	};

	template<class TKey, class TEnd, const bool Result>
	struct MPLMapCompositor_has_key<TKey, TEnd, TEnd, Result>
	{
		typedef boost::mpl::bool_<Result> type;
	};

	template<class TKey, class TCurrent, class TEnd>
	class MPLMapCompositor_at
	{
	private:
		typedef typename boost::mpl::deref<TCurrent>::type TCurrentSequence;

	public:
		typedef 
			typename boost::mpl::eval_if<
				typename boost::mpl::has_key<
					TCurrentSequence,
					TKey
				>::type,
				typename boost::mpl::at<TCurrentSequence, TKey>::type,
				typename NInternalMplContainersCompositor::MPLMapCompositor_at<
					TKey,
					typename boost::mpl::next<TCurrent>::type,
					TEnd
				>::type
			> type;
	};

	struct TNotFoundType{};

	template<class TKey, class TEnd>
	class MPLMapCompositor_at<TKey, TEnd, TEnd>
	{
	public:
		typedef TNotFoundType type;
	};//�� ������ ���� TKey.*/
	
}

template<class TMPLMapCompositor_>
class MPLMapCompositor_empty
{
private:
	typedef typename TMPLMapCompositor_::TContainersListValue TContainersListValue;

public:
	typedef 
		typename boost::mpl::eval_if<
			typename boost::mpl::empty<TContainersListValue>,
			boost::mpl::true_,
			typename NInternalMplContainersCompositor::MPLMapCompositor_empty<
				typename boost::mpl::begin<TContainersListValue>::type,
				typename boost::mpl::end<TContainersListValue>::type,
				true
			>::type
		> type;
};

template<class TMPLMapCompositor_, class TKey>
class MPLMapCompositor_has_key
{
private:
	typedef typename TMPLMapCompositor_::TContainersListValue TContainersListValue;

public:
	typedef 
		typename boost::mpl::eval_if<
			typename MPLMapCompositor_empty<TMPLMapCompositor_>::type,
			boost::mpl::false_,
			typename NInternalMplContainersCompositor::MPLMapCompositor_has_key<
				TKey,
				typename boost::mpl::begin<TContainersListValue>::type,
				typename boost::mpl::end<TContainersListValue>::type,
				false
			>::type
		> type;
};

template<class TMPLMapCompositor_, class TKey>
class MPLMapCompositor_at
{
private:
	typedef typename TMPLMapCompositor_::TContainersListValue TContainersListValue;

public:
	typedef 
		typename NInternalMplContainersCompositor::MPLMapCompositor_at<
			TKey,
			typename boost::mpl::begin<TContainersListValue>::type,
			typename boost::mpl::end<TContainersListValue>::type
		>::type type;
};


template<class TContainersList>
struct TMPLMapCompositor
{
	typedef TContainersList TContainersListValue;	
};

#endif // mplContainersCompositor_h__