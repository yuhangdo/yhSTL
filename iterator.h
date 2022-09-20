#ifndef _YHSTL_ITERATOR_H_
#define _YHSTL_ITERATOR_H_
#include<cstddef>      //加入这个头文件，主要是用到   _WIN64 typedef __int64          ptrdiff_t;这些数据类型
#include "type_traits.h"   //后续要实现 type_traits.h

//STL中心思想就是容器和算法分开，独立设计，而迭代器则是两者的粘合剂
//20220921 迭代器是一种智能指针，将大致类内容完善
namespace yhstl
{   
    //五种内嵌型别标记
	struct input_iterator_tag {};      
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// iterator 模板  任何新定义的迭代器最好继承于这个，由于后三个参数都有默认值，所以只要定义前两个参数就行
	template <class Category, class T, class Distance = ptrdiff_t,class Pointer = T*, class Reference = T&>
	struct iterator
	{
		typedef Category                             iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
	};

    // iterator traits
    //榨汁机  traits
    template <class Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };

    // 针对原生指针的偏特化版本
    template <class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag           iterator_category;
        typedef T                                    value_type;
        typedef T*                                   pointer;
        typedef T&                                   reference;
        typedef ptrdiff_t                            difference_type;
    };
    //针对pointer-to-const 设计的traits偏特化版
    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag           iterator_category;
        typedef T                                    value_type;
        typedef const T*                             pointer;
        typedef const T&                             reference;
        typedef ptrdiff_t                            difference_type;
    };


    // 萃取某个迭代器的 category
    template <class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&)
    {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    // 萃取某个迭代器的 distance_type
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 萃取某个迭代器的 value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // 以下函数用于计算迭代器间的距离
    //这个很奇妙 通过重载实现 advance函数
    // distance 的 input_iterator_tag 的版本
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++first;
            ++n;
        }
        return n;
    }

    // distance 的 random_access_iterator_tag 的版本
    template <class RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last,
            random_access_iterator_tag)
    {
        return last - first;
    }

    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
    {
        return distance_dispatch(first, last, iterator_category(first));
    }

    // 以下函数用于让迭代器前进 n 个距离

    // advance 的 input_iterator_tag 的版本
    template <class InputIterator, class Distance>
    void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
    {
        while (n--)
            ++i;
    }

    // advance 的 bidirectional_iterator_tag 的版本
    template <class BidirectionalIterator, class Distance>
    void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
    {
        if (n >= 0)
            while (n--)  ++i;
        else
            while (n++)  --i;
    }

    // advance 的 random_access_iterator_tag 的版本
    template <class RandomIter, class Distance>
    void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }

    template <class InputIterator, class Distance>
    void advance(InputIterator& i, Distance n)
    {
        advance_dispatch(i, n, iterator_category(i));
    }

    // 模板类 : reverse_iterator
    // 代表反向迭代器，使前进为后退，后退为前进
    template <class Iterator>
    class reverse_iterator
    {
    private:
        Iterator current;  // 记录对应的正向迭代器

    public:
        // 反向迭代器的五种相应型别
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type        value_type;
        typedef typename iterator_traits<Iterator>::difference_type   difference_type;
        typedef typename iterator_traits<Iterator>::pointer           pointer;
        typedef typename iterator_traits<Iterator>::reference         reference;

        typedef Iterator                                              iterator_type;
        typedef reverse_iterator<Iterator>                            self;

    public:
        // 构造函数
        reverse_iterator() {}
        explicit reverse_iterator(iterator_type i) :current(i) {}
        reverse_iterator(const self& rhs) :current(rhs.current) {}

    public:
        // 取出对应的正向迭代器
        iterator_type base() const
        {
            return current;
        }

        // 重载操作符
        reference operator*() const
        { // 实际对应正向迭代器的前一个位置
            auto tmp = current;
            return *--tmp;
        }
        pointer operator->() const
        {
            return &(operator*());
        }

        // 前进(++)变为后退(--)
        self& operator++()
        {
            --current;
            return *this;
        }
        self operator++(int)
        {
            self tmp = *this;
            --current;
            return tmp;
        }
        // 后退(--)变为前进(++)
        self& operator--()
        {
            ++current;
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }
        self operator+(difference_type n) const
        {
            return self(current - n);
        }
        self& operator-=(difference_type n)
        {
            current += n;
            return *this;
        }
        self operator-(difference_type n) const
        {
            return self(current + n);
        }

        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }
    };

    // 重载 operator-
    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
        operator-(const reverse_iterator<Iterator>& lhs,
            const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() - lhs.base();
    }

    // 重载比较操作符
    template <class Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() < lhs.base();
    }

    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return rhs < lhs;
    }

    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs,
        const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs < rhs);
    }
}
#endif
