#ifndef _YHSTL_SLIST_H_
#define _YHSTL_SLIST_H_
//20221002  ��ʼʵ�ֵ�������
//�����������˫��ѭ�������ʵ��Ҫ�򵥺ܶ�
#include"iterator.h"
#include"allocator.h"
namespace yhstl
{
    //��������Ľڵ�Ļ����ṹ
    struct __slist_node_base {
        __slist_node_base* next;
    };

    //��������Ľڵ�ṹ
    template <class T>
    struct __slist_node : public __slist_node_base {
        T data;
    };

    //ȫ�ֺ��� ��֪ĳһ���ڵ㣬�����µĽڵ������
    inline __slist_node_base* __slist_make_link(
        __slist_node_base* prev_node, __slist_node_base* new_node) {
        //��new_node�ڵ����һ���ڵ�Ϊprev�ڵ����һ�ڵ�
        new_node->next = prev_node->next->next;
        prev_node->next = new_node; //��prev�ڵ��nextָ��ָ��new�ڵ�
        return new_node;
    }

    //ȫ�ֺ��� ��������Ĵ�С(Ԫ�صĸ���)
    inline std::size_t __slist_size(__slist_node_base* node) {
        std::size_t result = 0;
        for (; node != 0; node = node->next) {
            ++result; //һ��һ���ۼ�
        }
        return result;
    }

    //��������ĵ������Ļ����ṹ
    struct __slist_iterator_base {
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category; //ע�� ����

        __slist_node_base* node; //ָ��ڵ�Ļ����ṹ nextָ��  ��������

        __slist_iterator_base(__slist_node_base* x) : node(x) {}

        void incr() { node = node->next; } //ǰ��һ���ڵ�

        bool operator==(const __slist_iterator_base& x)const {
            return node == x.node;
        }

        bool operator!=(const __slist_iterator_base& x)const {
            return node != x.node;
        }
    };

    //��������ĵ������ṹ
    template <class T, class Ref, class Ptr>
    struct __slist_iterator : public __slist_iterator_base {
        typedef __slist_iterator<T, T&, T*>  iterator;
        typedef __slist_iterator<T, const T&, const T*>const_iterator;
        typedef __slist_iterator<T, T&, T*>  self;
        //����Ƕ���ͱ���
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __slist_node<T> list_node;  //��������Ľڵ���_slist_node�����溬������

        //����slist<T>::end()�����__slist_iterator(0) ���ǵ������º���
        __slist_iterator(list_node* x) : __slist_iterator_base(x) {}
        __slist_iterator() : __slist_iterator_base(0) {}
        __slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}

        reference operator*() const { return ((list_node*)node)->data; }
        reference operator->() const { return &(operator*()); }

        self& operator++() {
            incr(); //ǰ��һ���ڵ�
            return *this;
        }

        self& operator++(int) {
            self tmp = *this;
            incr(); //ǰ��һ���ڵ�
            return tmp;
        }
        //û��ʵ��operator-- ��Ϊ����һ����������ʹ�õ���forward iterator
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
            list_node* node = list_node_allocator::allocate(); //���ÿռ�
            try{
                allocator<T>::construct(&node->data,x);
                node->next = 0;
            };
            catch(list_node_allocator::deallocate(node);) //�ͷſռ�  ʹ��commit or rollback����
        }

        static void destroy_node(list_node* node) {
            allocator<T>::destroy(&node->data);//��Ԫ������
            list_node_allocator::deallocate(node);  //�ͷſռ�
        }
    private:
        list_node_base head; //ͷ�� ע��head����ָ�룬��һ��ʵ�ڵĽڵ㣬���溬��data���ݵ�
    public:
        slist() { head.next = 0; }     //��ʼ��
        ~slist() { clear(); }          //��������
    public:
        void clear() {
            erase_after(&head, 0);
        }
        //ȫ�ֺ��� ���������ͷ����β��
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
        //����slist֮�以����ֻ��Ҫ����headͷ��㼴��
        void swap(slist& L) {
            list_node_base* tmp = head.next;
            head.next = L.head.next;
            L.head.next = tmp;
        }
        //��ȡͷ��Ԫ��
        reference front() { return ((list_node*)head.next)->data; }

        //��ͷ������Ԫ�� �µ�Ԫ�س�Ϊslist�ĵ�һ��Ԫ��
        void push_front(const value_type& x) {
            __slist_make_link(&head, create_node(x));
        }
        //ע�� û��push_back() ��ΪΪ����������Ϊforward_iterator_tag

        //��ͷ��ȡ��Ԫ�أ�����ɾ��֮ �޸�head
        void pop_front() {
            list_node* node = (list_node*)head.next;
            head.next = node->next;
            destroy_node(node);
        }
    };
 }
#endif