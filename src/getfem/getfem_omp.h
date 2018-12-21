/* -*- c++ -*- (enables emacs c++ mode) */
/*===========================================================================

 Copyright (C) 2012-2017 Andriy Andreykiv

 This file is a part of GetFEM++

 GetFEM++  is  free software;  you  can  redistribute  it  and/or modify it
 under  the  terms  of the  GNU  Lesser General Public License as published
 by  the  Free Software Foundation;  either version 3 of the License,  or
 (at your option) any later version along with the GCC Runtime Library
 Exception either version 3.1 or (at your option) any later version.
 This program  is  distributed  in  the  hope  that it will be useful,  but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License and GCC Runtime Library Exception for more details.
 You  should  have received a copy of the GNU Lesser General Public License
 along  with  this program;  if not, write to the Free Software Foundation,
 Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.

 As a special exception, you  may use  this file  as it is a part of a free
 software  library  without  restriction.  Specifically,  if   other  files
 instantiate  templates  or  use macros or inline functions from this file,
 or  you compile this  file  and  link  it  with other files  to produce an
 executable, this file  does  not  by itself cause the resulting executable
 to be covered  by the GNU Lesser General Public License.  This   exception
 does not  however  invalidate  any  other  reasons why the executable file
 might be covered by the GNU Lesser General Public License.

===========================================================================*/

/**@file getfem_omp.h
@author  Andriy Andreykiv <andriy.andreykiv@gmail.com>
@date May 14th, 2013.
@brief Tools for multithreaded, OpenMP and Boost based parallelization.

This is the kernel of getfem.
*/
#pragma once

#include <atomic>
#include <memory>
#include <set>
#include <vector>

#include "bgeot_config.h"

#ifdef GETFEM_HAS_OPENMP
  #include <mutex>
  #include <boost/thread.hpp> /**TODO: get rid of this dependency as soon
                                       as thread_local is widely supported*/
#endif

namespace getfem
{
  using bgeot::size_type;

#ifdef GETFEM_HAS_OPENMP
  //declaring a thread lock, to protect multi-threaded accesses to
  //asserts, traces and warnings. Using a global mutex
  class omp_guard: public std::lock_guard<std::recursive_mutex>
  {
  public:
    omp_guard();

  private:
    static std::recursive_mutex mutex_;
  };

  //like std::lock_guard, but copyable
  class local_guard
  {
  public:
    local_guard(std::recursive_mutex&);

  private:
    std::recursive_mutex& mutex_;
    std::shared_ptr<std::lock_guard<std::recursive_mutex>> plock_;
  };

  //produces scoped lock on the
  //mutex, held in this class
  class lock_factory
  {
  public:

    //get a lock object with RAII acquire/release semantics
    //on the mutex from this factory
    local_guard get_lock() const;
  private:
    mutable std::recursive_mutex mutex_;
  };


#else

  class omp_guard{};
  class local_guard{};
  struct lock_factory
  {
    inline local_guard get_lock() const {return local_guard();}
  };
#endif

  /**set maximum number of OpenMP threads*/
  void set_num_threads(int n);

  /**is the program running in the parallel section*/
  bool me_is_multithreaded_now();

  /** is the program is running on a single thread*/
  bool not_multithreaded();

  /**Maximum number of threads that can run concurrently*/
  size_type max_concurrency();

  /**Thread policy, where partitioning is based on true threads*/
  struct true_thread_policy{
    static size_type this_thread();
    static size_type num_threads();
  };

  /** Thread policy, regulated by partition_master
     (can be true thread- or partition-based)*/
  struct global_thread_policy{
    static size_type this_thread();
    static size_type num_threads();
  };

  //implementation classes for omp_distribute
  namespace detail{

    struct general_tag{};
    struct vector_tag{};
    struct bool_tag{};

    template<typename T>
    struct distribute_traits
    {
      using type = general_tag;
    };

    template<typename T>
    struct distribute_traits<std::vector<T>>
    {
      using type = vector_tag;
    };

    template<>
    struct distribute_traits<bool>
    {
      using type = bool_tag;
    };

    template<typename T, typename thread_policy, typename tag>
    class omp_distribute_impl;

    template <typename T, typename thread_policy>
    class omp_distribute_impl<T, thread_policy, general_tag> {
    private:
      std::vector<T> thread_values;
      friend struct all_values_proxy;

      struct all_values_proxy{
        omp_distribute_impl& distro;
        all_values_proxy(omp_distribute_impl& d)
          : distro(d)
        {}

        void operator = (const T& x){
          for(auto it  = distro.thread_values.begin();
                   it != distro.thread_values.end(); ++it){
            *it=x;
          }
        }
      };

    public:

      template <class... args>
       explicit omp_distribute_impl(args&&... value){
        thread_values.reserve(thread_policy::num_threads());
        for (size_type i = 0; i != thread_policy::num_threads(); ++i){
          thread_values.emplace_back(value...);
        }
      }

      operator T& (){
        return thread_values[thread_policy::this_thread()];
      }

      operator const T& () const {
        return thread_values[thread_policy::this_thread()];
      }

      T& thrd_cast(){
        return thread_values[thread_policy::this_thread()];
      }

      const T& thrd_cast() const {
        return thread_values[thread_policy::this_thread()];
      }

      T& operator()(size_type i) {
        return thread_values[i];
      }

      void on_thread_update() {
        std::unique_ptr<omp_guard> p_guard = nullptr;
        if (me_is_multithreaded_now()) p_guard = std::make_unique<omp_guard>();
        if (thread_values.size() != thread_policy::num_threads()) {
          thread_values.resize(thread_policy::num_threads());
        }
      }

      size_type num_threads() const {
        return thread_policy::num_threads();
      }

      size_type this_thread() const {
        return thread_policy::this_thread();
      }

      const T& operator()(size_type i) const {
        return thread_values[i];
      }

      T& operator = (const T& x){
        if (me_is_multithreaded_now()){
          thread_values[thread_policy::this_thread()] = x;
        }
        else all_threads() = x;

        return thread_values[thread_policy::this_thread()];
      }

      all_values_proxy all_threads(){
        return all_values_proxy(*this);
      }
    };

    /**Specialization for std::vector<T>, adds vector indexing operator*/
    template <typename T,
              typename thread_policy>
    class omp_distribute_impl<std::vector<T>, thread_policy, vector_tag>
      : public omp_distribute_impl<std::vector<T>, thread_policy, general_tag>
    {
    public:
      using base = omp_distribute_impl<std::vector<T>, thread_policy, general_tag>;

      template <class... args>
      explicit omp_distribute_impl(args&&... value)
        : base(value...)
      {}

      T& operator[](size_type i){
        return base::thrd_cast()[i];
      }
      const T& operator[](size_type i) const{
        return base::thrd_cast()[i];
      }

      std::vector<T>& operator = (const std::vector<T>& x){
        return base::operator=(x);
      }
    };

    /**Specialization for bool, to circumvent the shortcomings
    of standards library's specialization for std::vector<bool>,
    we use std::vector<int> instead*/
    template <typename thread_policy>
    class omp_distribute_impl<bool, thread_policy, bool_tag>
      : public omp_distribute_impl<int, thread_policy, general_tag>
    {
    public:
      using base = omp_distribute_impl<int, thread_policy, general_tag>;

      template <class... Args>
      explicit omp_distribute_impl(Args&&... value)
        : base(value...)
      {}

      operator bool () const {
        return base::operator const int&();
      }

      bool operator = (const bool& x){
        return base::operator=(x);
      }
    };

  } /* end of namespace detail.                                             */

  template<typename T, typename thread_policy>
  using od_base = typename detail::omp_distribute_impl<
                    T, thread_policy, detail::distribute_traits<T>::type>;

  /**
    Use this template class for any object you want to
    distribute to open_MP threads. The creation of this
    object should happen in serial, while accessing the individual
    thread local instances will take place in parallel.
    Use thread_policy to either distribute the objects between physical
    threads or a fixed number of partitions, independent of the number
    of threads. If you change the default policy, remember to also
    use this_thread() and num_threads() from the corresponding policy
    for iterating over the thread-specific components.
  */
  template<typename T,
  typename thread_policy = global_thread_policy>
  class omp_distribute : public od_base<T, thread_policy>
  {
  public:
    using base = od_base<T, thread_policy>;

    template <class... args>
    explicit omp_distribute(args&&... value)
      : base(value...)
    {}

    auto operator = (const T& x) -> decltype(std::declval<base>() = x){
      return base::operator=(x);
    }
  };

  /* Use these macros only in function local context to achieve
  the effect of thread local storage for any type of objects
  and their initialization (it's more general and portable
  then using __declspec(thread))*/
  #ifdef GETFEM_HAS_OPENMP

    #define DEFINE_STATIC_THREAD_LOCAL_INITIALIZED(Type,Var,initial) \
      static boost::thread_specific_ptr<Type> ptr_##Var; \
      if(!ptr_##Var.get()) {ptr_##Var.reset(new Type(initial));} \
      Type& Var=*ptr_##Var;

    #define DEFINE_STATIC_THREAD_LOCAL(Type,Var) \
      static boost::thread_specific_ptr<Type> ptr_##Var; \
      if(!ptr_##Var.get()) {ptr_##Var.reset(new Type());} \
      Type& Var=*ptr_##Var;

    #define DEFINE_STATIC_THREAD_LOCAL_CONSTRUCTED(Type, Var, ...) \
      static boost::thread_specific_ptr<Type> ptr_##Var; \
      if(!ptr_##Var.get()) {ptr_##Var.reset(new Type(__VA_ARGS__));} \
      Type& Var=*ptr_##Var;

  #else

    #define DEFINE_STATIC_THREAD_LOCAL_INITIALIZED(Type,Var,initial) \
      static Type Var(initial);

    #define DEFINE_STATIC_THREAD_LOCAL(Type,Var) \
      static Type Var;

    #define DEFINE_STATIC_THREAD_LOCAL_CONSTRUCTED(Type, Var, ...) \
      static Type Var(__VA_ARGS__);

  #endif

  class partition_master;

  /**Iterator that runs over partitions on the current
     thread and sets the global (but thread-specific)
     partition during incrementation*/
  class partition_iterator
  {
  public:

    partition_iterator operator ++();
    bool operator==(const partition_iterator&) const;
    bool operator!=(const partition_iterator&) const;
    size_type operator*() const;

  private:

    friend class partition_master;

    /**Only partition_master can create one*/
    partition_iterator(partition_master &master,
                       std::set<size_type>::const_iterator it);

    partition_master &master;
    std::set<size_type>::const_iterator it;
  };

  enum class thread_behaviour {true_threads, partition_threads};

  /**
    A singleton that Manages partitions on individual threads.
  */
  class partition_master
  {
  public:

    static partition_master &get();

    /**beginning of the partitions for the current thread*/
    partition_iterator begin();

    /**end of the partitions for the current thread*/
    partition_iterator end();

    /**Sets the behaviour for the full program: either partitioning parallel loops
       according to the number of true threads, specified by the user,
       or to the number of the fixed partitions equal to the max concurrency of the system.
       The later makes the partitioning independent of the number of the threads set*/
    void set_behaviour(thread_behaviour);

    /**active partition on the thread. If number of threads is equal to the
    max concurrency of the system, then it's also the index of the actual thread*/
    size_type get_current_partition() const;

    /**number of partitions or threads, depending on thread policy*/
    size_type get_nb_partitions() const;

    /**for thread_behaviour::partition_threads set the total number of partitions.
      This call must be made before all the omp_distribute based classes are created.
      Otherwise they become invalid*/
    void set_nb_partitions(size_type);

    void check_threads();

  private:

    void rewind_partitions();

    //Parallel execution of a lambda. Please use the macros below
    friend void parallel_execution(std::function<void(void)> lambda, bool iterate_over_partitions);

    /**set current partition, which will be also returned in this_thread() call*/
    void set_current_partition(size_type);

    friend partition_iterator;

    partition_master();

    void update_partitions();

    omp_distribute<std::set<size_type>, true_thread_policy> partitions;
    omp_distribute<size_type, true_thread_policy> current_partition;
    std::atomic<size_type> nb_user_threads;
    thread_behaviour behaviour = thread_behaviour::partition_threads;
    std::atomic<bool> partitions_updated = false;
    size_type nb_partitions;

    static partition_master instance;
  };

  class standard_locale;
  class thread_exception;

  /**Encapsulates open_mp-related initialization and de-initialization*/
  class parallel_boilerplate
  {
    std::unique_ptr<standard_locale> plocale;
    std::unique_ptr<thread_exception> pexception;

  public:
    parallel_boilerplate();
    void run_lamda(std::function<void(void)> lambda);
    ~parallel_boilerplate();
  };

  #ifdef __GNUC__
    #define pragma_op(arg) _Pragma("arg")
  #else
    #define pragma_op(arg) __pragma(arg)
  #endif

  /**
   Organizes a proper parallel omp section:
   - iteration on thread independent partitions
   - passing exceptions to the master thread
   - thread-safe locale
   */
  #ifdef GETFEM_HAS_OPENMP
    #define GETFEM_OMP_PARALLEL(body) getfem::parallel_execution([&](){body;}, true);

    /**execute in parallel, but do not iterate over partitions*/
    #define GETFEM_OMP_PARALLEL_NO_PARTITION(body) getfem::parallel_execution([&](){body;}, false);

    /**execute for loop in parallel. Not iterating over partitions*/
    #define GETFEM_OMP_FOR(init, check, increment, body) {\
      auto boilerplate = getfem::parallel_boilerplate{};  \
      pragma_op(omp parallel for)                         \
      for (init; check; increment){                       \
        boilerplate.run_lamda([&](){body;});              \
      }                                                   \
    }

  #else
    #define GETFEM_OMP_PARALLEL(body) body
    #define GETFEM_OMP_PARALLEL_NO_PARTITION(body) body;
    #define GETFEM_OMP_FOR(init, check, increment, body)\
      for (init; check; increment) {                    \
        body                                            \
      }

  #endif

}  /* end of namespace getfem.                                             */