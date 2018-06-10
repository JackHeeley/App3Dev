//
// mmf_vector.hpp : implements a vector type with data backed by a memory mapped file.
//
// Memory mapped files are named shared objects which are efficiently backed to disk. 
// The vector is linked to the file space by use of a custom allocator (supplied here).
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//
#pragma once
#undef max           // remove the default macro

#include <limits>
#include <iostream>
#include <vector>

///<summary> specialize a type for vector.</summary>
namespace is_stl_vector_impl 
{
   template <typename T> 
   struct is_stl_vector : std::false_type {};
   
   template <typename... Args> 
   struct is_stl_vector<std::vector<Args...>> : std::true_type {};
   
   template <> 
   struct is_stl_vector<std::_Container_proxy> : std::true_type {};
}

///<summary> type trait to utilize the implementation type traits as well as decay the type.</summary>
template <typename T> 
struct is_stl_vector 
{
   static constexpr bool const value = is_stl_vector_impl::is_stl_vector<std::decay_t<T>>::value;
};

///<summary> provide the neccessary custom allocator.</summary>
namespace CustomAllocator
{

   template <class T>
   class mmf_allocator {
   public:
      // type definitions
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;

      ///<summary> rebind allocator to type U.<summary>
      template <class U>
      struct rebind 
      {
         typedef mmf_allocator<U> other;
      };

      ///<summary> return address of values.</summary>
      pointer address(reference value) const 
      {
         return &value;
      }
      
      const_pointer address(const_reference value) const 
      {
         return &value;
      }

      ///<summary> default constructor.</summary>
      ///<remarks> nothing to do because the allocator has no state.</remarks>
      mmf_allocator() noexcept
      {
      }
      
      ///<summary> copy constructor.</summary>
      ///<remarks> nothing to do because the allocator has no state.</remarks>
      mmf_allocator(const mmf_allocator&) noexcept
      {
      }
      
      ///<summary> generalized copy constructor.</summary>
      ///<remarks> nothing to do because the allocator has no state.</remarks>
      template <class U>
      mmf_allocator(const mmf_allocator<U>&) noexcept 
      {
      }

      ///<summary> destructor.</summary>
      ///<remarks> nothing to do because the allocator has no state.</remarks>
      ~mmf_allocator() noexcept
      {
      }

      ///<summary> return maximum number of elements that can be allocated.</summary>
      size_type max_size() const noexcept
      {
         return std::numeric_limits<std::size_t>::max() / sizeof(T);
      }

      ///<summary> allocate but don't initialize num elements of type T.</summary>
      pointer allocate(size_type num, const void* = 0) 
      {
         if (is_stl_vector<T>::value) 
         {
            // allocate the vector *container* as per default allocator
            return static_cast<pointer>(::operator new(num * sizeof(T)));
         }
         else 
         {
            // allocate the vector *content* from the memory mapped file space
            return static_cast<pointer>(mmf_malloc(num * sizeof(T)));
         }
      }

      ///<summary> initialize elements of allocated storage p with value value.</summary>
      void construct(pointer p, const T& value) noexcept
      {
         // initialize memory with placement new
         //new(static_cast<void*>(p))T(value);
         new(p)T(value);
      }

      ///<summary> destroy elements of initialized storage p.</summary>
      void destroy(pointer p) noexcept
      {
         // destroy objects by calling their destructor
         p->~T();
      }

      ///<summary> deallocate storage p of deleted elements.</summary>
      void deallocate(pointer p, size_type num) noexcept
      {
         if (is_stl_vector<T>::value) 
         {
            // deallocate the vector *container* as per default allocator
            ::operator delete(p);
         }
         else 
         {
            // deallocate the vector *content* from the memory mapped file space
            mmf_free(p);
         }
      }
   };

   ///<summary> return that all specializations of this allocator are interchangeable.</summary>
   template <class T1, class T2>
   bool operator== (const mmf_allocator<T1>&, const mmf_allocator<T2>&) throw() 
   {
      return true;
   }

   template <class T1, class T2>
   bool operator!= (const mmf_allocator<T1>&, const mmf_allocator<T2>&) throw() 
   {
      return false;
   }
}

///<summary> alias for a byte vector with a custom allocator backed by a memory mapped file.<summary>
using mmf_vector = std::vector<unsigned char, CustomAllocator::mmf_allocator<unsigned char>>;