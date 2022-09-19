#ifndef _YHSTL_ITERATOR_H_
#define _YHSTL_ITERATOR_H_
#include<cstddef>      //�������ͷ�ļ�����Ҫ���õ�   _WIN64 typedef __int64          ptrdiff_t;��Щ��������
#include "type_traits.h"   //����Ҫʵ�� type_traits.h
namespace yhstl
{
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// iterator ģ��
	template <class Category, class T, class Distance = ptrdiff_t,class Pointer = T*, class Reference = T&>
	struct iterator
	{
		typedef Category                             iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
	};

}
#endif
