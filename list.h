#ifndef _YHSTL_LIST_H_
#define _YHSTL_LIST_H_
//20220922 ��ʼ��list��
//20220923 ��Ҫ����list���ڴ湹�����������push_back����
//list����ʵ��һ������˫�������ں�ǰ��ָ���Լ�data����
//listͻ���ڵ�ĸ���
#include "iterator.h"
#include"allocator.h"
#include "construct.h"
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
		typedef iterator::size_type   size_type;       //���������е�Ƕ���ͱ���
		typedef iterator::reference	  reference;
		typedef iterator::value_type  value_type;
		typedef iterator::pointer	  pointer;
		typedef iterator::difference_type   difference_type;
	public:
		//list�Ĺ������ڴ����

		//ר���ռ���������ÿ��ֻ����һ���ڵ��С
		typedef allocator<list_node>  list_node_allocator;
		//list�����кܶ๹�캯����������Ĭ�Ϲ��캯�����ο�
		//Ĭ�Ϲ��캯��  �ɹ������list
		list() { empty_initialize(); }      //empty...����������ʵ��


		//����
		void push_back(const T& x)
		{
			insert(end(), x);
		}
	protected:
		//����һ���ռ��С������
		link_type get_node() { return list_node_allocator::allocate(); }
		//�ͷ�һ���ڵ�
		void put_node(link_type p) { return list_node_allocator::deallocate(p); }

		//����һ���ڵ㣬������Ԫ��ֵ      �ȷ���ռ䣬�ٹ���
		link_type create_node(const T& x)
		{
			link_type p = get_node();
			construct(&p->data, x); 
			return p;
		}
		//�������ͷ�һ���ڵ�			  �����������ͷſռ�
		void destroy_node(link_type p)
		{
			destroy(&p->data);
			put_node();
		}
  
		//ʵ�ֿ������Ĭ�Ϲ��캯��    �ǹ���˫��������
		void empty_initialize()
		{
			node = get_node();    //����һ������ڵ�ռ䣬����nodeָ����
			node->next = node;	  //������ڵ�ͷβָ���Լ�������Ԫ��ֵ
			node->prev = node;
		}

		//insert������ʵ��  insert��һ�����غ����ж�����ʽ�������������� push_back����������
		iterator insert(iterator position, const T& x)
		{
			link_type tmp = create_node(x);   //�ȴ���һ����ʱ�ڵ㣬������Ԫ��x
			//����˫��ָ�룬�����tmp�ڵ����ԭ����β��
			//������˫������������߼����ı��ĸ�ָ��
			tmp->next = position.node;  
			tmp->prev = position.node->prev;   
			(link_type(position.node->prev)->next = tmp;
			positon.node->prev = tmp;
			return tmp;
		}

	protected:
		link_type	node;


	public:
		iterator begin() { return (link_type)((*node)->next) }
		iterator end() { return node; }
		bool empty()const  {return node->next==node}
		size_type  size() const {
			size_type result = 0;
			distance(begin(), end(),result);    //ȫ�ֺ�������iteratorͷ�ļ���ʵ��
			return result;
		}
		//ȡͷ����е�Ԫ��
		reference front() const { return *begin(); }
		//ȡβ����Ԫ��
		reference back() const { return *(--end()); }
	};


}

#endif
