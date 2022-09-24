#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  今日开始deque容器实现  先做部分迭代器

#include"iterator.h"

namespace yhstl
{
	//一个全局函数：_deque_buf_size 来决定deque的缓冲区大小
	//如果n不为0，则就是n，表示缓冲区大小为用户指定的n
	//如果n为0，则使用默认值，如果size(T)>512则为1，否则为512/size(T)
	inline  size_t _deque_buf_size(size_t n, size_t sz)
	{
		return n != 0 ? n : (sz < 512 ? sizeof(512 / sz) : sizeof(1));
	}

	//deque的迭代器
	template <class T,class Ref,class Ptr,size_t BufSiz>
	struct _deque_iterator    //没有继承iterator
	{
		typedef _deque_iterator<T, &T, T*, BufSiz> iterator;
		typedef _deque_iterator<T, const&T, const T*, BufSiz> const_iterator;
		static size_t buffer_size() { return _deque_buf_size(BufSiz, sizeof(T)); }  //内含一个全局函数

		//未继承 yhstl::iterator，所以必须自己定义五个必要相应迭代器型别
		typedef random_access_iterator_tag           iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
		typedef T**		map_pointer;
		typedef _deque_iterator self;

		//保持与容器的联结
		T* cur;
		T* first;
		T* last;
		map_pointer map;   //指向管控中心
	};
	
}


#endif