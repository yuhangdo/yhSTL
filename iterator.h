#ifndef _YHSTL_ITERATOR_H_
#define _YHSTL_ITERATOR_H_
#include<cstddef>      //加入这个头文件，主要是用到   _WIN64 typedef __int64          ptrdiff_t;这些数据类型
#include "type_traits.h"   //后续要实现 type_traits.h
namespace yhstl
{
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// iterator 模板
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
