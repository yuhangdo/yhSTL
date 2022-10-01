#ifndef _YHSTL_HEAP_H_
#define _YHSTL_HEAP_H_
//20221001 ����heap�㷨
//��ͷ�ļ���Ҫʵ��heap�㷨
//heapû�е�����
#include"iterator.h"

namespace yhstl
{
    //push_heap�㷨
    //����˵�����ڴ���ѹ�����̣���Ԫ�ض����ȷ���ĩ��λ�ã�Ȼ��ᾭ��һ�����ݵĹ��̣��븸�ڵ�ֵ���бȽ�
    //ֱ������¼���ڵ�ŵ��÷��õ�λ��
    //����ʵ�־�������push_heap����_push_heap_aux�������ٵ���_push_heap�����ݺ�����
    template<class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        //���������õ�ʱ�� ��Ԫ��Ӧ�Ѿ����ڵײ���������β��
        //distance_type �����жϵ����������ͣ��Ӷ��ж��Ƿ����ƫ�ػ�����
        __push_heap_aux(first, last, distance_type(first), value_type(first));
    }

    template <class RandomAccessIterator, class Distance, class T>
    inline void __push_heap_aux(RandomAccessIterator first,
        RandomAccessIterator last,
        Distance*, T*) {
        //����implicit representation heap�Ľṹ���ԣ���ֵ�������ڵײ���������β�ˣ�����һ������(last-first)-1
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
    }

    //��������push_back() ������ָ�� "��С�Ƚϵı�׼"
    
    template <class RandomAccessIterator, class Distance, class T>
    void __push_heap(RandomAccessIterator first, Distance holeIndex,
        Distance topIndex, T value) 
    {
        Distance parent = (holeIndex - 1) / 2; //�ҳ����ڵ�
        //��holeIndex��û�е��ﶥ���ͬʱ ���ڵ㻹С����ֵ ��������heap�Ĵ������ԣ�
        //ʹ��operator < ����Ԫ�صıȽϣ����STL heap��һ��max-heap
        //���ڵ��Ƕ�̬�仯��
        //ÿ���Ǻ�value���бȽϣ���˲���Ҫ���и���Ԫ��ֵ�ĶԵ���ֱ������Ԫ�ؽӹܸ��׵���ֵ
        while (holeIndex > topIndex && *(first + parent) < value) {
            *(first + holeIndex) = *(first + parent); //�ֵΪ��ֵ,Ҳ���Ǻ��Ӹ����˸��׵���ֵ������λ��
            holeIndex = parent; //�������ţ��������������ڵ�
            parent = (holeIndex - 1) / 2;//�¶��ĸ��ڵ�
        }//���������ˣ��������� heap�Ĵ�������Ϊֹ
        *(first + holeIndex) = value;//���¶�Ϊ�µ���ֵ����ɲ������
    }

    //pop_heap   ִ��һ�����ݲ���  push_heap��pop_heap����һ����ʾ���ڵĲ���
    //�����û��ָ�� ��С�Ƚϵı�׼
    template <class RandomAccessIterator, class T, class Distance>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
        RandomAccessIterator result, T value, Distance*) {
        *result = *first; //�趨βֵΪ��ֵ��Ҳ���Ǵ˿̵�βֵΪ��ǰ�ĸ��ڵ�
        //����ͨ���ײ������� pop_back()ȡ��βֵ
        //���µ��� heap ����Ϊ0(��������)��������ֵ�趨Ϊ value(��ǰ��β�ڵ����ֵ)
        __adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    //�����û��ָ�� ��С�Ƚϵı�׼
    template <class RandomAccessIterator, class Distance, class T>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
        Distance len, T value) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2;//���ڵ�����ӽڵ�
        while (secondChild < len) 
        {
            //�Ƚ϶��ڵ�������������ӵ���ֵ��Ȼ����secondChild����ϴ���ӽڵ�
            if (*(first + secondChild) < *(first + secondChild - 1))  
            {
                secondChild--;
            }
            //��ϴ����ֵΪ��ֵ ������½����ϴ���ӽڵ㴦
            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;        //�����д��������
            //�ҳ��¶��ڵ�����ӽڵ�
            secondChild = 2 * (secondChild + 1);
        }
        if (secondChild == len) 
        {
            //û���ұ߽ڵ� ֻ����ߵĽڵ�
            //������ֵ��Ϊ��ֵ ����������� ���ӽڵ㴦
            *(first + holeIndex) = *(first + secondChild - 1);
            holeIndex = secondChild - 1;
        }
        *(first + holeIndex) = value;   //Դ������ʵ��������ִ����push_heap����������û��Ҫ����Ϊ�Ѿ��Ǵ�����
    }

    //sort_heap��ͨ������pop_heap
    //���õ������о��ǵ������У����Ǵ�ʱԭ����heap�Ͳ���һ�����������heap��
    template <class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) 
    {
        //ÿִ��һ��pop_heap(),��ֵ(��STL heap��Ϊ����ֵ)�ͻᱻ����β��
        //�۳�β���ٴ�ִ��pop_heap()���μ�ֵ�ֱ������µ�β�ˣ�������������һֱִ�У����õ�������
        while (last - first > 1) 
        {
            pop_heap(first, last--);//ÿ��ִ��pop_heap()һ�Σ�������Χ������һ��
        }
    }

    //make_heap����
    //ʵ��˼·�������ҵ����һ�����ڵ���±�  �� (len - 2) / 2
    //Ȼ�������±���Ϊ���ڣ�����adjust_heap������Ϊ������Ȼ��ݼ�����±ֱ꣬��Ϊ���ڵ�
    //�����make_heap��ʵ�֣�
    template <class RandomAccessIterator, class T, class Distance>
    void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
    {
        if (last - first < 2)
        { //�������Ϊ0����Ϊ1������Ҫ��������
            return;
        }
        Distance len = last - first;
        //�ҳ���һ����Ҫ�������е�������ͷ������parent�궨
        //���ǵ��κνڵ㲻��Ҫִ��perlocate down�����ʹ��hole Index������������
        Distance parent = (len - 2) / 2;   //ע�����ͨ��ģ�⣬֪��(len-2)/2��ֵΪ���һ�����ڵ�
        while (true) {
            //����������parentΪ�׵�����
            //len��Ŀ����Ϊ����__adjust_heap() �жϲ����ķ�Χ
            __adjust_heap(first, parent, len, T(*(first + parent)));
            if (parent == 0) {
                return; //������ڵ� ����
            }
            parent--; // (��������������)ͷ����ǰһ���ڵ�
        }
    }
}
#endif
