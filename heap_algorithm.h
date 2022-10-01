#ifndef _YHSTL_HEAP_H_
#define _YHSTL_HEAP_H_
//20221001 结束heap算法
//本头文件主要实现heap算法
//heap没有迭代器
#include"iterator.h"

namespace yhstl
{
    //push_heap算法
    //简单来说就是在大根堆构造过程，新元素都是先放在末端位置，然后会经过一个上溯的过程，与父节点值进行比较
    //直到最后新加入节点放到该放置的位置
    //具体实现就是先用push_heap调用_push_heap_aux，后者再调用_push_heap（上溯函数）
    template<class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        //函数被调用的时候 新元素应已经置于底部容器的最尾端
        //distance_type 用于判断迭代器的类型，从而判定是否进行偏特化操作
        __push_heap_aux(first, last, distance_type(first), value_type(first));
    }

    template <class RandomAccessIterator, class Distance, class T>
    inline void __push_heap_aux(RandomAccessIterator first,
        RandomAccessIterator last,
        Distance*, T*) {
        //根据implicit representation heap的结构特性，新值必须置于底部容器的最尾端，即第一个洞号(last-first)-1
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
    }

    //以下这组push_back() 不允许指定 "大小比较的标准"
    
    template <class RandomAccessIterator, class Distance, class T>
    void __push_heap(RandomAccessIterator first, Distance holeIndex,
        Distance topIndex, T value) 
    {
        Distance parent = (holeIndex - 1) / 2; //找出父节点
        //当holeIndex并没有到达顶峰的同时 父节点还小于新值 （不符合heap的次序特性）
        //使用operator < 进行元素的比较，因此STL heap是一种max-heap
        //父节点是动态变化的
        //每次是和value进行比较，因此不需要进行父子元素值的对调，直接让子元素接管父亲的数值
        while (holeIndex > topIndex && *(first + parent) < value) {
            *(first + holeIndex) = *(first + parent); //令洞值为父值,也就是孩子赋予了父亲的数值，父亲位置
            holeIndex = parent; //调整洞号，向上提升至父节点
            parent = (holeIndex - 1) / 2;//新洞的父节点
        }//持续至顶端，或者满足 heap的次序特性为止
        *(first + holeIndex) = value;//令新洞为新的数值，完成插入操作
    }

    //pop_heap   执行一个下溯操作  push_heap和pop_heap都有一个表示洞口的参数
    //这个堆没有指定 大小比较的标准
    template <class RandomAccessIterator, class T, class Distance>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
        RandomAccessIterator result, T value, Distance*) {
        *result = *first; //设定尾值为首值，也就是此刻的尾值为先前的根节点
        //可以通过底层容器的 pop_back()取出尾值
        //重新调整 heap 洞号为0(即树根处)，将其数值设定为 value(先前的尾节点的数值)
        __adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    //这个堆没有指定 大小比较的标准
    template <class RandomAccessIterator, class Distance, class T>
    void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
        Distance len, T value) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2;//洞节点的右子节点
        while (secondChild < len) 
        {
            //比较洞节点的左右两个孩子的数值，然后以secondChild代表较大的子节点
            if (*(first + secondChild) < *(first + secondChild - 1))  
            {
                secondChild--;
            }
            //令较大的子值为洞值 再令洞号下降至较大的子节点处
            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;        //这两行代码像迭代
            //找出新洞节点的右子节点
            secondChild = 2 * (secondChild + 1);
        }
        if (secondChild == len) 
        {
            //没有右边节点 只有左边的节点
            //将左子值设为洞值 再令洞号下移至 左子节点处
            *(first + holeIndex) = *(first + secondChild - 1);
            holeIndex = secondChild - 1;
        }
        *(first + holeIndex) = value;   //源代码其实是在这里执行了push_heap操作，但是没必要，因为已经是次序了
    }

    //sort_heap，通过调用pop_heap
    //最后得到的序列就是递增序列，但是此时原来的heap就不是一个真正意义的heap了
    template <class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) 
    {
        //每执行一次pop_heap(),极值(在STL heap中为极大值)就会被放在尾端
        //扣除尾端再次执行pop_heap()，次极值又被放在新的尾端，按照上述流程一直执行，最后得到排序结果
        while (last - first > 1) 
        {
            pop_heap(first, last--);//每次执行pop_heap()一次，操作范围即缩减一格
        }
    }

    //make_heap函数
    //实现思路就是先找到最后一个父节点的下标  即 (len - 2) / 2
    //然后把这个下标作为洞口，传入adjust_heap函数作为参数，然后递减这个下标，直到为根节点
    //这就是make_heap的实现！
    template <class RandomAccessIterator, class T, class Distance>
    void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
    {
        if (last - first < 2)
        { //如果长度为0或者为1，不需要重新排列
            return;
        }
        Distance len = last - first;
        //找出第一个需要重新排列的子树的头部，以parent标定
        //考虑到任何节点不需要执行perlocate down，因此使用hole Index进行命名更好
        Distance parent = (len - 2) / 2;   //注意可以通过模拟，知道(len-2)/2的值为最后一个父节点
        while (true) {
            //重新排列以parent为首的子树
            //len的目的是为了让__adjust_heap() 判断操作的范围
            __adjust_heap(first, parent, len, T(*(first + parent)));
            if (parent == 0) {
                return; //走完根节点 结束
            }
            parent--; // (即将重拍子树的)头部向前一个节点
        }
    }
}
#endif
