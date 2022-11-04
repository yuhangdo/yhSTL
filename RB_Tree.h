#ifndef _YHSTL_RB_TREE_H_
#define _YHSTL_RB_TRRR_H_
//20221104 �������������� �����ű��ѧϰ�������չ���ּ�����ʼ����Ŀ��ѧϰ
#include"iterator.h"
#include"allocator.h"
namespace yhstl
{
    //�Զ���һ���������ͣ���ʾ������ڵ����ɫ��������boolֵ��ʾ����0��1
    typedef bool __rb_tree_color_type;  //��0 ��1
    const __rb_tree_color_type __rb_tree_red = false;  //��ɫΪ0
    const __rb_tree_color_type __rb_tree_black = true; //��ɫΪ1

    //������ڵ�Ľṹ
    //�ں� ��ɫ��ָ�򸸽ڵ��ָ�롢ָ����ڵ��ָ�룬ָ���ҽڵ��ָ��
    //�Ұ���������Ա�������ֱ��ʾ���ֵ����Сֵ����Ҫ����Ϊ���������������ԣ�
    struct __rb_tree_node_base {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base* base_ptr;

        color_type color;   //�ڵ���ɫ �Ǻ켴��
        base_ptr parent;    //RB-Tree�������� ������Ҫ���ڵ�Ĳ���
        base_ptr left;      //ָ����ڵ�
        base_ptr right;     //ָ���ҽڵ�

        static base_ptr minimum(base_ptr x) {
            while (x->left != 0) {
                //����������������
                //һֱ������ �ͻ��ҵ���Сֵ
                x = x->left;
            }
            return x;
        }

        static base_ptr maximum(base_ptr x) {
            while (x->right != 0) {
                //����������������
                //һֱ������ �ͻ��ҵ����ֵ
                x = x->right;
            }
            return x;
        }
    };

    //��������Ľڵ��ʾ�����Ǿ����������Ǹ������ڵ�
    //�½ڵ�̳�����������ڵ�ṹ��������һ�����Ա���Ǳ�ʾ���������ڵ��ֵ
    template <class Value>
    struct __rb_tree_node : public __rb_tree_node_base {
        typedef __rb_tree_node<Value>* link_type;
        Value value_field; //�ڵ�ֵ  
    };


    //RB-Tree�Ľڵ�͵�������ʹ��struct��ɣ���ҪĿ����structĬ��ʹ��public�����Ա��������ȡ��(��������Ϳ�����չc++��struct��class�������)
    //����������Ϊ˫��ṹ
    //���������
    struct __rb_tree_base_iterator {
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef yhstl::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;

        base_ptr node; //���ں�����֮�����һ�����ӹ�ϵ(make a reference)

        //����ʵ����operator++�ڣ���Ϊ���������ط���������������
        //�������ǰ���ڵ�
        void increment() { 
            if (node->right != 0) {        //������ҽڵ㣬״��һ
                node = node->right;       //��������
                while (node->left != 0) {   //Ȼ��һֱ��������ǰ��
                    node = node->left;    //��Ϊ��
                }
            }
            else {                       //û���ҽڵ� ״����
                base_ptr y = node->parent;//�ҳ����ڵ�
                while (node == y->right) {  //������нڵ㱾���Ǹ����ӽڵ�
                    node = y;             //��Ҫһֱ����  ֱ����Ϊ���ӽڵ�Ϊֹ
                    y = y->parent;
                }
                if (node->right != y) {   //�����ʱnode�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ� ��Ҫ�Ǻ������������
                    node = y;             //״���� ��ʱ�ĸ��ڵ㼴Ϊ��
                }                         //���� ��ʱ��node��Ϊ���  ״����
            }
            /*�����ж� "����ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ�"��Ϊ��Ӧ��һ����������
             * ������Ѱ�ҵĸ��ڵ����һ���ڵ㣬����ʱ���ڵ�ǡ��û�����ӽڵ�
             * ��Ȼ������������������Ҫ���RB-Tree���ڵ�������header֮�������Ĺ�ϵ
             */
        }

        //����ʵ����operator--�ڣ���Ϊ���������ط���������������
        void decrement() {
            //״��һ
            //����Ǻ�ڵ� �� ���ڵ�ĸ��ڵ�����Լ� ���ӽڵ㼴Ϊ��
            //״��һ ������nodeΪheader��ʱ��(�༴nodeΪend()ʱ)
            //ע�⣺header�����ӽڵ� �� mostrightָ����������max�ڵ�
            if (node->color == __rb_tree_red && node->right->right == node) {
                node = node->right;
            }
            //״����
            //��������ӽڵ� ��yָ�����ӽڵ㣻ֻ�е�y�����ӽڵ��ʱ�� һֱ������ ���ף����Ϊ��
            else if (node->left != 0) {
                base_ptr y = node->left;
                while (y->right != 0) {
                    y = y->right;
                }
                node = y;
            }
            else {
                //״����
                //�Ȳ��Ǹ��ڵ� Ҳû����ڵ�
                //�ҳ����ڵ㣬�����еĽڵ���Ϊ���ӽڵ�ʱ��һֱ���������ߣ�ֱ�����нڵ㲻����ڵ�Ϊֹ
                base_ptr y = node->parent;
                while (node == y->left) {
                    node = y;
                    y = y->right;
                }
                node = y; //��ʱ���ڵ㼴Ϊ��
            }
        }
    };

    //RB-Tree�����������
    template <class Value, class Ref, class Ptr>
    struct __rb_tree_iterator : public __rb_tree_base_iterator {
        typedef Value value_type;  //�Ȳ�ȫǶ���ͱ���
        typedef Ref reference;
        typedef Ptr pointer;
        typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
        typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
        typedef __rb_tree_iterator<Value, Value&, Value*> self;  //ָ������ĵ�����
        typedef __rb_tree_node<Value>* link_type;  //ָ�������ڵ��ָ��

        __rb_tree_iterator() {}
        __rb_tree_iterator(link_type x) { node = x; }
        __rb_tree_iterator(const iterator& it) { node = it.node; }

        reference operator* () const { return link_type(node)->value_field; }

        reference operator->()const { return &(operator*()); }
        self& operator++() {
            increment();
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            increment();
            return tmp;
        }

        self& operator--() {
            decrement();
            return *this;
        }
        self operator--(int) {
            self tmp = *this;
            decrement();
            return tmp;
        }
    };

    //���������

    template <class Key, class Value, class KeyOfValue, class Compare>
    class rb_tree {
    protected:
        typedef void* void_pointer;
        typedef __rb_tree_node_base* base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef allocator<rb_tree_node>rb_tree_node_allocator;  //ר���ڵ�����
        typedef __rb_tree_color_type color_type;
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef rb_tree_node* link_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
    protected:
        link_type get_node() { return rb_tree_node_allocator::allocate(); }
        void put_node(link_type p) { return rb_tree_node_allocator::deallocate(p); }
        link_type create_node(const value_type& x) {
            link_type tmp = get_node(); //���ÿռ�
            try{
                allocator<Key>::construct(&tmp->value_field,x);//��������
            };
            catch (...)
            {
                (put_node(tmp)); //commit or rollback  ʵ�ִ�����
            }
            return tmp;
        }
        link_type clone_node(link_type x) {//����һ���ڵ����ɫ����ֵ
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right = 0;
            return tmp;
        }

        void destroy_node(link_type p) {
            allocator<Key>::destroy(&p->value_field); //��������
            put_node(p); //�ͷ��ڴ�
        }
    protected:
        //RB-treeֻʹ���������ݱ���
        size_type node_count; //׷�ټ�¼���Ĵ�С (�ڵ������)
        link_type header;     //ʵ���ϵ�С����
        Compare key_compare;  //�ڵ�֮��ļ�ֵ��С�ıȽ�׼��. Ӧ�û���һ��function object
        //���������������ڷ����ȡheader�ĳ�Ա
        link_type& root() const { return (link_type&)header->parent; }
        link_type& left_most() const { return (link_type&)header->left; }
        link_type& right_most() const { return (link_type&)header->right; }
        //���������������ڷ����ýڵ�x�ĳ�Ա
        static link_type& left(link_type x) { return(link_type&)x->left; }
        static link_type& right(link_type x) { return(link_type&)x->right; }
        static link_type& parent(link_type x) { return(link_type&)x->parent; }
        static reference value(link_type x) { return x->value_field; }
        static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
        static color_type& color(link_type x) { return (color_type&)(x->color); }
        //��ȡ����ֵ�ͼ�Сֵ node class��ʵ�ִ˹���
        static link_type minimum(link_type x) {
            return (link_type)__rb_tree_node_base::minimum(x);
        }
        static link_type maximum(link_type x) {
            return (link_type)__rb_tree_node_base::maximum(x);
        }
        //������
        typedef __rb_tree_iterator<value_type, reference, pointer>iterator;
    private:
        iterator __insert(base_ptr x, base_ptr y, const value_type& v);
        link_type __copy(link_type x, link_type p);
        void __erase(link_type x);

        void clear() {
            if (node_count != 0) {
                __erase(root());
                left_most() = header;
                root() = 0;
                right_most() = header;
                node_count = 0;
            }
        }

        void init() {
            header = get_node(); //����һ���ڵ�ռ� ��headerָ����
            color(header) = __rb_tree_red;//��headerΪ��ɫ ��������header��root,��iterator��operator++��
            root() == 0;
            left_most() = header;   //��header�����ӽڵ�����Լ�
            right_most() = header;  //��header�����ӽڵ�����Լ�
        }
    public:
        //allocation / deallocation
        rb_tree(const Compare& comp = Compare())
            : node_count(0), key_compare(comp) {
            init();
        }
        ~rb_tree() {
            clear();
            put_node(header);
        }

        rb_tree<Key, Value, KeyOfValue, Compare>& operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);
    public:
        //accessors
        Compare key_comp() const { return key_compare; }
        iterator begin() { return left_most(); } //RB������ͷΪ����(��С)�ڵ㴦
        iterator end() { return header; } //RB�����յ�Ϊheader��ָ��
        bool empty() const { return node_count == 0; }
        size_type size() const { return node_count; }
        size_type max_size() const { return size_type(-1); }
    public:
        //insert/erase
        //��x���뵽RB-Tree�� (���ֽڵ�Ķ�һ�޶�)
        std::pair<iterator, bool> insert_unique(const value_type& x);
        //��x���뵽RB-Tree�� (����ڵ���ֵ�ظ�)
        iterator insert_equal(const value_type& x);
    };
}

#endif
