#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  ���տ�ʼdeque����ʵ��  �������ֵ�����
//20220926  �����Ҫ׼�����⣬finish ����������
#include"iterator.h"

namespace yhstl
{
	//һ��ȫ�ֺ�����_deque_buf_size ������deque�Ļ�������С
	//���n��Ϊ0�������n����ʾ��������СΪ�û�ָ����n
	//���nΪ0����ʹ��Ĭ��ֵ�����size(T)>512��Ϊ1������Ϊ512/size(T)
	inline  size_t _deque_buf_size(size_t n, size_t sz)
	{
		return n != 0 ? n : (sz < 512 ? sizeof(512 / sz) : sizeof(1));
	}

	//deque�ĵ�����
	template <class T,class Ref,class Ptr,size_t BufSiz>
	struct _deque_iterator    //û�м̳�iterator
	{
		typedef _deque_iterator<T, &T, T*, BufSiz> iterator;
		typedef _deque_iterator<T, const&T, const T*, BufSiz> const_iterator;
		static size_t buffer_size() { return _deque_buf_size(BufSiz, sizeof(T)); }  //�ں�һ��ȫ�ֺ���

		//δ�̳� yhstl::iterator�����Ա����Լ����������Ҫ��Ӧ�������ͱ�
		typedef random_access_iterator_tag           iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
		typedef T**		map_pointer;
		typedef _deque_iterator self;

		//����������������
		T* cur;
		T* first;
		T* last;
		map_pointer node;   //ָ��ܿ�����


		//�ȼ�һ�����������ĺ���   ��һ����������С
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//�������ǵ������ĸ����������������
		//* �� -> ��������ϳ�����
		reference operator*() { return *cur; }   
		pointer operator->() const { return &(operator*()); }

		difference_type operator -(self& x)const    //�������-���߼�ͦ����˼
		{
			return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
		}
		self& operator++()    //Ҫ����һ�� �ڻ������߽�����
		{
			++cur;
			if (cur == last)
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		self& operator++(int)   //����++    ���õ�ǰ�� ��׼д��
		{
			self tmp = *this;
			++* this;
			return tmp;
		}
		self& operator--()    //Ҫ����һ�� �ڻ������߽�����
		{		
			if (cur == first)
			{
				set_node(node -1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self& operator--(int)   //����--    ���õ�ǰ�� ��׼д��
		{
			self tmp = *this;
			--* this;
			return tmp;
		}

		//��Ϊ�������������֧�������ȡ������Ҫʵ��һ��
		self& operator+=(difference_type n)
		{
			difference_type offset = n + (cur - first);   //�������Ծ����
			if (offset >= 0 && offset < difference_type(buffer_size()))  //�ж��ǲ�����ͬһ����������
			{
				cur += n;
			}
			else
			{
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : -difference_type((-offset-1) /(buffer_size())-1;
				//�л�����ȷ�Ľڵ�
				set_node(node + node_offset);
				//�л�����ȷ��Ԫ��
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}
		self operator+(difference_type  n)const   //��������+
		{
			self tmp = *this;
			return tmp += n;
		}
		self operator-=(difference_type n) { return *this += -n };
		self operator-(difference_type  n)const   //��������-
		{
			self tmp = *this;
			return tmp -= n;
		}

		//����ʵ�������ȡ  ���� operator+
		reference operator[](difference_type n) const { return *(*this + n); }
		
		//���µ��� operator* operator+
		bool operator == (const self& x) { return cur == x.cur; }
		bool operator != (const self& x) { return !(*this == x); }
		bool operator<(const self& x)const
		{
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};
	
}


#endif