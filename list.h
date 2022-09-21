#ifndef _YHSTL_LIST_H_
#define _YHSTL_LIST_H_
//20220922 开始做list类
//list类其实是一个环形双向链表，内含前后指针以及data数据
//list突出节点的概念
#include "iterator.h"
namespace yhstl
{
	//先做list节点的设计，包含前后指针和data数据
	template <class T>
	struct _list_node
	{
		typedef void* void_pointer;		//指针
		void_pointer  prev;				//指向前一个节点的指针
		void_pointer  next;				//指向后一个节点的指针
		T			  data;				//list节点的数据
	};


	//下面是list迭代器的设计
	template<class T,class Ref,class Ptr>
	struct _list_iterator
	{
		typedef _list_iterator<T, T&, T*>			iterator;
		typedef _list_iterator<T, Ref, Ptr>			self;	    //self是自身

		typedef yhstl::bidirectional_iterator_tag	iterator_category;   //因为list是一个双向链表，迭代器要支持前移后移的能力
		typedef T				value_type;
		typedef Ptr				pointer;
		typedef Ref				reference;
		typedef _list_node<T>*  link_type;	
		typedef size_t			size_type;
		typedef ptrdiff_t		difference_type;

		link_type	node;   //迭代器内部肯定是要有一个普通指针来指向节点的
		//一个小知识点，一个空类会默认构造：缺省构造函数、拷贝构造、析构、赋值
		//构造函数 constructor
		_list_iterator(link_type x) :node(x) {}   //列表初始化,相比赋值省了构造临时对象的过程
		_list_iterator() {}
		_list_iterator(const iterator& x) :node(x.node) {} //传入的是const 迭代器

		//赋值运算符
		bool operator==(const self& x) const { return node == x.node; }  //类其实包含一个this指针
		bool operator==(const self& x) const { return node != x.node; }

		//以下是对迭代器取值，取的是node节点的值
		reference operator*()const { return (*node).data; }  //对*重载，返回的是引用

		//以下是对箭头运算符重载,是迭代器成员取放运算子的标准用法，即使用operator *
		pointer operator->()const { return &(operator*()); } 

		//对迭代器加一，就是做前移操作     都是先做前置++，再用后置++调用前置++
		self& operator++()			//返回自身，对自身自加一    是++i操作
		{
			node = (link_type)((*node).text);   //text返回之后节点的指针，还要强制类型转换为link_type类型
			return *this;
		}		
		self operator++(int)
		{
			self tmp = *this;   //是 i++操作，先创建一个临时对象，再自加，再返回临时对象
			++* this;
			return tmp;
		}

		//对迭代器减一，就是后退一个节点
		self& operator--()			//返回自身，对自身自减一    是--i操作
		{
			node = (link_type)((*node).prev);   //text返回之后节点的指针，还要强制类型转换为link_type类型
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;   //是 i++操作，先创建一个临时对象，再自加，再返回临时对象
			--* this;
			return tmp;
		}


	};


	//接着是list的数据结构
	template<class T>
	class list
	{
	protected:
		typedef _list_node<T>	 list_node;
	public:
		typedef list_node*		 link_type;
		typedef _list_iterator   iterator;    //20220922 0:05 今天就到这儿吧，加油

	protected:
		link_type	node;
	};


}

#endif
