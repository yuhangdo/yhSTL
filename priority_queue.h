#ifndef _YHSTL_PRIORITY_QUEUE_H_
#define _YHSTL_PRIORITY_QUEUE_H_
#include"vector.h"
#include"heap_algorithm.h"
//���ȶ��еײ�ʹ��heap��heap�ײ�ʹ��vector
//priority_queue��ʵ��������Ϊ���������������
namespace yhstl
{
    template <class T, class Sequence = vector<T>, class Compare = std::less<typename Sequence::value_type>>
    class priority_queue 
    {
        public:
            //����Ƕ���ͱ���
            typedef typename Sequence::valuetype value_type;
            typedef typename Sequence::size_type size_type;
            typedef typename Sequence::reference reference;
            typedef typename Sequence::const_reference const_reference;
        protected:
            Sequence c; //�ײ�����
            Compare comp; //Ԫ�ش�С�ıȽϱ�׼
        public:
            priority_queue() : c() {}
            explicit priority_queue(const Compare& x) : c(), comp(x) {}   //����Ҫ��explicit�ؼ��ʣ��Է���ʽת��
            
            //ע�� ����һ�����캯���������ڵײ������ڲ���һ��implicit representation heap
            //ָ���Ƚ���
            template<class InputIterator>
            priority_queue(InputIterator first, InputIterator last, const Compare& x) :
                c(first, last), comp(x) {
                __make_heap(c.begin(), c.end(), comp);  //�����
            }
            //û��ָ���Ƚ���
            template<class InputIterator>
            priority_queue(InputIterator first, InputIterator last) :
                c(first, last) {
                _make_heap(c.begin(), c.end(), comp);    //ʹ��Ĭ�ϱȽ�������
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
                    //push_heap �Ƿ����㷨�������õײ��push_back()���µ�Ԫ������ĩ�ˣ�����������heap
                    c.push_back(x);
                //push_heap �Ƿ����㷨
                    push_heap(c.begin(),c.end(),comp);
                }
                catch(c.clear());   //ʹ��commit or rollback����
            }
            void pop() {
                try{
                    //pop_heap�Ƿ����㷨����heap��ȡ��һ��Ԫ�أ����������������Ԫ�ص�������������heap
                    //��Ҫʹ�õײ�vector��pop_back()ȡ�ñ�������Ԫ��
                    pop_heap(c.begin(),c.end(),comp);
                    c.pop_back();
                }
                catch(c.clear());
            }


    };

}


#endif
