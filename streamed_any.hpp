#ifndef streamed_any_h__
#define streamed_any_h__
#include <Common/accessory/accessory.h>
#include <boost/spirit/home/support/detail/hold_any.hpp>

//TWStreamedAny\TStreamedAny - ��������� ��� ���������� ������������� ����, �������� ������������� ��������� �����-������ << >>.
//�� TWStreamedAny\TStreamedAny ������ ������������ �� boost::any:
// http://www.boost.org/doc/libs/1_54_0/doc/html/any.html
//�� ����� ���� ��� ���������� ������������ boost::spirit::basic_hold_any<Char> -
//������������� boost::any � �� ����� � ����� ������ �������� �� boost.
//���� ��� ����������, �� ��������� ��� ��������� ������ �� ���������� �������, � ���� ����� �� ������� ����� ������������� �������.
//�������� ��� �������, �� boost::any �� ������������ ����-����� � �����, ����� ��������� lexical_cast.
//� ������� �� boost::any,
//TWStreamedAny ����� ���������� � wide stream (������� ����� �����-������), �������� � std::wcout �
//TStreamedAny ����� ���������� � stream (����� �����-������), �������� � std::cout.
//����� �� ����� ����������������.
//��� �������������� � ������������ ��� ��������� any_cast. ����� ����� ������ ���������� � ����� �����-������.

namespace NStreamedAny
{
	typedef boost::spirit::whold_any TWStreamedAny;
	//typedef boost::spirit::hold_any TStreamedAny;//��������������, �� ���� ����� ������������. ����� �����������������.
	using boost::spirit::any_cast;
	using boost::spirit::bad_any_cast;
	using boost::spirit::basic_hold_any;
}

#endif // streamed_any_h__
