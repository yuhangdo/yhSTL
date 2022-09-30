#ifndef _YHSTL_STACK_H_
#define _YHSTL_STACK_H_
//栈是一个先进后出的数据结构
//栈不允许遍历行为
//栈的底层容器一般是deque，也可以是list，只要在第二模板参数设置即可
//20220930  stack
#include"deque.h"
#include"iterator.h"
namespace yhstl
{
    template <class T, class Sequence = deque<T>>
    class stack {
     
        friend bool operator== (const stack&, const stack&);
        friend bool operator< (const stack&, const stack&);
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        //底层容器
        Sequence c;
    public:
        //以下完全使用Sequence c的操作，完成stack的操作
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }
        //deque 是两头可以进出，stack是末端进，末端出 (所以后进者先出)
        void push(const value_type& x) { c.push_back(x); }
        void pop() { return c.pop_back(); }
    };

    template <class T, class Sequence>
    bool operator==(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
        return x.c == y.c;
    }

    template <class T, class Sequence>
    bool operator<(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
        return x.c < y.c;
    }
}
#endif
