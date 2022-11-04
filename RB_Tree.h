#ifndef _YHSTL_RB_TREE_H_
#define _YHSTL_RB_TRRR_H_
//20221104 今天是阳历生日 伴随着编程学习及课题进展，又继续开始了项目的学习
#include"iterator.h"
#include"allocator.h"
namespace yhstl
{
    //自定义一个变量类型，表示红黑树节点的颜色，就是用bool值表示，红0黑1
    typedef bool __rb_tree_color_type;  //红0 黑1
    const __rb_tree_color_type __rb_tree_red = false;  //红色为0
    const __rb_tree_color_type __rb_tree_black = true; //黑色为1

    //红黑树节点的结构
    //内涵 颜色、指向父节点的指针、指向左节点的指针，指向右节点的指针
    //且包含两个成员函数，分别表示最大值和最小值（主要是因为二叉搜索树的特性）
    struct __rb_tree_node_base {
        typedef __rb_tree_color_type color_type;
        typedef __rb_tree_node_base* base_ptr;

        color_type color;   //节点颜色 非红即黑
        base_ptr parent;    //RB-Tree的许多操作 都是需要父节点的参与
        base_ptr left;      //指向左节点
        base_ptr right;     //指向右节点

        static base_ptr minimum(base_ptr x) {
            while (x->left != 0) {
                //二叉搜索树的特性
                //一直向左走 就会找到最小值
                x = x->left;
            }
            return x;
        }

        static base_ptr maximum(base_ptr x) {
            while (x->right != 0) {
                //二叉搜索树的特性
                //一直向右走 就会找到最大值
                x = x->right;
            }
            return x;
        }
    };

    //最后红黑树的节点表示并不是就是用上面那个基础节点
    //新节点继承了这个基础节点结构，加上了一个类成员就是表示这个红黑树节点的值
    template <class Value>
    struct __rb_tree_node : public __rb_tree_node_base {
        typedef __rb_tree_node<Value>* link_type;
        Value value_field; //节点值  
    };


    //RB-Tree的节点和迭代器都使用struct完成，主要目的是struct默认使用public，可以被外界自由取用(可能这里就可以扩展c++的struct和class的区别等)
    //迭代器设置为双层结构
    //基层迭代器
    struct __rb_tree_base_iterator {
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef yhstl::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;

        base_ptr node; //用于和容器之间产生一个连接关系(make a reference)

        //以下实现于operator++内，因为再无其他地方会调用这个函数了
        //红黑树的前进节点
        void increment() { 
            if (node->right != 0) {        //如果有右节点，状况一
                node = node->right;       //就向右走
                while (node->left != 0) {   //然后一直往左子树前进
                    node = node->left;    //即为答案
                }
            }
            else {                       //没有右节点 状况二
                base_ptr y = node->parent;//找出父节点
                while (node == y->right) {  //如果现行节点本身是个右子节点
                    node = y;             //需要一直上溯  直到不为右子节点为止
                    y = y->parent;
                }
                if (node->right != y) {   //如果此时node的右子节点不等于此时的父节点 主要是红黑树特殊的设计
                    node = y;             //状况三 此时的父节点即为答案
                }                         //否则 此时的node即为解答  状况四
            }
            /*以上判断 "若此时的右子节点不等于此时的父节点"是为了应对一种特殊的情况
             * 即：欲寻找的根节点的下一个节点，而此时根节点恰好没有右子节点
             * 当然，上述的特殊做法需要配合RB-Tree根节点和特殊的header之间的特殊的关系
             */
        }

        //以下实现于operator--内，因为再无其他地方会调用这个函数了
        void decrement() {
            //状况一
            //如果是红节点 且 父节点的父节点等于自己 右子节点即为答案
            //状况一 发生于node为header的时候(亦即node为end()时)
            //注意：header的右子节点 即 mostright指向整棵树的max节点
            if (node->color == __rb_tree_red && node->right->right == node) {
                node = node->right;
            }
            //状况二
            //如果有左子节点 令y指向左子节点；只有当y有右子节点的时候 一直往右走 到底，最后即为答案
            else if (node->left != 0) {
                base_ptr y = node->left;
                while (y->right != 0) {
                    y = y->right;
                }
                node = y;
            }
            else {
                //状况三
                //既不是根节点 也没有左节点
                //找出父节点，当现行的节点身为左子节点时，一直交替往上走，直到现行节点不是左节点为止
                base_ptr y = node->parent;
                while (node == y->left) {
                    node = y;
                    y = y->right;
                }
                node = y; //此时父节点即为答案
            }
        }
    };

    //RB-Tree的正规迭代器
    template <class Value, class Ref, class Ptr>
    struct __rb_tree_iterator : public __rb_tree_base_iterator {
        typedef Value value_type;  //先补全嵌套型别定义
        typedef Ref reference;
        typedef Ptr pointer;
        typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
        typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
        typedef __rb_tree_iterator<Value, Value&, Value*> self;  //指向自身的迭代器
        typedef __rb_tree_node<Value>* link_type;  //指向红黑树节点的指针

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

    //迭代器完结

    template <class Key, class Value, class KeyOfValue, class Compare>
    class rb_tree {
    protected:
        typedef void* void_pointer;
        typedef __rb_tree_node_base* base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef allocator<rb_tree_node>rb_tree_node_allocator;  //专属节点配置
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
            link_type tmp = get_node(); //配置空间
            try{
                allocator<Key>::construct(&tmp->value_field,x);//构造内容
            };
            catch (...)
            {
                (put_node(tmp)); //commit or rollback  实现处理函数
            }
            return tmp;
        }
        link_type clone_node(link_type x) {//复制一个节点的颜色和数值
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right = 0;
            return tmp;
        }

        void destroy_node(link_type p) {
            allocator<Key>::destroy(&p->value_field); //析构内容
            put_node(p); //释放内存
        }
    protected:
        //RB-tree只使用三笔数据表现
        size_type node_count; //追踪记录树的大小 (节点的数量)
        link_type header;     //实现上的小技巧
        Compare key_compare;  //节点之间的键值大小的比较准则. 应该会是一个function object
        //以下三个函数用于方便获取header的成员
        link_type& root() const { return (link_type&)header->parent; }
        link_type& left_most() const { return (link_type&)header->left; }
        link_type& right_most() const { return (link_type&)header->right; }
        //以下六个函数用于方便获得节点x的成员
        static link_type& left(link_type x) { return(link_type&)x->left; }
        static link_type& right(link_type x) { return(link_type&)x->right; }
        static link_type& parent(link_type x) { return(link_type&)x->parent; }
        static reference value(link_type x) { return x->value_field; }
        static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
        static color_type& color(link_type x) { return (color_type&)(x->color); }
        //获取极大值和极小值 node class有实现此功能
        static link_type minimum(link_type x) {
            return (link_type)__rb_tree_node_base::minimum(x);
        }
        static link_type maximum(link_type x) {
            return (link_type)__rb_tree_node_base::maximum(x);
        }
        //迭代器
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
            header = get_node(); //产生一个节点空间 令header指向它
            color(header) = __rb_tree_red;//令header为红色 用于区分header和root,在iterator的operator++中
            root() == 0;
            left_most() = header;   //令header的左子节点等于自己
            right_most() = header;  //令header的右子节点等于自己
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
        iterator begin() { return left_most(); } //RB树的起头为最左(最小)节点处
        iterator end() { return header; } //RB树的终点为header所指处
        bool empty() const { return node_count == 0; }
        size_type size() const { return node_count; }
        size_type max_size() const { return size_type(-1); }
    public:
        //insert/erase
        //将x插入到RB-Tree中 (保持节点的独一无二)
        std::pair<iterator, bool> insert_unique(const value_type& x);
        //将x插入到RB-Tree中 (允许节点数值重复)
        iterator insert_equal(const value_type& x);
    };
}

#endif
