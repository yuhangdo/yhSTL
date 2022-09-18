#ifndef _YHSTL_VECTOR_H_
#define _YHSTL_VECTOR_H_

//这个头文件包含一个容器类 vector
//vector是向量的意思
//Vector和数组差别在于数据安排和操作方式
//差别主要在于空间运用的灵活性。
//Array是静态空间，一旦配置就不能改变;Vector是动态空间，会自行扩充空间以容纳新元素。

//20220918  弄完vector，先弄基础的，比如全局的constructor和destroy等，还有copy等
//注意一定要加simple_alloc这个类
//20220918  23:18 开始今晚的项目
#include <iterator>
namespace yhstl
{
	template <class T>
	class vector
	{
	public:
		// vector 的嵌套型别定义
		typedef T  value_type;
		typedef value_type* pointer;
		typedef value_type* iterator;
		typedef value_type& reference;
 		typedef size_t size_type;
		typedef ptrdiff_t difference_type;   //后续得添加一个iterator类

	protected:
		typedef simple_alloc<value_type, Alloc>  data_allocator;  //后续将增加simple_alloc类
		iterator start;     //表示当前使用空间的头
		iterator finish;	//表示当前使用空间的尾，注意表示的是尾后
		iterator end_of_storage;   //表示当前可用空间的尾
		//void insert_aux(iterator position, const T& x);  //先声明，动态扩展空间实现push_back
		void deallocator()
		{
			if (start)
			{
				data_allocator::deallocate(start, end_of_storage - start);   //删除，要去看底层实现
			}
		}
		void fill_initialize(size_type n, const T& value)  //容器初始化为n个value
		{
			start = allocator_and_fill(n, value);   //初始化
			finish = start + n;						//更新finish迭代器
			end_of_storage = finish;				//更新end_of_storage迭代器
		}
	public:
		iterator begin() { return start; }    //vector迭代器的begin函数
		iterator end() { return finish; }	  //vector迭代器的end函数
		size_type size() const { return size_type(end() - begin()); } //注意这是一个const成员函数
		size_type capacity()const			  //vector的capacity函数
		{
			return size_type(end_of_storage - begin());
		}
		bool empty() const { return begin() == end(); } //判断vector是否为空
		reference operator[](size_type n) { return *(begin + n); } //借用begin+n实现下标操作符
		vector() :start(0), finish(0), end_of_storage(0) {};   //默认构造函数
		vector(size_type n, const T& value) { fill_initialize(n, value); };  //初始化
		vector(int n, const T& value) { fill_initialize(n, value); };
		vector(long n, const T& value) { fill_initialize(n, value); };
		explicit vector(size_type n) { fill_initialize(n, T()); };  //使用explicit，不能进行隐式构造函数，使用T()创建一个临时对象

		~vector()
		{
			destory(start, finish);  //后面需要自己实现，把destory放在一个头文件中
			deallocator();			 //这是vector的一个成员函数
		}
		reference front() { return *begin(); }  //返回第一个元素
		reference back() { return *(end() - 1); } //返回最后一个元素，思路是尾后迭代器-1,
		void push_back(const T& x)   //将元素插入尾端
		{
			if (finish != end_of_storage)  //先判断当前分配空间是否有剩余
			{
				constuctor(finish, x); //在finish的位置上构建x，注意constructor我们将在后续定义为全局函数
				finish++;  //构建完后更新finish迭代器
			}
			else     //finish==end_of_storage,需要动态扩展空间
			{
				insert_aux(end(), x);  //这是vector的一个成员函数
			}
		}
		void pop_back()     //将尾端元素取出
		{
			--finish;    //始终注意finish是尾后迭代器
			destroy(finsh);   //一样是全局函数，最后跟constructor一样去实现
		}
		iterator erase(iterator position)  //删除vector某个位置上的元素
		{
			if (position + 1 != end()) copy(position + 1, finish, position);  //如果删除元素不是最后一个元素就需要使用copy函数
			--finish;  //因为删除一个元素，所以finish迭代器要更新
			destroy(finsh);  //跟pop_back操作流程差不多
			return position;   //erase返回的是删除之后这个位置迭代器
		}
		void resize(size_type new_size,const T&x) //分两种情况，一种比当前size大，一种比当前size小
		{
			if (new_size < size()) erase(begin() + new_size, end());
			else insert(end(), new_size - size(), x);  //insert是algorithm头文件实现的泛型算法，用于容器
		}
		void resize(size_type new_size)   //重载resize
		{
			resize(new_size, T());  //调用resize（new_size，x)来实现
		}
		void clear()    //清空容器
		{
			erase(begin(), end());   //其实就是调用eraseAPI 
		}
	protected:
		//配置空间并填满内容
		iterator allocator_and_fill(size_type n, const T& x)
		{
			iterator result = data_allocator::alocator(n);  //先调用alocator，分配n个空间
			uninitialized_fill_n(result, n, x);  //全局函数
			return result;
		}
		
		void insert_aux(iterator position, const T& x)  //这个函数是vector最重点的函数,用于动态扩展空间
		{
			if (finish != end)   //还有后备空间，那直接构造呗
			{
				construct(finish, *(finish - 1));
				++finish;
				T x_copy = x;
				copy_backward(position, finish - 2, finish - 1);  //这个函数必须要记得，等做完vector就做这些基本的
				*position = x_copy;
			}
			else  //无备用空间，则需要动态扩展
			{
				const size_type old_size = size();
				const size_type len = old_size != 0 ? 2 * old_size : 1; //关键代码
				iterator new_start = data_allocator::allocator(len);
				iterator new_finish = new_start;   //先暂时将new_finish指向new_start
				try   //加异常处理机制
				{
					//将原空间position位置之前的内容拷贝到新vector
					new_finish = uninitialized_copy(start, position, new_start); //这个uninitialized_copy后续实现
					//为position位置的元素设定为x，因为此时new_finish是指向这个position位置的
					construct(new_finish, x);
					//调整new_finish
					++new_finish;
					//将原空间position后面位置的元素copy到新vector里
					new_finish = uninitialized_copy(position, finish, new_finish); //这里hj理解错了
				}
				catch (...)   //用了catch（...）的技巧，但是一般不建议使用
				{
					//commit or rollback！注意这个专业术语
					destroy(new_start, new_finish); //回滚！
					data_allocator::deallocator(new_start, len);
					throw;    //抛出异常，让外层函数捕获
				}
				//释放并析构原vector
				destroy(begin(), end());
				deallocate();

				//调整迭代器，指向新vector
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}

	};



}
#endif // !_YHSTL_VECTOR_H_
