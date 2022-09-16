#ifndef _YHSTL_VECTOR_H_
#define _YHSTL_VECTOR_H_

//这个头文件包含一个容器类 vector
//vector是向量的意思
//Vector和数组差别在于数据安排和操作方式
//差别主要在于空间运用的灵活性。
//Array是静态空间，一旦配置就不能改变;Vector是动态空间，会自行扩充空间以容纳新元素。
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
		void insert_aux(iterator position, const T& x);  //先声明
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

		}
	};



}
#endif // !_YHSTL_VECTOR_H_
