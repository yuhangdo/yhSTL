#ifndef _YHSTL_STACK_H_
#define _YHSTL_STACK_H_
//ջ��һ���Ƚ���������ݽṹ
//ջ�����������Ϊ
//ջ�ĵײ�����һ����deque��Ҳ������list��ֻҪ�ڵڶ�ģ��������ü���
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
        //�ײ�����
        Sequence c;
    public:
        //������ȫʹ��Sequence c�Ĳ��������stack�Ĳ���
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }
        //deque ����ͷ���Խ�����stack��ĩ�˽���ĩ�˳� (���Ժ�����ȳ�)
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
