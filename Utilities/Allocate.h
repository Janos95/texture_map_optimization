//
// Created by janos on 10/10/20.
//

#pragma once

#include <new>

namespace TextureMapOptimization {

inline void* allocateBuffer(size_t Size, size_t Alignment) {
    return ::operator new(Size
#ifdef __cpp_aligned_new
            ,
                          std::align_val_t(Alignment)
#endif
    );
}


inline void deallocateBuffer(void* Ptr, size_t Size, size_t Alignment) {
    ::operator delete(Ptr
#ifdef __cpp_sized_deallocation
            ,
                      Size
#endif
#ifdef __cpp_aligned_new
            ,
                      std::align_val_t(Alignment)
#endif
    );
}



}

