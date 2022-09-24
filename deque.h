#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  ���տ�ʼdeque����ʵ��  �������ֵ�����

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
		map_pointer map;   //ָ��ܿ�����
	};
	
}


#endif