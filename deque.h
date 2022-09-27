#ifndef _YHSTL_DEQUE_H_
#define _YHSTL_DEQUE_H_
//20220924  ���տ�ʼdeque����ʵ��  �������ֵ�����
//20220926  �����Ҫ׼�����⣬finish ����������
//20220927  ��ʼdeque�����ݽṹ
#include"iterator.h"
#include"allocator.h"
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
		typedef Ptr                                  pointer;
		typedef Ref									 reference;
		typedef ptrdiff_t                            difference_type;
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
	
	//deque�����ݽṹ
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
		typedef _deque_iterator<T, T&, T*, BufSiz> iterator;  //ʹ�����湹���dequeר��������

	protected:
		typedef pointer* map_pointer;   //��Ȼ����һ��ָ��ָ���ָ��
	protected:       //���ݳ�Ա
		iterator start;       //map�ĵ�һ���ڵ�
		iterator finish;	  //ָ��map���һ���ڵ㣨��ʵ��β��������ĸ��
		map_pointer map;	  //�п���map, ָ��map��map�Ƕ������ռ䣬��ÿ��Ԫ�ض���ָ�룬ָ��һ��������
		size_type map_size;   //��ʾmap��Ԫ�ظ��������ǻ������ĸ���

	
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

		//deque������ڴ����
	public:
		//һ�����캯�������� fill_initialize
		deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0)
		{
			fill_initialize(n, value);
		}
	protected:
		//ר���ռ���������һ������һ��Ԫ�ش�С
		typedef allocator<T>  data_allocator;
		//ר���ռ���������ÿ������һ��ָ���С
		typedef allocator<T>  map_allocator;
		void allocate_node() { return data_allocator::allocate(buffer_size()); }
		void deallocate_node() { return data_allocator::deallocate(buffer_size()); }

		void fill_initialize(size_type n, const value_type& value)
		{
			create_map_and_nodes(n);  //��deque�Ľṹ�����������ź�
			map_pointer cur;
			try
			{
				for (cur = start.node; cur < finish.node; ++cur)
				{
					unitialized_fill(*cur, *cur + buffer_size(), value);
				}
				//���һ���ڵ������������ͬ(��Ϊβ�˿����б��ÿռ䣬���Բ������ó�ֵ)
				unitialized_fill(finish.first, finish.cur, value);
			}
		}

		//�����������������������ź�deque�Ľṹ
		void create_map_and_nodes(size_type num_elements)
		{
			//��Ҫ�ڵ���=��Ԫ�ظ���/ÿ����������С��+1
			//��������ȡ���ģ��������������������һ��
			size_type num_nodes = num_elements / buffer_size() + 1;

			//һ��map�������ڵ㣬������8�������������ڵ�+2
			//ǰ����Ը�Ԥ��һ�����Ա�����
			map_size = max(8, num_nodes + 2);
			map = map_allocator::allocator(map_size);
			//���ˣ����ó�һ��ӵ��map_size�ڵ��map

			//���£���nstart��nfinishָ��map��ӵ�����нڵ������������
			//�����������룬����ʹͷβ���˵���������һ����ÿ���ڵ�ɶ�Ӧһ��������
			map_pointer nstart = map + (map_size - num_nodes) / 2;
			map_pointer nfinish = nstart + num_nodes - 1;

			map_pointer cur;
			try
			{
				//Ϊmap��ÿ�����ýڵ���仺��������������������map��
				//���û�����
				for (cur = nstart; cur < =nfinish; cur++)
				{
					*cur = allocate_node();
				}
			}
			catch (...)
			{
				//commit or rollback  ʵ�ִ�����
				...
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.cur = start.first;
			finsh.cur = finish.first + num_elements % buffer_size();
			//֮ǰ����num_nodesʱ������������������һ���ڵ�
			//��ʱ����curָ��������һ���ڵ����ʼ����������finish.curָ�����һ������Ľڵ����ʼ��
		}

		

		//push_back()
		void push_back(const value_type& t) {
			if (finish.cur != finish.last - 1) {
				//�������һ�����ϵı��ÿռ�
				//ֱ���ڱ��ÿռ��Ͻ���Ԫ�صĹ���
				data_allocator::allocator<T>::construct(finish.cur, t);
				//������󻺳�����ʹ��״̬
				++finish.cur;
			}
			else {
				//��󻺳����Ѿ� ��(����ֻʣ��һ��)Ԫ�صı��ÿռ�
				push_back_aux(t);
			}
		}

		//��β��ֻʣ��һ��Ԫ����Ϊ���õĿռ�,���ǻ����push_back_aux()
	    //�ȷ���һ�����µĻ��������������µ�Ԫ�ص����ݣ�Ȼ����ĵ�����finish��״̬
	    //ֻ�е� finish.cur == finish.last - 1 ��ʱ��Ż����
		void push_back_aux(const value_type& t) {
			value_type t_copy = t;
			reverse_map_at_back(); //����ĳ������������ػ�һ��map
			*(finish.node + 1) = allocate_node(); //����һ���µĽڵ� (������)
			try {
				data_allocator::allocator<T>::construct(finish.cur, t_copy);//��Ա��Ԫ���趨��ֵ
				finish.set_node(finish.node + 1);//�ı�finish����ָ���µĽڵ�
				finish.cur = finish.first;      //�趨finish��״̬
			}
			catch (std::exception) {
				data_allocator::allocator<T>::deallocate(*(finish.node + 1));
			}
		}

		void push_front(const value_type& t) {
			if (start.cur != start.first) { //��һ���������б��õĿռ�
				data_allocator::allocator<T>::construct(start.cur - 1, t); //ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				--start.cur; //������һ��������ʹ��״̬
			}
			else { //��һ�������Ѿ��ޱ��ÿռ�
				push_back_aux(t);
			}
		}

		//��start.cur == start.firstʱ�Żᱻ����
		//����һ��������û���κα���Ԫ�ص�ʱ��Żᱻ����
		void push_front_aux(const value_type& t) {
			value_type t_copy = t;
			reverse_map_at_front(); //����������������Ҫ���»�һ��map
			*(start.node - 1) = allocate_node(); //����һ���µĽڵ�(������)
			try {
				start.set_node(start.node - 1); //�ı�start������ָ���µĽڵ�
				start.cur = start.cur - 1; //�趨start��״̬
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


		//���ε�ʵ�ʲ�����ͨ������ reallocate_map()���� ��ʵ�ֵ�
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
				//����һ���µ��ڴ� ��mapʹ��
				map_pointer new_map = map_allocator::allocate(new_map_size);
				new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
				//��ԭmap���ݿ�������
				std::copy(start.node, finish.node + 1, new_nstart);
				//�ͷ�ԭmap
				map_allocator::deallocate(map, map_size);
				map = new_map;
				map_size = new_map_size;
			}
			//�����趨 ������ start �� finish
			start.set_node(new_nstart);
			finish.set_node(new_nstart + old_num_nodes - 1);
		}

		//reverse_map_at_front() �� reverse_map_at_back()�������� map
		void reverse_map_at_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size - (finish.node - map)) {
				//���map�ڵ㱸�ÿռ䲻��
				//����������������������滻һ��map (���ø�����ڴ棬������ǰ�ľɵ����䣬�ͷ���ǰ�ľɵĿռ�)
				reallocate_map(nodes_to_add, false);
			}
		}
		void reverse_map_at_front(size_type nodes_to_add = 1) {
			if (nodes_to_add > start.node - map) {
				//���map��ǰ�˽ڵ㱸�ÿռ䲻��
				//����������������������滻һ��map (���ø�����ڴ棬������ǰ�ľɵ����䣬�ͷ���ǰ�ľɵĿռ�)
				reallocate_map(nodes_to_add, true);
			}
		}

		void pop_back() {
			if (finish.cur != finish.first) {
				//���һ������������һ�� ���� �����Ԫ��
				--finish.cur; //����ָ�룬�൱���ų�������Ԫ��
				data_allocator::allocator<T>::destroy(finish.cur); //��������Ԫ��
			}
			else {
				pop_back_aux();
			}
		}

		//finish.cur == finish.first
		void pop_back_aux() {
			deallocate_node(finish.first); //�ͷ����һ��������
			finish.set_node(finish.node - 1);//����finish��״̬
			finish.cur = finish.last - 1;//��finish��curָ��ָ�����һ��Ԫ��
			data_allocator::allocator<T>::destroy(finish.cur);//����Ԫ�ص�����
		}

		void pop_front() {
			if (start != start.last - 1) {
				//��һ��������һ�����߶��Ԫ��
				data_allocator::allocator<T>::destroy(start.cur);//������ǰ��Ԫ��
				++start.cur; //����ָ�룬�൱���ų��˵�һ��Ԫ��
			}
			else {
				pop_back_aux(); //���ｫ���л��������ͷŹ���
			}
		}

		//��start.cur == start.last-1������
		void pop_front_aux() {
			data_allocator::allocator<T>::destroy(start.cur); //����һ��������ʣ��һ��Ԫ������
			deallocate_node(start.first); //�ͷŵ�һ��������
			start.set_node(start.node + 1);//����start��״̬
			start.cur = start.first; //��startָ�򻺳����ĵ�һ��Ԫ��
		}

		//�������deque
		//��Ҫע����� deque�����״̬(���κ�Ԫ�ص�ʱ��) ��֤��һ��������
		void clear() {
			//���ͷβָ�������ÿһ������������Ӧ���Ǳ�����
			for (map_pointer node = start.node + 1; node < finish.node - 1; ++node) {
				//���������ڵ�����Ԫ��ȫ������
				//����destroy()�ĵڶ��汾
				data_allocator::allocator<T>::deallocate(*node, buffer_size());
			}
			if (start.node != finish.node) {//������ͷβ����������
				data_allocator::allocator<T>::destroy(start.cur, start.last); //��ͷ������Ŀǰ���е�Ԫ������
				data_allocator::allocator<T>::destroy(finish.first, finish.cur); //��β������Ŀǰ���е�Ԫ������
				//�ͷ�β�������ĵ������ڴ棬���Ǳ���ͷ��������
				data_allocator::deallocate(finish.first, buffer_size());
			}
			else {
				//ֻ��һ��������
				//��Ψһ�������ڵ�����Ԫ������
				//ע�⣺�����ͷŻ������Ŀռ䣬����Ψһ�Ļ���������
				data_allocator::allocator<T>::destroy(start.cur, finish.cur);
			}
			//����״̬
			finish = start;
		}

		//ʹ��erase�����ض�λ���ϵ�Ԫ��
		//����posָ���Ԫ�أ�posΪ�����
		iterator erase(iterator pos) {
			iterator next = pos;
			++next;
			difference_type index = pos - start; //�����֮ǰ��Ԫ�صĸ���
			if (index < (size() >> 1)) { //��������֮ǰ��Ԫ�رȽ���
				std::copy_backward(start, pos, next);//�ƶ������֮ǰ��Ԫ��
				pop_front(); //�ƶ���ϣ�����ǰһ��Ԫ��ɾ��
			}
			else {
				//�����֮���Ԫ�ؽ���
				std::copy(next, finish, pos);//�ƶ������֮���Ԫ��
				pop_back();//�ƶ���� �����һ�������Ԫ��ɾ��
			}
			return start + index;
		}

		//ʹ��eraseɾ��[first,last)�����ڵ�����Ԫ��
		// Ҳ����ɾ��ָ����Χ�ڵ�һ��Ԫ��
		iterator erase(iterator first, iterator last) {
			if (first == start && last == finish) {
				//���ɾ�����������ڴ�ռ䣬ֱ�ӵ���clear()
				clear();
				return finish;
			}
			else {
				difference_type n = last - first;        //�������ĳ���
				difference_type elements_before = first - start; //�������ǰ����Ԫ�ظ���
				if (elements_before < (size() - n) / 2) {  //���ǰ����Ԫ�رȽ���
					//copy_backward��copy�������ƣ�ֻ�������Ԫ�ؽ��и���
					std::copy_backward(start, first, last); //����ƶ�ǰ����Ԫ��(�����������)
					iterator new_start = start + n; //���deque���µ����
					data_allocator::allocator<T>::destroy(start, new_start); //�ƶ����֮�󣬽������Ԫ������
					//���½�����Ļ������ͷ�
					for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
						data_allocator::deallocate(*cur, buffer_size());
					}
					start = new_start;//�趨deque�������
				}
				else {
					//���ɾ������ĺ󷽵�Ԫ�رȽ���
					std::copy(last, finish, first); //��ǰ�ƶ��󷽵�Ԫ�� (�����������)
					iterator new_finish = finish - n;  //���finish���µ�β��
					data_allocator::allocator<T>::destroy(new_finish, finish);  //�ƶ���ϣ��������Ԫ�ؽ�������
					//���½��ͷ�����Ļ�����
					for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
						data_allocator::deallocate(cur, buffer_size());
					}
					//�趨�µ�β�ڵ�
					finish = new_finish;
				}
				return start + elements_before;
			}
		}

		//��ĳ����֮ǰ����һ��Ԫ�أ����趨��ֵ
		iterator insert(iterator position, const value_type& x) {
			if (position.cur == start.cur) { //����������deque����ǰ��
				push_front(x);
				return start;
			}
			else if (position.cur == finish.cur) { //����������deque������
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
			difference_type index = pos - start;//�����֮ǰ��Ԫ�صĸ���
			value_type x_copy = x;
			if (index < (size() / 2)) {
				//�������ڵ�֮ǰ��Ԫ�صĸ����Ƚ���
				push_front(front());//����ǰ�˼��Ϻ͵�һ��Ԫ����ֵ��ͬ��Ԫ��
				iterator front1 = start;//���±�ǼǺţ�Ȼ�����Ԫ�ص��ƶ�
				++front1; iterator front2 = front1;
				++front2;
				pos = start + index;
				iterator pos1 = pos;
				++pos1;
				std::copy(front2, pos1, front1);//Ԫ�ص��ƶ�
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
			//�ڲ�������趨��ֵ
			*pos = x_copy;
			return pos;
		}

	};
}


#endif