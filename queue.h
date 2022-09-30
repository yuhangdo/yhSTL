#ifndef _YHSTL_QUEUE_H_
#define _YHSTL_QUEUE_H_
#include "deque.h"
//队列是一个配接器，是一个先进先出的数据结构
namespace yhstl
{
    template <class T, class Sequence = deque<T>>
    class queue {
        //__STL_NULL_TMPL_ARGS会展开为 <>
        friend bool operator==(const queue& x, const queue& y);
        friend bool operator< (const queue& x, const queue& y);
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
        reference front() { return c.back(); }
        const_reference front() const { return c.front(); }
        reference back() const { return c.back(); }
        const_reference back() { return c.back(); }
        //deque 是两头可以进出，queue 是末端进，前端出 (所以先进者先出)
        void push(const value_type& x) { c.push_back(x); }
        void pop() { return c.pop_front(); }
    };

    template <class T, class Sequence>
    bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
        return x.c == y.c;
    }

    template <class T, class Sequence>
    bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
        return x.c < y.c;
    }
}
#endif
