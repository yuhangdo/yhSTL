#ifndef _YHSTL_VECTOR_H_
#define _YHSTL_VECTOR_H_

//���ͷ�ļ�����һ�������� vector
//vector����������˼
//Vector���������������ݰ��źͲ�����ʽ
//�����Ҫ���ڿռ����õ�����ԡ�
//Array�Ǿ�̬�ռ䣬һ�����þͲ��ܸı�;Vector�Ƕ�̬�ռ䣬����������ռ���������Ԫ�ء�

//20220918  Ū��vector����Ū�����ģ�����ȫ�ֵ�constructor��destroy�ȣ�����copy��
//ע��һ��Ҫ��simple_alloc�����
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
		//void insert_aux(iterator position, const T& x);  //����������̬��չ�ռ�ʵ��push_back
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
			if (finish != end_of_storage)  //���жϵ�ǰ����ռ��Ƿ���ʣ��
			{
				constuctor(finish, x); //��finish��λ���Ϲ���x��ע��constructor���ǽ��ں�������Ϊȫ�ֺ���
				finish++;  //����������finish������
			}
			else     //finish==end_of_storage,��Ҫ��̬��չ�ռ�
			{
				insert_aux(end(), x);  //����vector��һ����Ա����
			}
		}
		void pop_back()     //��β��Ԫ��ȡ��
		{
			--finish;    //ʼ��ע��finish��β�������
			destroy(finsh);   //һ����ȫ�ֺ���������constructorһ��ȥʵ��
		}
		iterator erase(iterator position)  //ɾ��vectorĳ��λ���ϵ�Ԫ��
		{
			if (position + 1 != end()) copy(position + 1, finish, position);  //���ɾ��Ԫ�ز������һ��Ԫ�ؾ���Ҫʹ��copy����
			--finish;  //��Ϊɾ��һ��Ԫ�أ�����finish������Ҫ����
			destroy(finsh);  //��pop_back�������̲��
			return position;   //erase���ص���ɾ��֮�����λ�õ�����
		}
		void resize(size_type new_size,const T&x) //�����������һ�ֱȵ�ǰsize��һ�ֱȵ�ǰsizeС
		{
			if (new_size < size()) erase(begin() + new_size, end());
			else insert(end(), new_size - size(), x);  //insert��algorithmͷ�ļ�ʵ�ֵķ����㷨����������
		}
		void resize(size_type new_size)   //����resize
		{
			resize(new_size, T());  //����resize��new_size��x)��ʵ��
		}
		void clear()    //�������
		{
			erase(begin(), end());   //��ʵ���ǵ���eraseAPI 
		}
	protected:
		//���ÿռ䲢��������
		iterator allocator_and_fill(size_type n, const T& x)
		{
			iterator result = data_allocator::alocator(n);  //�ȵ���alocator������n���ռ�
			uninitialized_fill_n(result, n, x);  //ȫ�ֺ���
			return result;
		}
		
		void insert_aux(iterator position, const T& x)  //���������vector���ص�ĺ���,���ڶ�̬��չ�ռ�
		{
			if (finish != end)   //���к󱸿ռ䣬��ֱ�ӹ�����
			{
				construct(finish, *(finish - 1));
				++finish;
				T x_copy = x;
				copy_backward(position, finish - 2, finish - 1);  //�����������Ҫ�ǵã�������vector������Щ������
				*position = x_copy;
			}
			else  //�ޱ��ÿռ䣬����Ҫ��̬��չ
			{
				const size_type old_size = size();
				const size_type len = old_size != 0 ? 2 * old_size : 1; //�ؼ�����
				iterator new_start = data_allocator::allocator(len);
				iterator new_finish = new_start;   //����ʱ��new_finishָ��new_start
				try   //���쳣�������
				{
					//��ԭ�ռ�positionλ��֮ǰ�����ݿ�������vector
					new_finish = uninitialized_copy(start, position, new_start); //���uninitialized_copy����ʵ��
					//Ϊpositionλ�õ�Ԫ���趨Ϊx����Ϊ��ʱnew_finish��ָ�����positionλ�õ�
					construct(new_finish, x);
					//����new_finish
					++new_finish;
					//��ԭ�ռ�position����λ�õ�Ԫ��copy����vector��
					new_finish = uninitialized_copy(position, finish, new_finish); //����hj������
				}
				catch (...)   //����catch��...���ļ��ɣ�����һ�㲻����ʹ��
				{
					//commit or rollback��ע�����רҵ����
					destroy(new_start, new_finish); //�ع���
					data_allocator::deallocator(new_start, len);
					throw;    //�׳��쳣������㺯������
				}
				//�ͷŲ�����ԭvector
				destroy(begin(), end());
				deallocate();

				//������������ָ����vector
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}

	};



}
#endif // !_YHSTL_VECTOR_H_
