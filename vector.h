#ifndef _YHSTL_VECTOR_H_
#define _YHSTL_VECTOR_H_

//���ͷ�ļ�����һ�������� vector
//vector����������˼
//Vector���������������ݰ��źͲ�����ʽ
//�����Ҫ���ڿռ����õ�����ԡ�
//Array�Ǿ�̬�ռ䣬һ�����þͲ��ܸı�;Vector�Ƕ�̬�ռ䣬����������ռ���������Ԫ�ء�
#include <iterator>
namespace yhstl
{
	template <class T>
	class vector
	{
	public:
		// vector ��Ƕ���ͱ���
		typedef T  value_type;
		typedef value_type* pointer;
		typedef value_type* iterator;
		typedef value_type& reference;
 		typedef size_t size_type;
		typedef ptrdiff_t difference_type;   //���������һ��iterator��

	protected:
		typedef simple_alloc<value_type, Alloc>  data_allocator;  //����������simple_alloc��
		iterator start;     //��ʾ��ǰʹ�ÿռ��ͷ
		iterator finish;	//��ʾ��ǰʹ�ÿռ��β��ע���ʾ����β��
		iterator end_of_storage;   //��ʾ��ǰ���ÿռ��β
		void insert_aux(iterator position, const T& x);  //������
		void deallocator()
		{
			if (start)
			{
				data_allocator::deallocate(start, end_of_storage - start);   //ɾ����Ҫȥ���ײ�ʵ��
			}
		}
		void fill_initialize(size_type n, const T& value)  //������ʼ��Ϊn��value
		{
			start = allocator_and_fill(n, value);   //��ʼ��
			finish = start + n;						//����finish������
			end_of_storage = finish;				//����end_of_storage������
		}
	public:
		iterator begin() { return start; }    //vector��������begin����
		iterator end() { return finish; }	  //vector��������end����
		size_type size() const { return size_type(end() - begin()); } //ע������һ��const��Ա����
		size_type capacity()const			  //vector��capacity����
		{
			return size_type(end_of_storage - begin());
		}
		bool empty() const { return begin() == end(); } //�ж�vector�Ƿ�Ϊ��
		reference operator[](size_type n) { return *(begin + n); } //����begin+nʵ���±������
		vector() :start(0), finish(0), end_of_storage(0) {};   //Ĭ�Ϲ��캯��
		vector(size_type n, const T& value) { fill_initialize(n, value); };  //��ʼ��
		vector(int n, const T& value) { fill_initialize(n, value); };
		vector(long n, const T& value) { fill_initialize(n, value); };
		explicit vector(size_type n) { fill_initialize(n, T()); };  //ʹ��explicit�����ܽ�����ʽ���캯����ʹ��T()����һ����ʱ����

		~vector()
		{
			destory(start, finish);  //������Ҫ�Լ�ʵ�֣���destory����һ��ͷ�ļ���
			deallocator();			 //����vector��һ����Ա����
		}
		reference front() { return *begin(); }  //���ص�һ��Ԫ��
		reference back() { return *(end() - 1); } //�������һ��Ԫ�أ�˼·��β�������-1,
		void push_back(const T& x)   //��Ԫ�ز���β��
		{

		}
	};



}
#endif // !_YHSTL_VECTOR_H_
