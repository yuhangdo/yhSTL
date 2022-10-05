#ifndef _YHSTL_PRIORITY_QUEUE_H_
#define _YHSTL_PRIORITY_QUEUE_H_
#include"vector.h"
#include"heap_algorithm.h"
//优先队列底层使用heap，heap底层使用vector
//priority_queue其实不被归类为容器，而是配接器
namespace yhstl
{
    template <class T, class Sequence = vector<T>, class Compare = std::less<typename Sequence::value_type>>
    class priority_queue 
    {
        public:
            //先是嵌套型别定义
            typedef typename Sequence::valuetype value_type;
            typedef typename Sequence::size_type size_type;
            typedef typename Sequence::reference reference;
            typedef typename Sequence::const_reference const_reference;
        protected:
            Sequence c; //底层容器
            Compare comp; //元素大小的比较标准
        public:
            priority_queue() : c() {}
            explicit priority_queue(const Compare& x) : c(), comp(x) {}   //必须要有explicit关键词，以防隐式转换
            
            //注意 任意一个构造函数都立刻在底层容器内产生一个implicit representation heap
            //指代比较器
            template<class InputIterator>
            priority_queue(InputIterator first, InputIterator last, const Compare& x) :
                c(first, last), comp(x) {
                __make_heap(c.begin(), c.end(), comp);  //构造堆
            }
            //没有指代比较器
            template<class InputIterator>
            priority_queue(InputIterator first, InputIterator last) :
                c(first, last) {
                _make_heap(c.begin(), c.end(), comp);    //使用默认比较器函数
            }
            bool empty()const {
                return c.empty();
            }
            size_type size() const {
                return c.size();
            }
            const_reference top()const {
                return c.front();
            }
            void push(const value_type& x) {
                try{
                    //push_heap 是泛型算法，先利用底层的push_back()将新的元素推入末端，再重新排列heap
                    c.push_back(x);
                //push_heap 是泛型算法
                    push_heap(c.begin(),c.end(),comp);
                }
                catch(c.clear());   //使用commit or rollback机制
            }
            void pop() {
                try{
                    //pop_heap是泛型算法，从heap中取出一个元素，并不是整整将这个元素弹出，而是重排heap
                    //需要使用底层vector的pop_back()取得被弹出的元素
                    pop_heap(c.begin(),c.end(),comp);
                    c.pop_back();
                }
                catch(c.clear());
            }


    };

}


#endif
