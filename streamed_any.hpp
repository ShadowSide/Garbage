#ifndef streamed_any_h__
#define streamed_any_h__
#include <Common/accessory/accessory.h>
#include <boost/spirit/home/support/detail/hold_any.hpp>

//TWStreamedAny\TStreamedAny - хранилище для экземпляра произвольного типа, имеющего перегруженные операторы ввода-вывода << >>.
//По TWStreamedAny\TStreamedAny смотри документацию по boost::any:
// http://www.boost.org/doc/libs/1_54_0/doc/html/any.html
//На самом деле для реализации используется boost::spirit::basic_hold_any<Char> -
//неофициальный boost::any и он может в любой момент пропасть из boost.
//Если это произойдет, то реализуем или скопируем старый из библиотеки спирита, а пока чтобы не тратить время воспользуемся готовым.
//Пришлось это сделать, тк boost::any не поддерживает ввод-вывод в поток, кроме негибкого lexical_cast.
//В отличии от boost::any,
//TWStreamedAny можно отправлять в wide stream (широкий поток ввода-вывода), например в std::wcout и
//TStreamedAny можно отправлять в stream (поток ввода-вывода), например в std::cout.
//Также он более производительный.
//Для преобразования в произвольный тип используй any_cast. Можно также просто отправлять в поток ввода-вывода.

namespace NStreamedAny
{
	typedef boost::spirit::whold_any TWStreamedAny;
	//typedef boost::spirit::hold_any TStreamedAny;//Закоментировал, тк пока негде использовать. Можно разкомментировать.
	using boost::spirit::any_cast;
	using boost::spirit::bad_any_cast;
	using boost::spirit::basic_hold_any;
}

#endif // streamed_any_h__
