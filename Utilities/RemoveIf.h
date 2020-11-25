//
// Created by janos on 9/17/20.
//

#pragma once

#include <utility>

namespace TextureMapOptimization {

template<class InputIt, class UnaryPredicate>
constexpr InputIt findIf(InputIt first, InputIt last, UnaryPredicate p) {
    for(; first != last; ++first) {
        if(p(*first)) {
            return first;
        }
    }
    return last;
}

template<class ForwardIt, class UnaryPredicate>
ForwardIt removeIf(ForwardIt first, ForwardIt last, UnaryPredicate p)
{
    first = findIf(first, last, p);
    if (first != last)
        for(ForwardIt i = first; ++i != last; )
            if (!p(*i))
                *first++ = std::move(*i);
    return first;
}

}
