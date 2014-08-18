#ifndef unique_ptr__h__
#define unique_ptr__h__

#include<Common\accessory\accessory.h>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/move/move.hpp>
#include<Common\ForBoost\default_delete_.h>
#include <boost/preprocessor.hpp>

template<class TPointed, class deleter = ::default_delete_<TPointed> >
struct unique_ptr_type_generator_
{
	typedef ::boost::interprocess::unique_ptr<TPointed, deleter > type;
};

namespace boost
{
	namespace interprocess 
	{
		/*//Закомментировал, тк не используется.
		//See std::make_shared:
		#define MAX_ARGS_AT_MAKE_UNIQUE 8

		//Ниже генерируется функция:
		/*template< class T, class... Args >
		boost::interprocess::unique_ptr<T, ::default_delete_<T> > make_unique_(const Args&... args )
		{
			return boost::interprocess::unique_ptr<T, ::default_delete_<T> >(new T(args));
		}*//*

		#define MAKE_UNIQUE_GENERATE_CODE(_, ArgsCount, __)																								\
		template< class T BOOST_PP_COMMA_IF(ArgsCount) BOOST_PP_ENUM_PARAMS(ArgsCount, class TArg) >													\
		typename unique_ptr_type_generator_<T>::type make_unique_(BOOST_PP_ENUM_BINARY_PARAMS(ArgsCount, const TArg, & Arg ))							\
		{																																				\
			return typename unique_ptr_type_generator_<T>::type(new T(BOOST_PP_ENUM_PARAMS(ArgsCount, Arg)));											\
		}

		BOOST_PP_REPEAT(MAX_ARGS_AT_MAKE_UNIQUE, MAKE_UNIQUE_GENERATE_CODE, _)
		#undef MAKE_UNIQUE_GENERATE_CODE
		*/



		/*//Закомментировал, тк не используется.
		//See std::static_pointer_cast, std::const_pointer_cast и std::dynamic_pointer_cast:
		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> static_pointer_cast_and_move(boost::interprocess::unique_ptr<TSourceType, TDeleter>& source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(static_cast<TDestinationType*>(source.release()));
		}
		
		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> static_pointer_cast_and_move(BOOST_RV_REF_2_TEMPL_ARGS(boost::interprocess::unique_ptr, TSourceType, TDeleter) source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(static_cast<TDestinationType*>(source.release()));
		}

		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> const_pointer_cast_and_move(boost::interprocess::unique_ptr<TSourceType, TDeleter>& source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(const_cast<TDestinationType*>(source.release()));
		}

		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> const_pointer_cast_and_move(BOOST_RV_REF_2_TEMPL_ARGS(boost::interprocess::unique_ptr, TSourceType, TDeleter) source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(const_cast<TDestinationType*>(source.release()));
		}

		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> polymorphic_downcast_pointer_cast_and_move(boost::interprocess::unique_ptr<TSourceType, TDeleter>& source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(::boost::polymorphic_downcast<TDestinationType*>(source.release()));
		}

		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> polymorphic_downcast_pointer_cast_and_move(BOOST_RV_REF_2_TEMPL_ARGS(boost::interprocess::unique_ptr, TSourceType, TDeleter) source)
		{
			return boost::interprocess::unique_ptr<TDestinationType, TDeleter>(::boost::polymorphic_downcast<TDestinationType*>(source.release()));
		}

		//Temporary moved source. You should be more accuracy at multithreaded environment.
		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> dynamic_pointer_cast_and_move_if_success(boost::interprocess::unique_ptr<TSourceType, TDeleter>& source)
		{
			boost::interprocess::unique_ptr<TSourceType, TDeleter> temporaryMoved(boost::move(source));
			try{
				TDestinationType* const Casted = dynamic_cast<TDestinationType*>(&*temporaryMoved);
				boost::interprocess::unique_ptr<TDestinationType, TDeleter> result(Casted);
				if(Casted!=NULL)
					temporaryMoved->release();
				else
					source = boost::move(temporaryMoved);
				return result;
			}catch(std::bad_cast&){
				source = boost::move(temporaryMoved);
				throw;
			}catch(std::exception&){
				source = boost::move(temporaryMoved);
				SHOULD_NEVER_REACH_HERE;
				throw;
			}
		}

		//Don't temporary move source. Need critical section at multithreaded environment, because at other thread can move this pointer too and delete before and this function.
		template<class TDestinationType, class TSourceType, class TDeleter>
		boost::interprocess::unique_ptr<TDestinationType, TDeleter> dynamic_pointer_cast_and_move_if_success(BOOST_RV_REF_2_TEMPL_ARGS(boost::interprocess::unique_ptr, TSourceType, TDeleter) source)
		{
			TDestinationType* const Casted = dynamic_cast<TDestinationType*>(&*source);
			boost::interprocess::unique_ptr<TDestinationType, TDeleter> result(Casted);
			if(Casted!=NULL)
				source->release();
			return result;
		}*/
	}
}

#endif // unique_ptr__h__
