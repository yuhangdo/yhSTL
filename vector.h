#ifndef _YHSTL_VECTOR_H_
#define _YHSTL_VECTOR_H_

//���ͷ�ļ�����һ�������� vector
//vector����������˼
//Vector���������������ݰ��źͲ�����ʽ
//�����Ҫ���ڿռ����õ�����ԡ�
//Array�Ǿ�̬�ռ䣬һ�����þͲ��ܸı�;Vector�Ƕ�̬�ռ䣬����������ռ���������Ԫ�ء�

//20220918  Ū��vector����Ū�����ģ�����ȫ�ֵ�constructor��destroy�ȣ�����copy��
//20220918  23:18 ��ʼ�������Ŀ  ʵ����insert����
#include "iterator.h"
#include"allocator.h"
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
		typedef yhstl::allocator<T>  data_allocator;  //����������simple_alloc��
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
		void insert(iterator position, size_type n, const T& x);
		~vector()
		{
			destory(start, finish);  //������Ҫ�Լ�ʵ�֣���destory����һ��ͷ�ļ���
			deallocator();			 //����vector��һ����Ա����
		}
		reference front() { return *begin(); }  //���ص�һ��Ԫ��
		reference back() { return *(end() - 1); } //�������һ��Ԫ�أ�˼·��β�������-1,
		void insert(iterator position, size_type n, const T& x);
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
		void resize(size_type new_size, const T& x) //�����������һ�ֱȵ�ǰsize��һ�ֱȵ�ǰsizeС
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
	//20220920 �޸�insert�࣬ʹ���Ϊvector�ĳ�Ա������ֻ�������ⲿ����
	template <class T> 
	void vector<T>::insert(iterator position, size_type n, const T& x)
	{
		if (n != 0)   //��n������0ʱ���Ž������²���
		{
			if (size_type(end_of_storage - finish) >= 0)   //ʣ��ռ��㹻
			{
				T x_copy = x;
				//���ż��� position֮�������Ԫ�ظ���
				const size_type elems_after = finish - position;  //ע����const
				iterator old_finish = finish;
				if (elems_after > n)   //���position֮������и�������n
				{
					//��������Ҫ���Ȱ�[finish-elems_after��finish]��Ԫ����finish+n����
					// ����ǰ�ԭvector��position+n��finish-n��Ԫ���ã�copy_backward��position
					//���������copy������ʵ�ֽ���[finish,finish+n]�������[finish-n,finish]��Ԫ��
					uninitialized_copy(finish - n, finish, finish);  //ע���������Ҫ֮���Լ�ʵ��
					finish += n;  //��finish�������������
					copy_backward(position, old_finish - n, old_finish);  //�������ͬ����Ҫע��
					fill(position, position + n, x_copy); //��λ���Ϲ���x
				}
				else    //n>elems_after
				{
					//�����֮��Ԫ��С����������
					uninitialized_fill_n(finish, n - elems_after, x_copy);//����finish֮���� n-elem��x
					finish += n - elems_after;		//����finish
					uninitialized_copy(position, old_finish, finish);    //��ԭvector��position֮���Ԫ��copy�����º��finish֮����
					finish += elems_after;		//����finish
					fill(position, old_finish, x_copy);    //��position��old_finish�й���x
				}
			}
			else   //���ÿռ�<����Ԫ�ظ��� ,Ҫ��ƶ�̬����ռ�
			{
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n); //���Ⱦ�������2*old����old+n��ȡ��ֵ
				//���������µĿռ�
				iterator new_start = data_allocator(len);  //�ȷ���len���ռ䣬Ȼ����һ��newstart����ֵָ�򿪱ٿռ���׵�ַ
				iterator new_finish = new_start; //vector��̬��չ����Ҫ���������µ�������������ʼָ���·���ռ�
				try    //����Ҫ�����쳣������
				{
					//���Ƚ���vector position֮ǰ��Ԫ�ظ��Ƶ���vector��
					new_finish = uninitialized_copy(start, position, new_start);
					//������Ԫ�ؼ�����vector��
					new_finish = uninitialized_fill_n(new_finish, n, x);  //�������֮��ҲҪ�Լ�ʵ��
					//�ٽ���vector position֮���Ԫ�ظ��Ƶ���vector��
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
#ifdef STL_USE_EXCEPTION     //ѧϰ�����쳣�������->��һ���������������棬���ܻ��ж���ļ�ͬʱ����һ��ͷ�ļ�������Щ�ļ��������ӳ�һ����ִ���ļ���ʱ���ͻ���ִ������ض��塱������ͷ�ļ���ʹ��#define��ifndef��ifdef��endif�ܱ���ͷ�ļ��ض��塣
				catch (...)
				{
					//�����쳣��������ʵ��commit or rollback
					destroy(new_start, new_finish); //������
					data_allocator::deallocator(new_start, len);
					throw;
				}
#endif
				//���������vector
				destroy(start, finish);
				deallocate();
				//����vector������������
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;

			}
		}
	}


}  //namespace yhstl
#endif // !_YHSTL_VECTOR_H_
