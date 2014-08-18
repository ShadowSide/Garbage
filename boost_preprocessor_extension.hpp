#ifndef boost_preprocessor_extension_h__
#define boost_preprocessor_extension_h__
#include <boost/move/move.hpp>// Ќужно дл€ части макросов, чтобы результат их кодогенерации скомпилировалс€.
#include <boost/preprocessor.hpp>


//ћакроподстановки:

//expands to "class ...TArgs" i.e. (а именно) "class TArg0, class TArg1, class TArg2 ... , class TArgARG_COUNTER-1"
#define PP_TEMPLATE_ARGS(ARG_COUNTER) BOOST_PP_ENUM_PARAMS(ARG_COUNTER, class TArg)

//expands to ", class ...TArgs" i.e. ", class TArg0, class TArg1, class TArg2 ... , class TArgARG_COUNTER-1".
//Comma at begin of lexical construction is a difference between this macro and last macro.
//TRAILING-suffix at macro name - attribute of comma begin macro substitution.
#define PP_TEMPLATE_TRAILING_ARGS(ARG_COUNTER)  BOOST_PP_COMMA_IF(ARG_COUNTER) PP_TEMPLATE_ARGS(ARG_COUNTER)

//expands to "template<class ...TArgs>" i.e. "template<class TArg0, class TArg1, class TArg2 ... , class TArgARG_COUNTER-1>"
#define PP_FULL_TEMPLATE_ARGS(ARG_COUNTER) BOOST_PP_EXPR_IF(ARG_COUNTER, template<) PP_TEMPLATE_ARGS(ARG_COUNTER) BOOST_PP_EXPR_IF(ARG_COUNTER, >)

#define INTERNAL_PP_FUNCTION_FORWARD_ARGS_INTERNAL(_, ARG_COUNTER, MAX_ARGUMENT) BOOST_FWD_REF(BOOST_PP_CAT(TArg, ARG_COUNTER)) BOOST_PP_CAT(arg, ARG_COUNTER) BOOST_PP_IF(BOOST_PP_AND(BOOST_PP_EQUAL(BOOST_PP_DEC(MAX_ARGUMENT), ARG_COUNTER), MAX_ARGUMENT), BOOST_PP_EMPTY, BOOST_PP_COMMA)()
//expands to "BOOST_FWD_REF(TArgs)... args" i.e. "BOOST_FWD_REF(TArg0) arg0, BOOST_FWD_REF(TArg1) arg1, BOOST_FWD_REF(TArg2) arg2 ... BOOST_FWD_REF(TArgARG_COUNTER-1) argARG_COUNTER-1"
//BOOST_FWD_REF - see in Boost.Move library documentation, for example at http://www.boost.org/doc/html/move/construct_forwarding.html
#define PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER) BOOST_PP_REPEAT(ARG_COUNTER, INTERNAL_PP_FUNCTION_FORWARD_ARGS_INTERNAL, ARG_COUNTER)

//expands to ", BOOST_FWD_REF(TArgs)... args" i.e. ", BOOST_FWD_REF(TArg0) arg0, BOOST_FWD_REF(TArg1) arg1, BOOST_FWD_REF(TArg2) arg2 ... BOOST_FWD_REF(TArgARG_COUNTER-1) argARG_COUNTER-1"
#define PP_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) BOOST_PP_COMMA_IF(ARG_COUNTER) PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER)

#define INTERNAL_PP_FUNCTION_CALL_FORWARD_ARGS_INTERNAL(_, ARG_COUNTER, MAX_ARGUMENT) boost::forward<BOOST_PP_CAT(TArg, ARG_COUNTER)>(BOOST_PP_CAT(arg, ARG_COUNTER)) BOOST_PP_IF(BOOST_PP_AND(BOOST_PP_EQUAL(BOOST_PP_DEC(MAX_ARGUMENT), ARG_COUNTER), MAX_ARGUMENT), BOOST_PP_EMPTY, BOOST_PP_COMMA)()
//expands to "boost::forward<TArgs>(args)..." i.e. "boost::forward<TArg0>(arg0), boost::forward<TArg1>(arg1), boost::forward<TArg2>(arg2), ... boost::forward<TArgARG_COUNTER-1>(argARG_COUNTER-1)"
//boost::forward - see in Boost.Move library documentation, for example at http://www.boost.org/doc/html/move/construct_forwarding.html
#define PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER) BOOST_PP_REPEAT(ARG_COUNTER, INTERNAL_PP_FUNCTION_CALL_FORWARD_ARGS_INTERNAL, ARG_COUNTER)

//expands to ", boost::forward<TArgs>(args)..." i.e. ", boost::forward<TArg0>(arg0), boost::forward<TArg1>(arg1), boost::forward<TArg2>(arg2), ... boost::forward<TArgARG_COUNTER-1>(argARG_COUNTER-1)"
#define PP_CALL_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) BOOST_PP_COMMA_IF(ARG_COUNTER) PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER)

//expands to "explicit" при нуле дополнительных аргументов.
//ѕодходит дл€ конструкторов, если есть один аргумент, за которым следует PP_FUNCTION_FORWARD_TRAILING_ARGS (дополнительные аргументы).
#define PP_EXPLICIT_IF_ZERO(ARG_COUNTER) BOOST_PP_EXPR_IF(BOOST_PP_NOT(ARG_COUNTER), explicit)
#endif // boost_preprocessor_extension_h__
