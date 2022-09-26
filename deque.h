#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  今日开始deque容器实现  先做部分迭代器
//20220926  最近需要准备开题，finish 迭代器部分
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
		map_pointer node;   //指向管控中心


		//先加一个跳缓冲区的函数   跳一个缓冲区大小
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//接下来是迭代器的各种重载运算符操作
		//* 和 -> 运算符是老程序了
		reference operator*() { return *cur; }   
		pointer operator->() const { return &(operator*()); }

		difference_type operator -(self& x)const    //这个重载-的逻辑挺有意思
		{
			return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
		}
		self& operator++()    //要特判一下 在缓冲区边界的情况
		{
			++cur;
			if (cur == last)
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		self& operator++(int)   //后置++    后置调前置 标准写法
		{
			self tmp = *this;
			++* this;
			return tmp;
		}
		self& operator--()    //要特判一下 在缓冲区边界的情况
		{		
			if (cur == first)
			{
				set_node(node -1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self& operator--(int)   //后置--    后置调前置 标准写法
		{
			self tmp = *this;
			--* this;
			return tmp;
		}

		//因为是随机迭代器，支持随机存取，所以要实现一下
		self& operator+=(difference_type n)
		{
			difference_type offset = n + (cur - first);   //先算出跳跃距离
			if (offset >= 0 && offset < difference_type(buffer_size()))  //判断是不是在同一个缓冲区内
			{
				cur += n;
			}
			else
			{
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : -difference_type((-offset-1) /(buffer_size())-1;
				//切换到正确的节点
				set_node(node + node_offset);
				//切换到正确的元素
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}
		self operator+(difference_type  n)const   //迭代器的+
		{
			self tmp = *this;
			return tmp += n;
		}
		self operator-=(difference_type n) { return *this += -n };
		self operator-(difference_type  n)const   //迭代器的-
		{
			self tmp = *this;
			return tmp -= n;
		}

		//以下实现随机存取  调用 operator+
		reference operator[](difference_type n) const { return *(*this + n); }
		
		//以下调用 operator* operator+
		bool operator == (const self& x) { return cur == x.cur; }
		bool operator != (const self& x) { return !(*this == x); }
		bool operator<(const self& x)const
		{
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};
	
}


#endif