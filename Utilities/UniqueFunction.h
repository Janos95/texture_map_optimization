#pragma once

#include "Allocate.h"

#include <utility>

namespace TextureMapOptimization {

template<typename FunctionT>
class UniqueFunction;

template<typename Ret, typename ...Params>
class UniqueFunction<Ret(Params...)> {
public:

    UniqueFunction() = default;

    UniqueFunction(std::nullptr_t) {}

    template<class Callable>
    UniqueFunction(Callable&& f) {

        erased = allocateBuffer(sizeof(Callable), alignof(Callable));
        ::new(erased) Callable((Callable&&) f);

        destroy = +[](void* e) {
            static_cast<Callable*>(e)->~Callable();
            deallocateBuffer(e, sizeof(Callable), alignof(Callable));
        };

        call = +[](void* e, Params... params) -> Ret {
            return (*static_cast<Callable*>(e))(params...);
        };
    }

    UniqueFunction& operator=(UniqueFunction&& other) noexcept {
        other.swap(*this);
        return *this;
    }

    UniqueFunction(UniqueFunction&& other) noexcept {
        other.swap(*this);
    }

    void swap(UniqueFunction& other) {
        std::swap(erased, other.erased);
        std::swap(destroy, other.destroy);
        std::swap(call, other.call);
    }

    Ret operator()(Params ...params) const {
        return call(erased, (Params&&) (params)...);
    }

    explicit operator bool() const { return erased; }

    ~UniqueFunction() {
        if(destroy) destroy(erased);
    }

private:

    void* erased = nullptr;
    void (* destroy)(void*) = nullptr;
    Ret (* call)(void*, Params...) = nullptr;
};

}
