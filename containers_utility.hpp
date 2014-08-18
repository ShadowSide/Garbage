#ifndef containers_utility_h__
#define containers_utility_h__

#include <Common\accessory\accessory.h>
#include <new>
#include <utility>
#include <boost\mpl\eval_if.hpp>
#include <boost\type_traits\is_convertible.hpp>
#include <boost\mpl\identity.hpp>
#include <boost\mpl\if.hpp>
#include <functional>

//Библиотека посильной помощи создателям контейнеров.
namespace containers_utility
{
	template<class T>
	void destroy_value(const T& constructed_value)
	{
		ASSERT(&constructed_value!=NULL);
		constructed_value.~T();
	}

	template<class T>
	void destroy_by_pointer(const T* const constructed_value)
	{
		destroy_value(*constructed_value);
	}

	//Убивает объекты, на которые указывает итератор, вызовом деструктора.
	template<class ForwardIterator>
	ForwardIterator destroy(ForwardIterator begin, const ForwardIterator& end)
	{
		for (; begin!=end; ++begin)
			destroy_value(*begin);
		return begin;
	}

	template<class ForwardIterator>
	struct uninitialized_copy_result_t
	{
		const ForwardIterator m_unitialized_iterator;
		const size_t m_sucessfull_inserted_items_amount;
		const bool m_transaction_fully_successful;		
	};

	struct uninitialized_copy_simple_result_t
	{
		template<class ForwardIterator>
		uninitialized_copy_simple_result_t(const uninitialized_copy_result_t<ForwardIterator>& uninitialized_copy_result):
			m_sucessfull_inserted_items_amount(uninitialized_copy_result.m_sucessfull_inserted_items_amount),
			m_transaction_fully_successful(uninitialized_copy_result.m_transaction_fully_successful)
		{}
		const size_t m_sucessfull_inserted_items_amount;
		const bool m_transaction_fully_successful;
	};

	namespace containers_utility_detail
	{
		struct roll_back_implementation
		{
			class for_bidirectional_or_more_powerful_iterator_tag;
			class for_forward_iterator_tag;
			class not_suitable_this_iterator_for_rollback_tag;
		};

		template<class RollBackImplementation>
		struct rollback_implementation_t;

		template<>
		struct rollback_implementation_t<roll_back_implementation::for_bidirectional_or_more_powerful_iterator_tag>
		{
			template<class BidirectionalIterator>
			static void rollback_implementation(const BidirectionalIterator& begin, const BidirectionalIterator& end)
			{
				typedef std::reverse_iterator<BidirectionalIterator> reversed_iterator;
				if(begin==end)
					return;
				const reversed_iterator begin_(end);
				const reversed_iterator end_(begin);
				destroy(begin_, end_);
			}		
		};

		template<>
		struct rollback_implementation_t<roll_back_implementation::for_forward_iterator_tag>
		{
			template<class ForwardIterator>
			static void rollback_implementation(const ForwardIterator& begin, const ForwardIterator& end)
			{
				destroy(begin, end);
			}
		};

		template<class InputIterator, class ForwardIterator, class Predicate>
		uninitialized_copy_result_t<ForwardIterator> uninitialized_copy_and_rollback_if_exception_implementation(InputIterator source_begin, const InputIterator& source_end,
			ForwardIterator destination_begin, const Predicate& alternative_stop_predicate )
		{
			size_t sucessfull_inserted_items_amount=0;
			ForwardIterator destination_begin_saved=destination_begin;
			typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
			typedef const uninitialized_copy_result_t<ForwardIterator> result_t;
			try{
				for (; source_begin!=source_end&&alternative_stop_predicate(destination_begin); ++sucessfull_inserted_items_amount, ++destination_begin, ++source_begin)
				{
					new (static_cast<void*>(&*destination_begin)) value_type(*source_begin);
				}
			}catch(...){
				rollback(destination_begin_saved, destination_begin);
				throw;
			}
			result_t result=
			{
				destination_begin, 
				sucessfull_inserted_items_amount, 
				source_begin==source_end
			};
			ASSERT(result.m_sucessfull_inserted_items_amount==std::distance(destination_begin_saved, destination_begin));
			return result;
		}
	}

	//Функция предназаначенна для отката недоконструированных объектов. В зависмоти от типа использоваемого итератора выбирает наиболее безопасный вариант.
	//При использовании ForwardIterator у неё нет выбора она использует прямой откат. Если итератор более мощный, например BidirectionalIterator, то откатывает в более безопасном вырианте - обратном созданию. Иначе не компилируется, тк итераторы не подходят.
	template<class ForwardIterator>
	void rollback(const ForwardIterator& begin, const ForwardIterator& end)
	{
		using namespace boost;
		using namespace mpl;
		using namespace containers_utility_detail;
		typedef typename std::iterator_traits<ForwardIterator>::iterator_category iterator_category;
		typedef typename eval_if<
			is_convertible<iterator_category, std::bidirectional_iterator_tag>,
				identity<roll_back_implementation::for_bidirectional_or_more_powerful_iterator_tag>
			,
				if_<
					is_convertible<iterator_category, std::forward_iterator_tag>,
						roll_back_implementation::for_forward_iterator_tag
					,
						roll_back_implementation::not_suitable_this_iterator_for_rollback_tag
				>
		>::type rollback_implementation_selected;
		rollback_implementation_t<rollback_implementation_selected>::rollback_implementation(begin, end);
	}

	//В отличии от std::uninitialized_copy, uninitialized_copy_and_rollback_if_exception вернет все созданные объекты, если не удалось создать их все, обратно в забвение.
	//Внимание! Порядок отката объетов прямой, а не обратный, если используется ForwardIterator, а не BidirectionalIterator или более властный!
	//Все объекты что создала, до достижения destination_end - оставит созданным.
	template<class InputIterator, class ForwardIterator>
	uninitialized_copy_result_t<ForwardIterator> uninitialized_copy_and_rollback_if_exception(InputIterator source_begin, const InputIterator& source_end,
		ForwardIterator destination_begin, const ForwardIterator& destination_end )
	{
		return containers_utility_detail::uninitialized_copy_and_rollback_if_exception_implementation(source_begin, source_end, destination_begin, std::bind1st(std::not_equal_to<ForwardIterator>(), destination_end));
	}

	//Аналогично uninitialized_copy_and_rollback_if_exception, но без контроля переполнения, как и в std::uninitialized_copy.
	template<class InputIterator, class ForwardIterator>
	uninitialized_copy_result_t<ForwardIterator> uninitialized_copy_and_rollback_if_exception(InputIterator source_begin, const InputIterator& source_end,
		ForwardIterator destination_begin)
	{
		struct _ 
		{
			static bool always_true(const ForwardIterator &)
			{
				return true;
			}
		};
		return containers_utility_detail::uninitialized_copy_and_rollback_if_exception_implementation(source_begin, source_end, destination_begin, _::always_true);
	}

	//Аналогична uninitialized_copy_and_rollback_if_exception, но откатит в случае переполнения (достижение destination_end), уничтожив все объекты, что создала.
	template<class InputIterator, class ForwardIterator>
	uninitialized_copy_result_t<ForwardIterator> uninitialized_copy_and_rollback_if_exception_or_overflow(const InputIterator& source_begin, const InputIterator& source_end,
		const ForwardIterator& destination_begin, const ForwardIterator& destination_end )
	{
		typedef const uninitialized_copy_result_t<ForwardIterator> result_t;
		result_t result=containers_utility::uninitialized_copy_and_rollback_if_exception(source_begin, source_end, destination_begin, destination_end);
		if(!(result.m_transaction_fully_successful))
		{
			containers_utility::rollback(destination_begin, destination_end);			
			result_t result=
			{
				destination_begin,
				0,
				false
			};
			return result;
		}
		return result;
	}
}

#endif // containers_utility_h__
