#ifndef _YHSTL_CONSTRUCT_H_
#define _YHSTL_CONSTRUCT_H_

// 20220922���ͷ�ļ������������� construct��destroy
// construct : �������Ĺ���
// destroy   : ������������

#include <new>

#include "type_traits.h"
#include "iterator.h"

namespace yhstl
{

    // construct �������

    template <class Ty>
    void construct(Ty* ptr)
    {
        ::new ((void*)ptr) Ty();
    }

    template <class Ty1, class Ty2>
    void construct(Ty1* ptr, const Ty2& value)
    {
        ::new ((void*)ptr) Ty1(value);
    }

    template <class Ty, class... Args>
    void construct(Ty* ptr, Args&&... args)
    {
        ::new ((void*)ptr) Ty(yhstl::forward<Args>(args)...);
    }

    // destroy ����������

    template <class Ty>
    void destroy_one(Ty*, std::true_type) {}

    template <class Ty>
    void destroy_one(Ty* pointer, std::false_type)
    {
        if (pointer != nullptr)
        {
            pointer->~Ty();
        }
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }

    template <class Ty>
    void destroy(Ty* pointer)
    {
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, std::is_trivially_destructible<
            typename iterator_traits<ForwardIter>::value_type>{});
    }

} // namespace yhstl

#endif // !MYTINYSTL_CONSTRUCT_H_


