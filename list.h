#ifndef _YHSTL_LIST_H_
#define _YHSTL_LIST_H_
//20220922 开始做list类
//20220923 主要完善list的内存构造与管理，加了push_back函数
//20220923 新加push_front,erase,pop_front,pop_back等操作
//20220924 sort未完善，等白天完善
//list类其实是一个环形双向链表，内含前后指针以及data数据
//list突出节点的概念
#include "iterator.h"
#include"allocator.h"
#include "construct.h"
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
		typedef iterator::size_type   size_type;       //以下是类中的嵌套型别定义
		typedef iterator::reference	  reference;
		typedef iterator::value_type  value_type;
		typedef iterator::pointer	  pointer;
		typedef iterator::difference_type   difference_type;
	public:
		//list的构造与内存管理

		//专属空间配置器，每次只分配一个节点大小
		typedef allocator<list_node>  list_node_allocator;
		//list可以有很多构造函数，可以拿默认构造函数做参考
		//默认构造函数  可构造出空list
		list() { empty_initialize(); }      //empty...函数在下面实现


		//插入
		void push_back(const T& x)
		{
			insert(end(), x);
		}

		//插入一个节点，作为头结点
		void push_front(const T& x) { insert(begin(), x); }

		//移除迭代器position所指节点,然后返回这个位置之后的元素
		iterator erase(iterator position)
		{
			link_type next_node = link_type(position.node->next);
			link_type prev_node = link_type(position.node->prev);
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destroy_node(position.node);
			return iterator(next_node);
		}

		//移除头节点
		void pop_front()
		{
			erase(begin());
		}
		//移除尾结点  其实就注意一下end是尾后迭代器的概念
		void pop_back() 
		{
			iterator tmp = end();
			erase(--tmp);
		}
		//清空所有节点   
		void clear()
		{
			link_type cur = (link_type)node->next;  //以此找到begin
			while (cur != node)  //环形链表以此为判断遍历结束依据，开始遍历咯
			{
				link_type tmp = cur;
				cur = (link_type)cur->next;
				destroy_node(tmp);  //遍历一下，销毁并释放一个节点
			}
			//恢复node原始状态
			node->next = node;
			node->prev = node;
		}

		//将数值为value的所有元素移除   实现思路就一个简单的遍历
		void remove(const T& value)
		{
			iterator first = begin();
			iterator last = end();
			while (first != last)
			{
				iterator next = first;
				++next;
				if (*first == value) erase(first);
				first = next;
			}
		}

		//移除数值相同的连续元素，注意，只有连续且相同的元素才会被移除，最后只剩一个
		void unique()
		{
			iterator frist = begin();
			iterator last = end();
			if (first == last) return;  //空链表直接返回
			iterator next = first;
			while (++next != last)   //这样设计很便捷，直接用++next!=last作为判断条件
			{
				if (*first == *next) erase(next);  //如果相邻两节点相同，则移除next
				else first = next;		//不相同，则更新first为next
				next = first;	 //更新next节点
			}
		}

		//list特有的splice接合操作，其实是封装了transfer操作  下面是诸多重载splice操作
		void splice(iterator position, list&x)
		{
			if (!x.empty()) transfer(transfer(position, x.begin(), x.end()));
		}
		void splice(iterator position, list&, iterator i)   //把i迭代器所指接到position前面
		{
			iterator j = i;
			j++;
			if (i == position || j == position) return;  //i跟position冲突或i本来就在position前面直接返回！
			transfer(position, i, j);  //j主要是用来维护右区间，左闭右开嘛
		}

		//list特有的merge操作，同样是封装了transfer
		void merge(list& x)
		{
			iterator first1 = begin();
			iterator last1 = end();
			iterator first2 = x.begin();
			iterator last2 = x.end();

			//注意merge前提是两个链表都是有序链表（从小到大）
			//其实list的merge操作是用到了归并排序的知识
			while (first1 != last1 && first2 != last2)   
			{
				if (*first2 < *first1)    //注意merge操作是调用merge的链表上进行的，所以是把第二个链表接到这个调用对象上
				{
					iterator next = first2;
					transfer(first1, first2, ++next);
					first2 = next;
				}
				else ++first1;
				
			}
			//最后如果list2还存在，直接全部迁移到list2的last前
			if (first2 != last2) transfer(last1, first2, last2);  //STL源码剖析，该行代码放在while循环里了			
		}   

		//翻转链表，reverse将*this的内容逆向重置
		void reverse()
		{
			if (node->next = node || link_type(node->next)-> == node) return;  //链表为空或只有一个元素，直接返回
			iterator first = begin();
			++first;
			while (first != end())
			{
				iterator old = first;
				++first;
				transfer(begin(), old, first);
			}
		}

		//list不能使用STL的sort，必须要创建自己的sort成员函数
		//list的sort函数使用快排 
		void  sort()
		{
			if (node->next = node || link_type(node->next)-> == node) return;  //链表为空或只有一个元素，直接返回
			//需要一些新的list，作为中介数据存放区
			list carry;
			list counter[64];
			int fill = 0;

			while (!empty())
			{
				carry.splice(carry.begin(), *this, begin());
				int i = 0;
				while (i < fill && !counter->empty())
				{
					counter[i].merge(carry);
					carry.swap(counter[i++]);
				}
				carry.swap(counter[i]);
				if (i == fill) ++fill;
			}

			for (int i = 1; i < fill; ++i)
			{
				counter[i].merge(counter[i - 1]);
			}
			swap(counter[fill - 1]);
		}
	protected:
		//配置一个空间大小并传回
		link_type get_node() { return list_node_allocator::allocate(); }
		//释放一个节点
		void put_node(link_type p) { return list_node_allocator::deallocate(p); }

		//产生一个节点，并配有元素值      先分配空间，再构造
		link_type create_node(const T& x)
		{
			link_type p = get_node();
			construct(&p->data, x); 
			return p;
		}
		//析构并释放一个节点			  先析构，再释放空间
		void destroy_node(link_type p)
		{
			destroy(&p->data);
			put_node();
		}
  
		//实现空链表的默认构造函数    是构造双向环形链表
		void empty_initialize()
		{
			node = get_node();    //分配一个链表节点空间，并让node指向它
			node->next = node;	  //令这个节点头尾指向自己，不设元素值
			node->prev = node;
		}

		//insert函数的实现  insert是一个重载函数有多种形式，下面这种满足 push_back函数的需求
		iterator insert(iterator position, const T& x)
		{
			link_type tmp = create_node(x);   //先创建一个临时节点，并构造元素x
			//调整双向指针，把这个tmp节点插入原链表尾部
			//下面是双向链表的正常逻辑，改变四个指向
			tmp->next = position.node;  
			tmp->prev = position.node->prev;   
			(link_type)(position.node->prev)->next = tmp;
			positon.node->prev = tmp;
			return tmp;
		}

		//list内部有个所谓的迁移操作：将某连续范围的元素迁移到某个特定位置之前
		//将[first,last)(前闭后开)的所有元素迁移到position之前
		//这个函数为list的特色函数
		void transfer(iterator position, iterator first, iterator last)
		{
			if (position != last)
			{
				//下面七行的代码的顺序很有讲究，要注意哦！
				(*(link_type)((*last.node).prev)).next = position.node;  //尾部节点next指position
				(*(link_type)((*first.node).prev)).next = last.node;	 //first前节点的next指向last（last为开区间，不参与）
				(*(link_type)((*position.node).prev)).next = first.node; //position前节点的next指向first
				link_type tmp = link_type((*position.node)->prev);       //设置一临时节点指向position前节点
				(*position.node)->prev = (*last.node)->prev;			 //positon的prev指向last的前节点
				(*last.node).prev = (*first.node).prev;				     //last节点的prev指向first的prev
				(*first.node).prev = tmp;						         //first的prev指向原position的前节点
			}
		}
		
	protected:
		link_type	node;   //关键节点元素


	public:
		iterator begin() { return (link_type)((*node)->next) }
		iterator end() { return node; }
		bool empty()const  {return node->next==node}
		size_type  size() const {
			size_type result = 0;
			distance(begin(), end(),result);    //全局函数，在iterator头文件中实现
			return result;
		}
		//取头结点中的元素
		reference front() const { return *begin(); }
		//取尾结点的元素
		reference back() const { return *(--end()); }


	};


}

#endif
