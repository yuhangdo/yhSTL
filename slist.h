#ifndef _YHSTL_SLIST_H_
#define _YHSTL_SLIST_H_
//20221002  开始实现单向链表
//单向链表相比双向循环链表的实现要简单很多
#include"iterator.h"
#include"allocator.h"
namespace yhstl
{
    //单向链表的节点的基本结构
    struct __slist_node_base {
        __slist_node_base* next;
    };

    //单向链表的节点结构
    template <class T>
    struct __slist_node : public __slist_node_base {
        T data;
    };

    //全局函数 已知某一个节点，插入新的节点于其后
    inline __slist_node_base* __slist_make_link(
        __slist_node_base* prev_node, __slist_node_base* new_node) {
        //令new_node节点的下一个节点为prev节点的下一节点
        new_node->next = prev_node->next->next;
        prev_node->next = new_node; //令prev节点的next指针指向new节点
        return new_node;
    }

    //全局函数 单向链表的大小(元素的个数)
    inline std::size_t __slist_size(__slist_node_base* node) {
        std::size_t result = 0;
        for (; node != 0; node = node->next) {
            ++result; //一个一个累计
        }
        return result;
    }

    //单向链表的迭代器的基本结构
    struct __slist_iterator_base {
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category; //注意 单向

        __slist_node_base* node; //指向节点的基本结构 next指针  不含数据

        __slist_iterator_base(__slist_node_base* x) : node(x) {}

        void incr() { node = node->next; } //前进一个节点

        bool operator==(const __slist_iterator_base& x)const {
            return node == x.node;
        }

        bool operator!=(const __slist_iterator_base& x)const {
            return node != x.node;
        }
    };

    //单向链表的迭代器结构
    template <class T, class Ref, class Ptr>
    struct __slist_iterator : public __slist_iterator_base {
        typedef __slist_iterator<T, T&, T*>  iterator;
        typedef __slist_iterator<T, const T&, const T*>const_iterator;
        typedef __slist_iterator<T, T&, T*>  self;
        //加上嵌套型别定义
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __slist_node<T> list_node;  //单向链表的节点是_slist_node，里面含有数据

        //调用slist<T>::end()会造成__slist_iterator(0) 于是调用如下函数
        __slist_iterator(list_node* x) : __slist_iterator_base(x) {}
        __slist_iterator() : __slist_iterator_base(0) {}
        __slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}

        reference operator*() const { return ((list_node*)node)->data; }
        reference operator->() const { return &(operator*()); }

        self& operator++() {
            incr(); //前进一个节点
            return *this;
        }

        self& operator++(int) {
            self tmp = *this;
            incr(); //前进一个节点
            return tmp;
        }
        //没有实现operator-- 因为这是一个单向链表，使用的是forward iterator
    };

    template <class T, class Alloc>
    class slist {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef __slist_iterator<T, T&, T*> iterator;
        typedef __slist_iterator<T, const T&, const T*>const_iterator;
    private:
        typedef __slist_node<T> list_node;
        typedef __slist_node_base list_node_base;
        typedef __slist_iterator_base iterator_base;
        typedef allocator<list_node>list_node_allocator;

        static list_node* create_node(const value_type& x) {
            list_node* node = list_node_allocator::allocate(); //配置空间
            try{
                allocator<T>::construct(&node->data,x);
                node->next = 0;
            };
            catch(list_node_allocator::deallocate(node);) //释放空间  使用commit or rollback机制
        }

        static void destroy_node(list_node* node) {
            allocator<T>::destroy(&node->data);//将元素析构
            list_node_allocator::deallocate(node);  //释放空间
        }
    private:
        list_node_base head; //头部 注意head不是指针，是一个实在的节点，里面含有data数据的
    public:
        slist() { head.next = 0; }     //初始化
        ~slist() { clear(); }          //析构函数
    public:
        void clear() {
            erase_after(&head, 0);
        }
        //全局函数 传递链表的头部和尾部
        inline list_node_base* erase_after(list_node_base* before_first,list_node_base* last_node)
        {
            list_node* cur = (list_node*)(before_first->next);
            while (cur != last_node) 
            {
                list_node* tmp = cur;
                cur = (list_node*)cur->next;
                destroy_node(tmp);
            }
            before_first->next = last_node;
            return last_node;
        }
    public:
        iterator begin() { return iterator((list_node*)head.next); }
        iterator end() { return iterator(0); }
        size_type size() const { return __slist_size(head.next); }
        bool empty() const { return head.next == 0; }
        //两个slist之间互换，只需要交换head头结点即可
        void swap(slist& L) {
            list_node_base* tmp = head.next;
            head.next = L.head.next;
            L.head.next = tmp;
        }
        //获取头部元素
        reference front() { return ((list_node*)head.next)->data; }

        //从头部插入元素 新的元素成为slist的第一个元素
        void push_front(const value_type& x) {
            __slist_make_link(&head, create_node(x));
        }
        //注意 没有push_back() 因为为迭代器类型为forward_iterator_tag

        //从头部取出元素，将其删除之 修改head
        void pop_front() {
            list_node* node = (list_node*)head.next;
            head.next = node->next;
            destroy_node(node);
        }
    };
 }
#endif