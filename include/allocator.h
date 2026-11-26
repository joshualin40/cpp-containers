#include <cstddef>
#include <exception> 
#include <new> 
/*
To implement a modern, C++11/17/20-compliant version of std::allocator, you only need to define value_type,
constructors, allocate, deallocate, and equality operators, because std::allocator_traits automatically provides 
all other boilerplate methods.
*/

namespace ft {

    template <typename T> 
    class Allocator {
        public: 
            // allocate n objects 
            constexpr T* allocate(std::size_t n) {
                size_t bytes = n * sizeof(T); 

                // if (n > std::numeric_limits<size_t>::max() / sizeof(T)) 
                //     throw std::length_error("Capacity overflow");
                
                T* res = static_cast<T*>(::operator new (bytes)); 
                
                return res; 
            }
            
            // void operator delete(void* p, std::size_t n) noexcept deallocates n bytes of storage at pointer p.
            // only frees member, does not destroy objects
            void deallocate(T* p, std::size_t n) {
                size_t bytes = n * sizeof(T); 
                ::operator delete(p, bytes);
            }
            // Constructs an object in allocated storage.
            void construct(T* p, const T& v) { new((void*)p)T(v); }

            void construct(T* p, T&& v) { new((void*)p)T(std::move(v)); }

            // Destroy an object in allocated storage.
            void destroy(T* p) { p->~T(); }
            
    }; 
}
