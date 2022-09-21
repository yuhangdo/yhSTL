#ifndef _YHSTL_LIST_H_
#define _YHSTL_LIST_H_
//20220922 ��ʼ��list��
//list����ʵ��һ������˫�������ں�ǰ��ָ���Լ�data����
//listͻ���ڵ�ĸ���
#include "iterator.h"
namespace yhstl
{
	//����list�ڵ����ƣ�����ǰ��ָ���data����
	template <class T>
	struct _list_node
	{
		typedef void* void_pointer;		//ָ��
		void_pointer  prev;				//ָ��ǰһ���ڵ��ָ��
		void_pointer  next;				//ָ���һ���ڵ��ָ��
		T			  data;				//list�ڵ������
	};


	//������list�����������
	template<class T,class Ref,class Ptr>
	struct _list_iterator
	{
		typedef _list_iterator<T, T&, T*>			iterator;
		typedef _list_iterator<T, Ref, Ptr>			self;	    //self������

		typedef yhstl::bidirectional_iterator_tag	iterator_category;   //��Ϊlist��һ��˫������������Ҫ֧��ǰ�ƺ��Ƶ�����
		typedef T				value_type;
		typedef Ptr				pointer;
		typedef Ref				reference;
		typedef _list_node<T>*  link_type;	
		typedef size_t			size_type;
		typedef ptrdiff_t		difference_type;

		link_type	node;   //�������ڲ��϶���Ҫ��һ����ָͨ����ָ��ڵ��
		//һ��С֪ʶ�㣬һ�������Ĭ�Ϲ��죺ȱʡ���캯�����������졢��������ֵ
		//���캯�� constructor
		_list_iterator(link_type x) :node(x) {}   //�б��ʼ��,��ȸ�ֵʡ�˹�����ʱ����Ĺ���
		_list_iterator() {}
		_list_iterator(const iterator& x) :node(x.node) {} //�������const ������

		//��ֵ�����
		bool operator==(const self& x) const { return node == x.node; }  //����ʵ����һ��thisָ��
		bool operator==(const self& x) const { return node != x.node; }

		//�����ǶԵ�����ȡֵ��ȡ����node�ڵ��ֵ
		reference operator*()const { return (*node).data; }  //��*���أ����ص�������

		//�����ǶԼ�ͷ���������,�ǵ�������Աȡ�������ӵı�׼�÷�����ʹ��operator *
		pointer operator->()const { return &(operator*()); } 

		//�Ե�������һ��������ǰ�Ʋ���     ��������ǰ��++�����ú���++����ǰ��++
		self& operator++()			//���������������Լ�һ    ��++i����
		{
			node = (link_type)((*node).text);   //text����֮��ڵ��ָ�룬��Ҫǿ������ת��Ϊlink_type����
			return *this;
		}		
		self operator++(int)
		{
			self tmp = *this;   //�� i++�������ȴ���һ����ʱ�������Լӣ��ٷ�����ʱ����
			++* this;
			return tmp;
		}

		//�Ե�������һ�����Ǻ���һ���ڵ�
		self& operator--()			//���������������Լ�һ    ��--i����
		{
			node = (link_type)((*node).prev);   //text����֮��ڵ��ָ�룬��Ҫǿ������ת��Ϊlink_type����
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;   //�� i++�������ȴ���һ����ʱ�������Լӣ��ٷ�����ʱ����
			--* this;
			return tmp;
		}


	};


	//������list�����ݽṹ
	template<class T>
	class list
	{
	protected:
		typedef _list_node<T>	 list_node;
	public:
		typedef list_node*		 link_type;
		typedef _list_iterator   iterator;    //20220922 0:05 ����͵�����ɣ�����

	protected:
		link_type	node;
	};


}

#endif
