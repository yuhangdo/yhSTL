#ifndef _YHSTL_LIST_H_
#define _YHSTL_LIST_H_
//20220922 ��ʼ��list��
//20220923 ��Ҫ����list���ڴ湹�����������push_back����
//20220923 �¼�push_front,erase,pop_front,pop_back�Ȳ���
//20220924 sortδ���ƣ��Ȱ�������
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

		//����һ���ڵ㣬��Ϊͷ���
		void push_front(const T& x) { insert(begin(), x); }

		//�Ƴ�������position��ָ�ڵ�,Ȼ�󷵻����λ��֮���Ԫ��
		iterator erase(iterator position)
		{
			link_type next_node = link_type(position.node->next);
			link_type prev_node = link_type(position.node->prev);
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destroy_node(position.node);
			return iterator(next_node);
		}

		//�Ƴ�ͷ�ڵ�
		void pop_front()
		{
			erase(begin());
		}
		//�Ƴ�β���  ��ʵ��ע��һ��end��β��������ĸ���
		void pop_back() 
		{
			iterator tmp = end();
			erase(--tmp);
		}
		//������нڵ�   
		void clear()
		{
			link_type cur = (link_type)node->next;  //�Դ��ҵ�begin
			while (cur != node)  //���������Դ�Ϊ�жϱ����������ݣ���ʼ������
			{
				link_type tmp = cur;
				cur = (link_type)cur->next;
				destroy_node(tmp);  //����һ�£����ٲ��ͷ�һ���ڵ�
			}
			//�ָ�nodeԭʼ״̬
			node->next = node;
			node->prev = node;
		}

		//����ֵΪvalue������Ԫ���Ƴ�   ʵ��˼·��һ���򵥵ı���
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

		//�Ƴ���ֵ��ͬ������Ԫ�أ�ע�⣬ֻ����������ͬ��Ԫ�زŻᱻ�Ƴ������ֻʣһ��
		void unique()
		{
			iterator frist = begin();
			iterator last = end();
			if (first == last) return;  //������ֱ�ӷ���
			iterator next = first;
			while (++next != last)   //������ƺܱ�ݣ�ֱ����++next!=last��Ϊ�ж�����
			{
				if (*first == *next) erase(next);  //����������ڵ���ͬ�����Ƴ�next
				else first = next;		//����ͬ�������firstΪnext
				next = first;	 //����next�ڵ�
			}
		}

		//list���е�splice�Ӻϲ�������ʵ�Ƿ�װ��transfer����  �������������splice����
		void splice(iterator position, list&x)
		{
			if (!x.empty()) transfer(transfer(position, x.begin(), x.end()));
		}
		void splice(iterator position, list&, iterator i)   //��i��������ָ�ӵ�positionǰ��
		{
			iterator j = i;
			j++;
			if (i == position || j == position) return;  //i��position��ͻ��i��������positionǰ��ֱ�ӷ��أ�
			transfer(position, i, j);  //j��Ҫ������ά�������䣬����ҿ���
		}

		//list���е�merge������ͬ���Ƿ�װ��transfer
		void merge(list& x)
		{
			iterator first1 = begin();
			iterator last1 = end();
			iterator first2 = x.begin();
			iterator last2 = x.end();

			//ע��mergeǰ������������������������С����
			//��ʵlist��merge�������õ��˹鲢�����֪ʶ
			while (first1 != last1 && first2 != last2)   
			{
				if (*first2 < *first1)    //ע��merge�����ǵ���merge�������Ͻ��еģ������ǰѵڶ�������ӵ�������ö�����
				{
					iterator next = first2;
					transfer(first1, first2, ++next);
					first2 = next;
				}
				else ++first1;
				
			}
			//������list2�����ڣ�ֱ��ȫ��Ǩ�Ƶ�list2��lastǰ
			if (first2 != last2) transfer(last1, first2, last2);  //STLԴ�����������д������whileѭ������			
		}   

		//��ת����reverse��*this��������������
		void reverse()
		{
			if (node->next = node || link_type(node->next)-> == node) return;  //����Ϊ�ջ�ֻ��һ��Ԫ�أ�ֱ�ӷ���
			iterator first = begin();
			++first;
			while (first != end())
			{
				iterator old = first;
				++first;
				transfer(begin(), old, first);
			}
		}

		//list����ʹ��STL��sort������Ҫ�����Լ���sort��Ա����
		//list��sort����ʹ�ÿ��� 
		void  sort()
		{
			if (node->next = node || link_type(node->next)-> == node) return;  //����Ϊ�ջ�ֻ��һ��Ԫ�أ�ֱ�ӷ���
			//��ҪһЩ�µ�list����Ϊ�н����ݴ����
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
			(link_type)(position.node->prev)->next = tmp;
			positon.node->prev = tmp;
			return tmp;
		}

		//list�ڲ��и���ν��Ǩ�Ʋ�������ĳ������Χ��Ԫ��Ǩ�Ƶ�ĳ���ض�λ��֮ǰ
		//��[first,last)(ǰ�պ�)������Ԫ��Ǩ�Ƶ�position֮ǰ
		//�������Ϊlist����ɫ����
		void transfer(iterator position, iterator first, iterator last)
		{
			if (position != last)
			{
				//�������еĴ����˳����н�����Ҫע��Ŷ��
				(*(link_type)((*last.node).prev)).next = position.node;  //β���ڵ�nextָposition
				(*(link_type)((*first.node).prev)).next = last.node;	 //firstǰ�ڵ��nextָ��last��lastΪ�����䣬�����룩
				(*(link_type)((*position.node).prev)).next = first.node; //positionǰ�ڵ��nextָ��first
				link_type tmp = link_type((*position.node)->prev);       //����һ��ʱ�ڵ�ָ��positionǰ�ڵ�
				(*position.node)->prev = (*last.node)->prev;			 //positon��prevָ��last��ǰ�ڵ�
				(*last.node).prev = (*first.node).prev;				     //last�ڵ��prevָ��first��prev
				(*first.node).prev = tmp;						         //first��prevָ��ԭposition��ǰ�ڵ�
			}
		}
		
	protected:
		link_type	node;   //�ؼ��ڵ�Ԫ��


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
