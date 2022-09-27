#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  今日开始deque容器实现  先做部分迭代器
//20220926  最近需要准备开题，finish 迭代器部分
//20220927  开始deque的数据结构
#include"iterator.h"
#include"allocator.h"
namespace yhstl
{
	//一个全局函数：_deque_buf_size 来决定deque的缓冲区大小
	//如果n不为0，则就是n，表示缓冲区大小为用户指定的n
	//如果n为0，则使用默认值，如果size(T)>512则为1，否则为512/size(T)
	inline  size_t _deque_buf_size(size_t n, size_t sz)
	{
		return n != 0 ? n : (sz < 512 ? sizeof(512 / sz) : sizeof(1));
	}

	//deque的迭代器
	template <class T,class Ref,class Ptr,size_t BufSiz>
	struct _deque_iterator    //没有继承iterator
	{
		typedef _deque_iterator<T, &T, T*, BufSiz> iterator;
		typedef _deque_iterator<T, const&T, const T*, BufSiz> const_iterator;
		static size_t buffer_size() { return _deque_buf_size(BufSiz, sizeof(T)); }  //内含一个全局函数

		//未继承 yhstl::iterator，所以必须自己定义五个必要相应迭代器型别
		typedef random_access_iterator_tag           iterator_category;
		typedef T                                    value_type;
		typedef Ptr                                  pointer;
		typedef Ref									 reference;
		typedef ptrdiff_t                            difference_type;
		typedef T**		map_pointer;
		typedef _deque_iterator self;

		//保持与容器的联结
		T* cur;
		T* first;
		T* last;
		map_pointer node;   //指向管控中心


		//先加一个跳缓冲区的函数   跳一个缓冲区大小
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//接下来是迭代器的各种重载运算符操作
		//* 和 -> 运算符是老程序了
		reference operator*() { return *cur; }   
		pointer operator->() const { return &(operator*()); }

		difference_type operator -(self& x)const    //这个重载-的逻辑挺有意思
		{
			return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
		}
		self& operator++()    //要特判一下 在缓冲区边界的情况
		{
			++cur;
			if (cur == last)
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		self& operator++(int)   //后置++    后置调前置 标准写法
		{
			self tmp = *this;
			++* this;
			return tmp;
		}
		self& operator--()    //要特判一下 在缓冲区边界的情况
		{		
			if (cur == first)
			{
				set_node(node -1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self& operator--(int)   //后置--    后置调前置 标准写法
		{
			self tmp = *this;
			--* this;
			return tmp;
		}

		//因为是随机迭代器，支持随机存取，所以要实现一下
		self& operator+=(difference_type n)
		{
			difference_type offset = n + (cur - first);   //先算出跳跃距离
			if (offset >= 0 && offset < difference_type(buffer_size()))  //判断是不是在同一个缓冲区内
			{
				cur += n;
			}
			else
			{
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : -difference_type((-offset-1) /(buffer_size())-1;
				//切换到正确的节点
				set_node(node + node_offset);
				//切换到正确的元素
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}
		self operator+(difference_type  n)const   //迭代器的+
		{
			self tmp = *this;
			return tmp += n;
		}
		self operator-=(difference_type n) { return *this += -n };
		self operator-(difference_type  n)const   //迭代器的-
		{
			self tmp = *this;
			return tmp -= n;
		}

		//以下实现随机存取  调用 operator+
		reference operator[](difference_type n) const { return *(*this + n); }
		
		//以下调用 operator* operator+
		bool operator == (const self& x) { return cur == x.cur; }
		bool operator != (const self& x) { return !(*this == x); }
		bool operator<(const self& x)const
		{
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};
	
	//deque的数据结构
	template <class T,size_t BufSiz =0>
	class deque
	{
	public:
		typedef T  value_type;
		typedef T* pointer;
		typedef size_t size_type;
		typedef ptrdiff_t    difference_type;
		typedef iterator::reference  reference;
	public:
		typedef _deque_iterator<T, T&, T*, BufSiz> iterator;  //使用上面构造的deque专属迭代器

	protected:
		typedef pointer* map_pointer;   //仍然定义一个指向指针的指针
	protected:       //数据成员
		iterator start;       //map的第一个节点
		iterator finish;	  //指向map最后一个节点（其实是尾后迭代器的概念）
		map_pointer map;	  //中控器map, 指向map，map是段连续空间，其每个元素都是指针，指向一个缓冲区
		size_type map_size;   //表示map的元素个数，就是缓冲区的个数

	
	public:
		iterator begin() { return start; }
		iterator end() { return finish; }
		reference operator[](size_type n) { return start[difference_type(n); }
		reference front() { return *start; }
		reference back()
		{
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		size_type size() { return finish - start; }
		size_type max_size()const { return size_type(-1); }
		bool empty() { return start == finish; }

		//deque构造和内存管理
	public:
		//一个构造函数，调用 fill_initialize
		deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0)
		{
			fill_initialize(n, value);
		}
	protected:
		//专属空间配置器，一次配置一个元素大小
		typedef allocator<T>  data_allocator;
		//专属空间配置器，每次配置一个指针大小
		typedef allocator<T>  map_allocator;
		void allocate_node() { return data_allocator::allocate(buffer_size()); }
		void deallocate_node() { return data_allocator::deallocate(buffer_size()); }

		void fill_initialize(size_type n, const value_type& value)
		{
			create_map_and_nodes(n);  //把deque的结构都产生并安排好
			map_pointer cur;
			try
			{
				for (cur = start.node; cur < finish.node; ++cur)
				{
					unitialized_fill(*cur, *cur + buffer_size(), value);
				}
				//最后一个节点的设置有所不同(因为尾端可能有备用空间，所以不用设置初值)
				unitialized_fill(finish.first, finish.cur, value);
			}
		}

		//下面这个函数负责产生并安排好deque的结构
		void create_map_and_nodes(size_type num_elements)
		{
			//需要节点数=（元素个数/每个缓冲区大小）+1
			//最后答案是上取整的，如果正好整除，则多分配一个
			size_type num_nodes = num_elements / buffer_size() + 1;

			//一个map管理几个节点，最少是8个，最多是所需节点+2
			//前后可以各预留一个，以备后用
			map_size = max(8, num_nodes + 2);
			map = map_allocator::allocator(map_size);
			//到此，配置出一个拥有map_size节点的map

			//以下，令nstart和nfinish指向map所拥有所有节点的最中央区段
			//保持在最中央，可以使头尾两端的扩充能量一样大，每个节点可对应一个缓冲区
			map_pointer nstart = map + (map_size - num_nodes) / 2;
			map_pointer nfinish = nstart + num_nodes - 1;

			map_pointer cur;
			try
			{
				//为map的每个现用节点分配缓冲区，加起来就是整个map的
				//可用缓冲区
				for (cur = nstart; cur < =nfinish; cur++)
				{
					*cur = allocate_node();
				}
			}
			catch (...)
			{
				//commit or rollback  实现处理函数
				...
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.cur = start.first;
			finsh.cur = finish.first + num_elements % buffer_size();
			//之前计算num_nodes时，如果整除，会多配置一个节点
			//此时即令cur指向这多配的一个节点的起始处，就是让finish.cur指向最后一个分配的节点的起始处
		}

		

		//push_back()
		void push_back(const value_type& t) {
			if (finish.cur != finish.last - 1) {
				//最后尚有一个以上的备用空间
				//直接在备用空间上进行元素的构造
				data_allocator::allocator<T>::construct(finish.cur, t);
				//调整最后缓冲区的使用状态
				++finish.cur;
			}
			else {
				//最后缓冲区已经 无(或者只剩下一个)元素的备用空间
				push_back_aux(t);
			}
		}

		//当尾端只剩下一个元素作为备用的空间,于是会调用push_back_aux()
	    //先分配一整块新的缓冲区，再设置新的元素的内容，然后更改迭代器finish的状态
	    //只有当 finish.cur == finish.last - 1 的时候才会调用
		void push_back_aux(const value_type& t) {
			value_type t_copy = t;
			reverse_map_at_back(); //符合某种条件则必须重换一个map
			*(finish.node + 1) = allocate_node(); //配置一个新的节点 (缓冲区)
			try {
				data_allocator::allocator<T>::construct(finish.cur, t_copy);//针对标的元素设定初值
				finish.set_node(finish.node + 1);//改变finish令其指向新的节点
				finish.cur = finish.first;      //设定finish的状态
			}
			catch (std::exception) {
				data_allocator::allocator<T>::deallocate(*(finish.node + 1));
			}
		}

		void push_front(const value_type& t) {
			if (start.cur != start.first) { //第一缓冲区尚有备用的空间
				data_allocator::allocator<T>::construct(start.cur - 1, t); //直接在备用空间上构造元素
				--start.cur; //调整第一缓冲区的使用状态
			}
			else { //第一缓冲区已经无备用空间
				push_back_aux(t);
			}
		}

		//当start.cur == start.first时才会被调用
		//当第一个缓冲区没有任何备用元素的时候才会被调用
		void push_front_aux(const value_type& t) {
			value_type t_copy = t;
			reverse_map_at_front(); //如果满足条件则必须要重新换一个map
			*(start.node - 1) = allocate_node(); //配置一个新的节点(缓冲区)
			try {
				start.set_node(start.node - 1); //改变start，另其指向新的节点
				start.cur = start.cur - 1; //设定start的状态
				data_allocator::allocator<T>::construct(start.cur, t_copy);
			}
			catch (std::exception) {
				//commit or rollback
				start.set_node(start.node + 1);
				start.cur = start.first;
				data_allocator::allocator<T>::deallocate(*(start.node - 1));
				throw;
			}
		}


		//整治的实际操作是通过调用 reallocate_map()函数 来实现的
		void reallocate_map(size_type nodes_to_add, bool add_at_front) {
			size_type old_num_nodes = finish.node - start.node + 1;
			size_type new_num_nodes = old_num_nodes + nodes_to_add;

			map_pointer new_nstart;
			if (map_size > 2 * new_num_nodes) {
				new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
				if (new_nstart < start.node) {
					std::copy(start.node, finish.node + 1, new_nstart);
				}
				else {
					std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
				}
			}
			else {
				size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
				//配置一块新的内存 给map使用
				map_pointer new_map = map_allocator::allocate(new_map_size);
				new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
				//将原map内容拷贝过来
				std::copy(start.node, finish.node + 1, new_nstart);
				//释放原map
				map_allocator::deallocate(map, map_size);
				map = new_map;
				map_size = new_map_size;
			}
			//重新设定 迭代器 start 和 finish
			start.set_node(new_nstart);
			finish.set_node(new_nstart + old_num_nodes - 1);
		}

		//reverse_map_at_front() 和 reverse_map_at_back()负责整治 map
		void reverse_map_at_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size - (finish.node - map)) {
				//如果map节点备用空间不足
				//符合上述条件则必须重新替换一个map (配置更大的内存，拷贝先前的旧的区间，释放先前的旧的空间)
				reallocate_map(nodes_to_add, false);
			}
		}
		void reverse_map_at_front(size_type nodes_to_add = 1) {
			if (nodes_to_add > start.node - map) {
				//如果map的前端节点备用空间不足
				//符合上述条件则必须重新替换一个map (配置更大的内存，拷贝先前的旧的区间，释放先前的旧的空间)
				reallocate_map(nodes_to_add, true);
			}
		}

		void pop_back() {
			if (finish.cur != finish.first) {
				//最后一个缓冲区存在一个 或者 更多的元素
				--finish.cur; //调整指针，相当于排除了最后的元素
				data_allocator::allocator<T>::destroy(finish.cur); //析构最后的元素
			}
			else {
				pop_back_aux();
			}
		}

		//finish.cur == finish.first
		void pop_back_aux() {
			deallocate_node(finish.first); //释放最后一个缓冲区
			finish.set_node(finish.node - 1);//调整finish的状态
			finish.cur = finish.last - 1;//将finish的cur指针指向最后一个元素
			data_allocator::allocator<T>::destroy(finish.cur);//进行元素的析构
		}

		void pop_front() {
			if (start != start.last - 1) {
				//第一缓冲区有一个或者多个元素
				data_allocator::allocator<T>::destroy(start.cur);//析构当前的元素
				++start.cur; //调整指针，相当于排除了第一个元素
			}
			else {
				pop_back_aux(); //这里将进行缓冲区的释放工作
			}
		}

		//当start.cur == start.last-1被调用
		void pop_front_aux() {
			data_allocator::allocator<T>::destroy(start.cur); //将第一缓冲区仅剩的一个元素析构
			deallocate_node(start.first); //释放第一个缓冲区
			start.set_node(start.node + 1);//调整start的状态
			start.cur = start.first; //将start指向缓冲区的第一个元素
		}

		//清除整个deque
		//需要注意的是 deque的最初状态(无任何元素的时候) 保证有一个缓冲区
		void clear() {
			//针对头尾指针以外的每一个缓冲区，都应该是饱满的
			for (map_pointer node = start.node + 1; node < finish.node - 1; ++node) {
				//将缓冲区内的所有元素全部析构
				//调用destroy()的第二版本
				data_allocator::allocator<T>::deallocate(*node, buffer_size());
			}
			if (start.node != finish.node) {//至少有头尾两个缓冲区
				data_allocator::allocator<T>::destroy(start.cur, start.last); //将头缓冲区目前所有的元素析构
				data_allocator::allocator<T>::destroy(finish.first, finish.cur); //将尾缓冲区目前所有的元素析构
				//释放尾缓冲区的的物理内存，但是保留头部缓冲区
				data_allocator::deallocate(finish.first, buffer_size());
			}
			else {
				//只有一个缓冲区
				//将唯一缓冲区内的所有元素析构
				//注意：并不释放缓冲区的空间，将这唯一的缓冲区保留
				data_allocator::allocator<T>::destroy(start.cur, finish.cur);
			}
			//调整状态
			finish = start;
		}

		//使用erase消除特定位置上的元素
		//消除pos指向的元素，pos为清除点
		iterator erase(iterator pos) {
			iterator next = pos;
			++next;
			difference_type index = pos - start; //清除点之前的元素的个数
			if (index < (size() >> 1)) { //如果清除点之前的元素比较少
				std::copy_backward(start, pos, next);//移动清除点之前的元素
				pop_front(); //移动完毕，将最前一个元素删除
			}
			else {
				//清除点之后的元素较少
				std::copy(next, finish, pos);//移动清除点之后的元素
				pop_back();//移动完毕 将最后一个冗余的元素删除
			}
			return start + index;
		}

		//使用erase删除[first,last)区间内的所有元素
		// 也就是删除指定范围内的一段元素
		iterator erase(iterator first, iterator last) {
			if (first == start && last == finish) {
				//如果删除的是整段内存空间，直接调用clear()
				clear();
				return finish;
			}
			else {
				difference_type n = last - first;        //清除区间的长度
				difference_type elements_before = first - start; //清除区间前方的元素个数
				if (elements_before < (size() - n) / 2) {  //如果前方的元素比较少
					//copy_backward和copy函数类似，只是逆向对元素进行复制
					std::copy_backward(start, first, last); //向后移动前方的元素(覆盖清除区间)
					iterator new_start = start + n; //标记deque的新的起点
					data_allocator::allocator<T>::destroy(start, new_start); //移动完毕之后，将冗余的元素析构
					//以下将冗余的缓冲区释放
					for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
						data_allocator::deallocate(*cur, buffer_size());
					}
					start = new_start;//设定deque的新起点
				}
				else {
					//如果删除区间的后方的元素比较少
					std::copy(last, finish, first); //向前移动后方的元素 (覆盖清除区间)
					iterator new_finish = finish - n;  //标记finish的新的尾巴
					data_allocator::allocator<T>::destroy(new_finish, finish);  //移动完毕，将冗余的元素进行析构
					//以下将释放冗余的缓冲区
					for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
						data_allocator::deallocate(cur, buffer_size());
					}
					//设定新的尾节点
					finish = new_finish;
				}
				return start + elements_before;
			}
		}

		//在某个点之前插入一个元素，并设定其值
		iterator insert(iterator position, const value_type& x) {
			if (position.cur == start.cur) { //如果插入点是deque的最前端
				push_front(x);
				return start;
			}
			else if (position.cur == finish.cur) { //如果插入点是deque的最后端
				pop_back(x);
				iterator tmp = finish;
				--tmp;
				return tmp;
			}
			else {
				return insert_aux(position, x);
			}
		}

		iterator insert_aux(iterator pos, const value_type& x) {
			difference_type index = pos - start;//插入点之前的元素的个数
			value_type x_copy = x;
			if (index < (size() / 2)) {
				//如果插入节点之前的元素的个数比较少
				push_front(front());//在最前端加上和第一个元素数值相同的元素
				iterator front1 = start;//以下标记记号，然后进行元素的移动
				++front1; iterator front2 = front1;
				++front2;
				pos = start + index;
				iterator pos1 = pos;
				++pos1;
				std::copy(front2, pos1, front1);//元素的移动
			}
			else {
				push_back(back());
				iterator back1 = finish;
				--back1;
				iterator back2 = back1;
				--back2;
				pos = start + index;
				std::copy_backward(pos, back2, back1);
			}
			//在插入点上设定新值
			*pos = x_copy;
			return pos;
		}

	};
}


#endif