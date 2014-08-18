#ifndef assoc_list_h__
#define assoc_list_h__

//BOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG

//std::initializer_list<value_type> in constructor, in assigment, in insert and some other
//unordered_map methods

// #include <boost/config.hpp>
// #include <memory>
#include <functional>
#include <boost/functional/hash_fwd.hpp>

#include <boost/unordered_set.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/intrusive/parent_from_member.hpp>
#include <utility>

namespace detail
{
  /*template<class FromIterator, class ToIterator, class ConstructorConvertorFunctor>
  class iterator_constructable_from: public ToIterator
  {
  public:
    iterator_constructable_from();
    explicit iterator_constructable_from( const FromIterator& x );
    explicit iterator_constructable_from( const ToIterator& x );
    iterator_constructable_from( const iterator_constructable_from& other );
    template< class U, class V, class W > 
    iterator_constructable_from( const iterator_constructable_from<U, V, W>& other );
    iterator_constructable_from& operator=( const iterator_constructable_from& other );
    template< class U, class V, class W> 
    iterator_constructable_from& operator=( const iterator_constructable_from<U, V, W>& other );
    iterator_constructable_from& operator=( const ToIterator& other );
  };*/

  class not_change_if_exist;
  class move_if_exist;

  template <
    class K,
    class T,
    class H = ::boost::hash<K>,
    class P = ::std::equal_to<K>,
    class A = ::std::allocator<::std::pair<const K, T> >
    class I = ::boost::not_change_if_exist>
  class map_list
  {
  public:

    typedef K key_type;
    typedef ::std::pair<const K, T> value_type;
    typedef T mapped_type;
    typedef H hasher;
    typedef P key_equal;
    typedef A allocator_type; 
    typedef I insert_mode;

    //...

    typedef value_type& reference;
    typedef value_type const& const_reference;
    typedef value_type* pointer;
    typedef value_type const* const_pointer;
    /*typedef value_type* const pointer_const;
    typedef value_type const* const const_pointer_const;*/

    //...
  private:
    friend class insert_mode;
    BOOST_COPYABLE_AND_MOVABLE(map_list)
    typedef ::boost::intrusive::list_base_hook<
      ::boost::intrusive::constant_time_size<false>, 
      ::boost::intrusive::size_type<size_type>
      ::boost::intrusive::link_mode<boost::intrusive::safe_link>
    > item_base;
    struct item_type: public item_base
    {
      value_type value;
      map_list* index_iterator_getter; € везде забыл заполнить этот член.
    };
    typedef ::boost::intrusive::list<item_base> list_type;
    struct hasher_internal: public std::unary_function<item_type, std::size_t>
    {
      ::std::size_t operator()(item_type const& toKey) const
      {
        return hasher()(toKey.value.first);
      }
    };
    struct key_equal_internal: public std::binary_function<item_type, item_type, bool>
    {
      bool operator()(item_type const& leftToKey, item_type const& rightToKey) const
      {
        return key_equal()(leftToKey.value.first, rightToKey.value.first);
      }
    };
    typedef typename A:: template rebind <item_type> ::other allocator_type_internal;
    //typedef ::boost::container::allocator_traits<allocator_type> allocator_traits;
    //typedef ::boost::container::allocator_traits<allocator_type_internal> allocator_traits_internal;
    typedef ::boost::unordered_set<item_type, hasher_internal, key_equal_internal, allocator_type_internal> index_type;
    struct iterator_transformer
    {
      typedef value_type result_type;
      const result_type& operator()(const item_type& item) const
      {
        return item.value;
      }

      result_type& operator()(item_type& item) const
      {
        return item.value;
      }
    };
    
    list_type list;
    index_type index;

    //::boost::iterator_to

    static iterator const_iterator_to_iterator(const_iterator it)
    {
      return iterator(::boost::iterator_to(const_cast<item_type*>(&*it.base())));
    }

    static typename index_type::const_iterator iterator_to_index_iterator(const_iterator it)
    {
      const item_type* const item = ::boost::intrusive::get_parent_from_member(&*it, &item_type::value);
      return item->index_iterator_getter.find(item.first);
    }

    static typename index_type::iterator iterator_to_index_iterator(iterator it)
    {
      const item_type* const item = ::boost::intrusive::get_parent_from_member(&*it, &item_type::value);
      return item->index_iterator_getter.find(item.first);
    }

    template<class Pred>
    class ValueCompareToNodeCompare: public ::std::unary_function<item_type, bool>, public ::std::binary_function<item_type, item_type, bool>
    {
      const typename ::boost::unwrap_reference<Pred>::type pred;
    public:
      ValueCompareToNodeCompare(Pred pred):pred(::boost::unwrap_ref(pred)){}
      bool operator()(const item_type& item) const
      {
        return pred(item.value);
      }
      bool operator()(const item_type& item, const item_type& item2) const
      {
        return pred(item.value, item2.value);
      }
    };

  public:
    typedef ::boost::transform_iterator<iterator_transformer, typename list_type::iterator> iterator;
    typedef ::boost::transform_iterator<iterator_transformer, typename list_type::const_iterator> const_iterator;
    typedef ::std::reverse_iterator<iterator> reverse_iterator;
    typedef ::std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef const_iterator const_local_iterator;
    typedef iterator local_iterator;

    typedef typename index::size_type size_type;
    typedef typename index::difference_type difference_type;

    //////////////////////////////////////////////
    //
    //          construct/copy/destroy
    //
    //////////////////////////////////////////////

    //! <b>Effects</b>: Default constructs a map_list.
    //!
    //! <b>Throws</b>: If allocator_type's default constructor throws.
    //!
    //! <b>Complexity</b>: Constant.
    map_list()
    {}

    //! <b>Effects</b>: Constructs a map_list taking the allocator as parameter.
    //!
    //! <b>Throws</b>: Nothing
    //!
    //! <b>Complexity</b>: Constant.
    придетс€ заводить свой алокатор дл€ index. scoped_ptr не подходит, тк не умеет в алокаторы
    explicit map_list(const allocator_type &a) ?BOOST_CONTAINER_NOEXCEPT?
      : index(a)
    {}
  

    //! <b>Effects</b>: Copy constructs a list.
    //!
    //! <b>Postcondition</b>: x == *this.
    //!
    //! <b>Throws</b>: If allocator_type's default constructor or copy constructor throws.
    //!
    //! <b>Complexity</b>: Linear to the elements x contains.
    map_list(const map_list& x)
      : index(x.get_allocator())
    {  (void)this->insert(this->cbegin(), x.begin(), x.end());   }

    //! <b>Effects</b>: Move constructor. Moves mx's resources to *this.
    //!
    //! <b>Throws</b>: If allocator_type's copy constructor throws.
    //!
    //! <b>Complexity</b>: Constant.
    map_list(BOOST_RV_REF(map_list) x)
      : index(::boost::move(x.index)),
        list(::boost::move(x.list))
    {}

    //! <b>Effects</b>: Copy constructs a map_list using the specified allocator.
    //!
    //! <b>Postcondition</b>: x == *this.
    //!
    //! <b>Throws</b>: If allocator_type's default constructor or copy constructor throws.
    //!
    //! <b>Complexity</b>: Linear to the elements x contains.
    map_list(const map_list& x, const allocator_type &a)
      : index(allocator_type_internal(a))
    {  (void)this->insert(this->cbegin(), x.begin(), x.end());   }

    //! <b>Effects</b>: Move constructor sing the specified allocator.
    //!                 Moves mx's resources to *this.
    //!
    //! <b>Throws</b>: If allocation or value_type's copy constructor throws.
    //!
    //! <b>Complexity</b>: Constant if a == x.get_allocator(), linear otherwise.
    map_list(BOOST_RV_REF(map_list) x, const allocator_type &a)
      : index(allocator_type_internal(a))
    {
        (void)this->insert(this->cbegin(), ::boost::make_move_iterator(x.begin()), ::boost::make_move_iterator(x.end()));
        x.clear();
    }

    //! <b>Effects</b>: Constructs a map_list from the range [first, last)
    //!
    //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
    //!   throws or T's constructor taking an dereferenced InIt throws.
    //!
    //! <b>Complexity</b>: Linear to the range [first, last).
    template <class InpIt>
    map_list(InpIt first, InpIt last)
      : index()
    {  (void)this->insert(this->cbegin(), first, last);  }

    //! <b>Effects</b>: Constructs a list that will use a copy of allocator a
    //!   and inserts a copy of the range [first, last) in the map_list .
    //!
    //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
    //!   throws or T's constructor taking an dereferenced InIt throws.
    //!
    //! <b>Complexity</b>: Linear to the range [first, last).
    template <class InpIt>
    map_list(InpIt first, InpIt last, const allocator_type &a)
      : index(allocator_type_internal(a))
    {  (void)this->insert(this->cbegin(), first, last);  }

    //! <b>Effects</b>: Destroys the list. All stored values are destroyed
    //!   and used memory is deallocated.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Linear to the number of elements.
    ~map_list() BOOST_CONTAINER_NOEXCEPT
    {}

    //! <b>Effects</b>: Makes *this contain the same elements as x.
    //!
    //! <b>Postcondition</b>: this->size() == x.size(). *this contains a copy
    //! of each of x's elements with source's order.
    //!
    //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
    //!
    //! <b>Complexity</b>: Linear to the number of elements in x.
    map_list& operator=(BOOST_COPY_ASSIGN_REF(map_list) x)
    {
      if (&x != this){
        map_list t(x);
        this->swap(t);
      }
      return *this;
    }

    //! <b>Effects</b>: Move assignment. All mx's values are transferred to *this.
    //!
    //! <b>Postcondition</b>: x.empty(). *this contains a the elements x had
    //!   before the function.
    //!
    //! <b>Throws</b>: If allocator_type's copy constructor throws.
    //!
    //! <b>Complexity</b>: Constant.
    map_list& operator=(BOOST_RV_REF(map_list) x)
    {
      if (&x != this){
        this->swap(x);
      }
      return *this;
    }


    //! <b>Effects</b>: Assigns the the range [first, last) to *this.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's constructor from dereferencing InpIt throws.
    //!
    //! <b>Complexity</b>: Linear to n.
    template <class InpIt>
    void assign(InpIt first, InpIt last)
    {
      map_list t;
      t.insert(this->cbegin(), first, last);
      this->swap(t);
    }

    //! <b>Effects</b>: Returns a copy of the internal allocator.
    //!
    //! <b>Throws</b>: If allocator's copy constructor throws.
    //!
    //! <b>Complexity</b>: Constant.
    allocator_type get_allocator() const BOOST_CONTAINER_NOEXCEPT
    {  return allocator_type(this->index.get_allocator()); }


    //////////////////////////////////////////////
    //
    //                iterators
    //
    //////////////////////////////////////////////

    //! <b>Effects</b>: Returns an iterator to the first element contained in the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    iterator begin() BOOST_CONTAINER_NOEXCEPT
    { return iterator(this->list.begin(), iterator_transformer()); }

    //! <b>Effects</b>: Returns a const_iterator to the first element contained in the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_iterator begin() const BOOST_CONTAINER_NOEXCEPT
    {  return const_iterator(this->list.cbegin(), iterator_transformer());   }

    //! <b>Effects</b>: Returns an iterator to the end of the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    iterator end() BOOST_CONTAINER_NOEXCEPT
    {  return iterator(this->list.end(), iterator_transformer());  }

    //! <b>Effects</b>: Returns a const_iterator to the end of the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_iterator end() const BOOST_CONTAINER_NOEXCEPT
    {  return const_iterator(this->list.cend(), iterator_transformer());  }

    //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    reverse_iterator rbegin() BOOST_CONTAINER_NOEXCEPT
    {  return reverse_iterator(this->end());  }

    //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reverse_iterator rbegin() const BOOST_CONTAINER_NOEXCEPT
    {  return const_reverse_iterator(this->begin());  }

    //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    reverse_iterator rend() BOOST_CONTAINER_NOEXCEPT
    {  return reverse_iterator(this->begin());   }

    //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reverse_iterator rend() const BOOST_CONTAINER_NOEXCEPT
    {  return const_reverse_iterator(this->begin());   }

    //! <b>Effects</b>: Returns a const_iterator to the first element contained in the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_iterator cbegin() const BOOST_CONTAINER_NOEXCEPT
    {  return const_iterator(this->begin());   }

    //! <b>Effects</b>: Returns a const_iterator to the end of the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_iterator cend() const BOOST_CONTAINER_NOEXCEPT
    {  return const_iterator(this->end());  }

    //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reverse_iterator crbegin() const BOOST_CONTAINER_NOEXCEPT
    {  return const_reverse_iterator(this->rbegin());  }

    //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
    //! of the reversed list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reverse_iterator crend() const BOOST_CONTAINER_NOEXCEPT
    {  return const_reverse_iterator(this->rend());   }

    //////////////////////////////////////////////
    //
    //                capacity
    //
    //////////////////////////////////////////////

    //! <b>Effects</b>: Returns true if the list contains no elements.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    bool empty() const BOOST_CONTAINER_NOEXCEPT
    {  return this->index.empty();  }

    //! <b>Effects</b>: Returns the number of the elements contained in the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    size_type size() const BOOST_CONTAINER_NOEXCEPT
    {   return this->index.size();   }

    //! <b>Effects</b>: Returns the largest possible size of the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    size_type max_size() const BOOST_CONTAINER_NOEXCEPT
    {  return this->index.max_size();  }


    //////////////////////////////////////////////
    //
    //               element access
    //
    //////////////////////////////////////////////

    //! <b>Requires</b>: !empty()
    //!
    //! <b>Effects</b>: Returns a reference to the first element
    //!   from the beginning of the container.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    reference front() BOOST_CONTAINER_NOEXCEPT
    { return *this->begin(); }

    //! <b>Requires</b>: !empty()
    //!
    //! <b>Effects</b>: Returns a const reference to the first element
    //!   from the beginning of the container.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reference front() const BOOST_CONTAINER_NOEXCEPT
    { return *this->begin(); }

    //! <b>Requires</b>: !empty()
    //!
    //! <b>Effects</b>: Returns a reference to the first element
    //!   from the beginning of the container.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    reference back() BOOST_CONTAINER_NOEXCEPT
    { return *(--this->end()); }

    //! <b>Requires</b>: !empty()
    //!
    //! <b>Effects</b>: Returns a const reference to the first element
    //!   from the beginning of the container.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    const_reference back() const BOOST_CONTAINER_NOEXCEPT
    { return *(--this->end()); }

    //////////////////////////////////////////////
    //
    //                modifiers
    //
    //////////////////////////////////////////////
#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
    ???
#else
    BOOST_MOVE_CONVERSION_AWARE_CATCH(push_front, value_type, void, priv_insert)
#endif

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
      //! <b>Effects</b>: Inserts a copy of x at the end of the list.
      //!
      //! <b>Throws</b>: If memory allocation throws or
      //!   T's copy constructor throws.
      //!
      //! <b>Complexity</b>: Amortized constant time.
      void push_back(const value_type &x);

    //! <b>Effects</b>: Constructs a new element in the end of the list
    //!   and moves the resources of mx to this new element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void push_back(value_type &&x);
#else
      BOOST_MOVE_CONVERSION_AWARE_CATCH(push_back, value_type, void, priv_push_back)
#endif
    ////////////////////////////////////////////// 
    /*multimap:
     iterator insert( const_iterator pos, size_type count, const T& value );*/

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
    //! <b>Effects</b>: Inserts a copy of x at the end of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's copy constructor throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    std::pair<iterator,bool> insert(const value_type &x);

    //! <b>Effects</b>: Constructs a new element in the end of the list
    //!   and moves the resources of mx to this new element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    std::pair<iterator,bool> insert(value_type &&x);
#else
    BOOST_MOVE_CONVERSION_AWARE_CATCH(insert, value_type, std::pair<iterator,bool>, priv_insert)
#endif

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
    //! <b>Effects</b>: Inserts a copy of x at the end of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's copy constructor throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    std::pair<iterator,bool> insert( const_iterator position, const value_type& value );

    //! <b>Effects</b>: Constructs a new element in the end of the list
    //!   and moves the resources of mx to this new element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    std::pair<iterator,bool> insert(value_type &&x);
#else
    BOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG(insert, value_type, std::pair<iterator,bool>, priv_insert)
#endif

    //boost::unordered::piecewise_construct
    
    template< class InputIt >
    iterator insert( InputIt first, InputIt last )
    {
      return this->insert(this->cend(), first, last);
    }

    iterator insert( const_iterator position, InputIt first, InputIt last )
    {
      iterator after_insert = const_iterator_to_iterator(position);
      if(first==last)
        return after_insert;
      iterator first_insert = this->insert(position, *first);
      ++first;
      for(;first!=last; ++first)
        (void)this->insert(position, *first);
      return first_insert;
    }
    
      /* потом void insert( std::initializer_list<value_type> ilist ); 
      std::unordered::unordered_map::insert
      operator= и конструктор std::initializer_list, перемещай, буть н€шой*/
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ////////////////////////////////////////////// 
    /*Ёто потом
#if defined(BOOST_CONTAINER_PERFECT_FORWARDING) || defined(BOOST_CONTAINER_DOXYGEN_INVOKED)

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... in the end of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    void emplace_back(Args&&... args)
    {  this->emplace(this->cend(), boost::forward<Args>(args)...); }

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... in the beginning of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    void emplace_front(Args&&... args)
    {  this->emplace(this->cbegin(), boost::forward<Args>(args)...);  }

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... before p.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    iterator emplace(const_iterator p, Args&&... args)
    {
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!     !!!!!
      NodePtr pnode(AllocHolder::create_node(boost::forward<Args>(args)...));
      return iterator(this->icont().insert(p.get(), *pnode));
    }

#else //#ifdef BOOST_CONTAINER_PERFECT_FORWARDING

#define BOOST_PP_LOCAL_MACRO(n)                                                              \
  BOOST_PP_EXPR_IF(n, template<) BOOST_PP_ENUM_PARAMS(n, class P) BOOST_PP_EXPR_IF(n, >)       \
  void emplace_back(BOOST_PP_ENUM(n, BOOST_CONTAINER_PP_PARAM_LIST, _))                        \
    {                                                                                            \
    this->emplace(this->cend()                                                                \
    BOOST_PP_ENUM_TRAILING(n, BOOST_CONTAINER_PP_PARAM_FORWARD, _));            \
    }                                                                                            \
    \
    BOOST_PP_EXPR_IF(n, template<) BOOST_PP_ENUM_PARAMS(n, class P) BOOST_PP_EXPR_IF(n, >)       \
    void emplace_front(BOOST_PP_ENUM(n, BOOST_CONTAINER_PP_PARAM_LIST, _))                       \
    {                                                                                            \
    this->emplace(this->cbegin()                                                              \
    BOOST_PP_ENUM_TRAILING(n, BOOST_CONTAINER_PP_PARAM_FORWARD, _));            \
    }                                                                                            \
    \
    BOOST_PP_EXPR_IF(n, template<) BOOST_PP_ENUM_PARAMS(n, class P) BOOST_PP_EXPR_IF(n, >)       \
    iterator emplace(const_iterator p                                                            \
    BOOST_PP_ENUM_TRAILING(n, BOOST_CONTAINER_PP_PARAM_LIST, _))                \
    {                                                                                            \
    NodePtr pnode (AllocHolder::create_node                                                   \
    (BOOST_PP_ENUM(n, BOOST_CONTAINER_PP_PARAM_FORWARD, _)));                              \
    return iterator(this->icont().insert(p.get(), *pnode));                                   \
    }                                                                                            \
    //!
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
#include BOOST_PP_LOCAL_ITERATE()

#endif   //#ifdef BOOST_CONTAINER_PERFECT_FORWARDING

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
    //! <b>Effects</b>: Inserts a copy of x at the beginning of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's copy constructor throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void push_front(const T &x);

    //! <b>Effects</b>: Constructs a new element in the beginning of the list
    //!   and moves the resources of mx to this new element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void push_front(T &&x);
#else
    BOOST_MOVE_CONVERSION_AWARE_CATCH(push_front, T, void, priv_push_front)
#endif

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
      //! <b>Effects</b>: Inserts a copy of x at the end of the list.
      //!
      //! <b>Throws</b>: If memory allocation throws or
      //!   T's copy constructor throws.
      //!
      //! <b>Complexity</b>: Amortized constant time.
      void push_back(const T &x);

    //! <b>Effects</b>: Constructs a new element in the end of the list
    //!   and moves the resources of mx to this new element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void push_back(T &&x);
#else
      BOOST_MOVE_CONVERSION_AWARE_CATCH(push_back, T, void, priv_push_back)
#endif

#if defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
      //! <b>Requires</b>: position must be a valid iterator of *this.
      //!
      //! <b>Effects</b>: Insert a copy of x before position.
      //!
      //! <b>Returns</b>: an iterator to the inserted element.
      //!
      //! <b>Throws</b>: If memory allocation throws or x's copy constructor throws.
      //!
      //! <b>Complexity</b>: Amortized constant time.
      iterator insert(const_iterator position, const T &x);

    //! <b>Requires</b>: position must be a valid iterator of *this.
    //!
    //! <b>Effects</b>: Insert a new element before position with mx's resources.
    //!
    //! <b>Returns</b>: an iterator to the inserted element.
    //!
    //! <b>Throws</b>: If memory allocation throws.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    iterator insert(const_iterator position, T &&x);
#else
      BOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG(insert, T, iterator, priv_insert, const_iterator)
#endif

      //! <b>Requires</b>: p must be a valid iterator of *this.
      //!
      //! <b>Effects</b>: Inserts n copies of x before p.
      //!
      //! <b>Returns</b>: an iterator to the first inserted element or p if n is 0.
      //!
      //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
      //!
      //! <b>Complexity</b>: Linear to n.
      iterator insert(const_iterator p, size_type n, const T& x)
    {
      typedef constant_iterator<value_type, difference_type> cvalue_iterator;
      return this->insert(p, cvalue_iterator(x, n), cvalue_iterator());
    }

    //! <b>Requires</b>: p must be a valid iterator of *this.
    //!
    //! <b>Effects</b>: Insert a copy of the [first, last) range before p.
    //!
    //! <b>Returns</b>: an iterator to the first inserted element or p if first == last.
    //!
    //! <b>Throws</b>: If memory allocation throws, T's constructor from a
    //!   dereferenced InpIt throws.
    //!
    //! <b>Complexity</b>: Linear to std::distance [first, last).
    template <class InpIt>
    iterator insert(const_iterator p, InpIt first, InpIt last
#if !defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
      , typename container_detail::enable_if_c
      < !container_detail::is_convertible<InpIt, size_type>::value
      && (container_detail::is_input_iterator<InpIt>::value
      || container_detail::is_same<alloc_version, allocator_v1>::value
      )
      >::type * = 0
#endif
      )
    {
      const typename Icont::iterator ipos(p.get());
      iterator ret_it(ipos);
      if(first != last){
        ret_it = iterator(this->icont().insert(ipos, *this->create_node_from_it(first)));
        ++first;
      }
      for (; first != last; ++first){
        this->icont().insert(ipos, *this->create_node_from_it(first));
      }
      return ret_it;
    }

#if !defined(BOOST_CONTAINER_DOXYGEN_INVOKED)
    template <class FwdIt>
    iterator insert(const_iterator p, FwdIt first, FwdIt last
      , typename container_detail::enable_if_c
      < !container_detail::is_convertible<FwdIt, size_type>::value
      && !(container_detail::is_input_iterator<FwdIt>::value
      || container_detail::is_same<alloc_version, allocator_v1>::value
      )
      >::type * = 0
      )
    {
      //Optimized allocation and construction
      insertion_functor func(this->icont(), p.get());
      iterator before_p(p.get());
      --before_p;
      this->allocate_many_and_construct(first, std::distance(first, last), func);
      return ++before_p;
    }
#endif*/

    

    //! <b>Effects</b>: Removes the first element from the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void pop_front() 
    {  (void)this->erase(this->cbegin());      }

    //! <b>Effects</b>: Removes the last element from the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void pop_back() 
    {  const_iterator tmp = this->cend(); (void)this->erase(--tmp);  }

    //! <b>Requires</b>: p must be a valid iterator of *this.
    //!
    //! <b>Effects</b>: Erases the element at p p.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    iterator erase(const_iterator p) BOOST_CONTAINER_NOEXCEPT
    {  
      iterator next = this->const_iterator_to_iterator(++p);
      (void)this->index.erase(iterator_to_index_iterator(p));
      return next;
    }

    //! <b>Requires</b>: first and last must be valid iterator to elements in *this.
    //!
    //! <b>Effects</b>: Erases the elements pointed by [first, last).
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Linear to the distance between first and last.
    iterator erase(const_iterator first, const_iterator last)
    {  
      for(;first!=last ; first = this->erase(first));
      return this->const_iterator_to_iterator(first);
    }

    size_type erase(const key_type& p)
    {  
      const std::pair<const_local_iterator, const_local_iterator> for_erase = this->equal_range(p);
      const size_type removed_amount = std::distance(for_erase.first, for_erase.second);
      (void)this->erase(for_erase.first, for_erase.second);
      return removed_amount;
    }

    //! <b>Requires</b>: p must be a valid iterator of *this.
    //!
    //! <b>Effects</b>: Erases the element at p p.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Amortized constant time.
    void erase(const_local_iterator p)//not for multimap. use transform_iterator to *_map::*_iterator
    {
      (void)this.erase(p);
    }

    //! <b>Requires</b>: first and last must be valid iterator to elements in *this.
    //!
    //! <b>Effects</b>: Erases the elements pointed by [first, last).
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Should be linear to the distance between first and last.
    void erase(const_local_iterator first, const_local_iterator last)//not for multimap. use transform_iterator to *_map::*_iterator
    {  
      (void)this.erase(first, last);
    }

    //! <b>Effects</b>: Swaps the contents of *this and x.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Constant.
    void swap(map_list& x)
    {  this->index.swap(x.index); this->list.swap(x.list); }

    //! <b>Effects</b>: Erases all the elements of the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: Linear to the number of elements in the list.
    void clear() BOOST_CONTAINER_NOEXCEPT
    {  this->index.clear();  }

   //////////////////////////////////////////////////////////////////////////
    //! <b>Effects</b>: Removes all the elements for which a specified
    //!   key is satisfied.
    //!
    //! <b>Throws</b>: No throws.
    //!
    //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
    //!   and iterators to elements that are not removed remain valid.
    void remove(const key_type& key)
    {
      (void)this->erase(this->find(key));//realization only not for multi_*.
    }

    //! <b>Effects</b>: Removes all the elements for which a specified
    //!   predicate is satisfied.
    //!
    //! <b>Throws</b>: If pred throws.
    //!
    //! <b>Complexity</b>: Linear time. It performs exactly size() calls to the predicate.
    //!
    //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
    //!   and iterators to elements that are not removed remain valid.
    template <class Pred>
    void remove_if(Pred pred)
    {
      for(const_iterator 
            current = this->cbegin(),
            last = this->cend();
          current!=last;
          current = ::std::find_if(current, last, pred)))
        current = this->erase(current);
    }

    //! <b>Effects</b>: Removes adjacent duplicate elements or adjacent
    //!   elements that satisfy some binary predicate from the list.
    //!
    //! <b>Throws</b>: If pred throws.
    //!
    //! <b>Complexity</b>: Linear time (size()-1 comparisons calls to pred()).
    //!
    //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
    //!   and iterators to elements that are not removed remain valid.
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred)
    {
      for(const_iterator 
            current = this->cbegin(),
            last = this->cend();
          current!=last;
          current = ::std::adjacent_find(current, last, binary_pred)))
        current = this->erase(current);
    }

    //! <b>Requires</b>: The lists x and *this must be distinct.
    //!
    //! <b>Effects</b>: This function removes all of x's elements and inserts them
    //!   in order into *this according to std::less<value_type>. The merge is stable;
    //!   that is, if an element from *this is equivalent to one from x, then the element
    //!   from *this will precede the one from x.
    //!
    //! <b>Throws</b>: If comparison throws.
    //!
    //! <b>Complexity</b>: This function is linear time: it performs at most
    //!   size() + x.size() - 1 comparisons.
    void merge(map_list &x)
    {  this->merge(x, ::std::less<value_type>());  }

    //! <b>Requires</b>: The lists x and *this must be distinct.
    //!
    //! <b>Effects</b>: This function removes all of x's elements and inserts them
    //!   in order into *this according to std::less<value_type>. The merge is stable;
    //!   that is, if an element from *this is equivalent to one from x, then the element
    //!   from *this will precede the one from x.
    //!
    //! <b>Throws</b>: If comparison throws.
    //!
    //! <b>Complexity</b>: This function is linear time: it performs at most
    //!   size() + x.size() - 1 comparisons.
    void merge(BOOST_RV_REF(map_list) x)
    {  this->merge(static_cast<list&>(x)); }

    //! <b>Requires</b>: p must be a comparison function that induces a strict weak
    //!   ordering and both *this and x must be sorted according to that ordering
    //!   The lists x and *this must be distinct.
    //!
    //! <b>Effects</b>: This function removes all of x's elements and inserts them
    //!   in order into *this. The merge is stable; that is, if an element from *this is
    //!   equivalent to one from x, then the element from *this will precede the one from x.
    //!
    //! <b>Throws</b>: If comp throws.
    //!
    //! <b>Complexity</b>: This function is linear time: it performs at most
    //!   size() + x.size() - 1 comparisons.
    //!
    //! <b>Note</b>: Iterators and references to *this are not invalidated.
    template <class StrictWeakOrdering>
    void merge(map_list &x, const StrictWeakOrdering &comp)
    {
      const_iterator 
        di = this->cbegin(),
        si = x.cbegin(),
        die = this->cend(),
        sie = x.cend();
      while(di!=die && si!=sie)
      {
        if(comp(*si, *di))
        {
          (void)this->insert(si, ::boost::move(*di));
          (void)++di;
        }
        else
          (void)++si;
      }
      while(si!=sie)
      {
        (void)this->insert(si, ::boost::move(*di));
        (void)++di;
      }
      x.clear();
    }

    //! <b>Requires</b>: p must be a comparison function that induces a strict weak
    //!   ordering and both *this and x must be sorted according to that ordering
    //!   The lists x and *this must be distinct.
    //!
    //! <b>Effects</b>: This function removes all of x's elements and inserts them
    //!   in order into *this. The merge is stable; that is, if an element from *this is
    //!   equivalent to one from x, then the element from *this will precede the one from x.
    //!
    //! <b>Throws</b>: If comp throws.
    //!
    //! <b>Complexity</b>: This function is linear time: it performs at most
    //!   size() + x.size() - 1 comparisons.
    //!
    //! <b>Note</b>: Iterators and references to *this are not invalidated.
    template <class StrictWeakOrdering>
    void merge(BOOST_RV_REF(list) x, StrictWeakOrdering comp)
    {  this->merge(static_cast<list&>(x), comp); }

    //! <b>Effects</b>: This function sorts the list *this according to std::less<value_type>.
    //!   The sort is stable, that is, the relative order of equivalent elements is preserved.
    //!
    //! <b>Throws</b>: If comparison throws.
    //!
    //! <b>Notes</b>: Iterators and references are not invalidated.
    //!  
    //! <b>Complexity</b>: The number of comparisons is approximately N log N, where N
    //!   is the list's size.
    void sort()
    {  this->sort(::std::less<value_type>());  }

    //! <b>Effects</b>: This function sorts the list *this according to std::less<value_type>.
    //!   The sort is stable, that is, the relative order of equivalent elements is preserved.
    //!
    //! <b>Throws</b>: If comp throws.
    //!
    //! <b>Notes</b>: Iterators and references are not invalidated.
    //!
    //! <b>Complexity</b>: The number of comparisons is approximately N log N, where N
    //!   is the list's size.
    template <class StrictWeakOrdering>
    void sort(StrictWeakOrdering comp)
    {
      // nothing if the list has length 0 or 1.
      if (this->size() < 2)
        return;
      this->list.sort(ValueCompareToNodeCompare<StrictWeakOrdering>(comp));
    }

    //! <b>Effects</b>: Reverses the order of elements in the list.
    //!
    //! <b>Throws</b>: Nothing.
    //!
    //! <b>Complexity</b>: This function is linear time.
    //!
    //! <b>Note</b>: Iterators and references are not invalidated
    void reverse() BOOST_CONTAINER_NOEXCEPT
    {  this->list.reverse(); }   

    std::pair<local_iterator, local_iterator> equal_range( const key_type& key )//not for multimap
    {
      local_iterator result = local_iterator(this->find(Key));
      return std::make_pair(result, result);
    }

    std::pair<const_local_iterator, const_local_iterator> equal_range( const key_type& key ) const//not for multimap
    {
      const_local_iterator result = const_local_iterator(this->find(Key));
      return std::make_pair(result, result);
    }

    hasher hash_function() const
    {wat?
      return this->index.hash_function();
    }

    key_equal key_eq() const
    {wat?
      return this->index.key_eq();
    }

    mapped_type& operator[](const key_type& k)
    {
      return *this->insert(this->cend(), std::make_pair(k, mapped_type())).first;
    }

    mapped_type& at(const key_type& k)
    {
      if(const iterator Result = this->find(k)!=this->end())
        return *Result;
      throw std::out_of_range();
    }

    mapped_type const& at(const key_type& k) const
    {
      if(const const_iterator Result = this->find(k)!=this->cend())
        return *Result;
      throw std::out_of_range();
    }

    size_type count( const Key& key ) const
    {
      return ::std::distance(this->equal_range(key));
    }

    // hash policy

    float load_factor() const 
    {
      return this->index.load_factor();
    }

    void max_load_factor(float m)
    {
      this->index.max_load_factor(m);
    }

    void rehash(size_type n)
    {
      this->index.rehash(n);
    }

    void reserve(size_type n)
    {
      this->index.reserve(n);
    }

    template< class Key, class T, class Hash, class KeyEqual, class Allocator >
    friend void operator==( unordered_map<Key,T,Hash,KeyEqual,Allocator> &lhs, unordered_map<Key,T,Hash,KeyEqual,Allocator> &rhs)
    {
      return lhs.list==rhs.list;
    }

    template< class Key, class T, class Hash, class KeyEqual, class Allocator >
    friend void operator=!( unordered_map<Key,T,Hash,KeyEqual,Allocator> &lhs, unordered_map<Key,T,Hash,KeyEqual,Allocator> &rhs)
    {
      return lhs.list!=rhs.list;
    }

    template< class Key, class T, class Hash, class KeyEqual, class Alloc >
    friend void swap( unordered_map<Key,T,Hash,KeyEqual,Alloc>& lhs, unordered_map<Key,T,Hash,KeyEqual,Alloc>& rhs )
    {
      using namespace std;
      swap(lhs.index, rhs.index);
      swap(lhs.list, rhs.list);
    }
    [s]ты снова всЄ сломал. указатель в каждом элементе остаЄтс€ на старый контейнер u_m.[/s]ќбменивай указатель на контейнер, а не контейнер
    всю перемещающую петушню перепиши на swap!

#if defined(BOOST_CONTAINER_PERFECT_FORWARDING) || defined(BOOST_CONTAINER_DOXYGEN_INVOKED)

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... in the end of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    void emplace_back(Args&&... args)
    {  (void)this->emplace(this->cend(), ::boost::forward<Args>(args)...); }

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... in the beginning of the list.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    void emplace_front(Args&&... args)
    {  (void)this->emplace(this->cbegin(), ::boost::forward<Args>(args)...);  }

    //! <b>Effects</b>: Inserts an object of type T constructed with
    //!   std::forward<Args>(args)... before p.
    //!
    //! <b>Throws</b>: If memory allocation throws or
    //!   T's in-place constructor throws.
    //!
    //! <b>Complexity</b>: Constant
    template <class... Args>
    std::pair<iterator, bool> emplace(const_iterator p, Args&&... args)
    {
      std::pair<typename index_type::iterator, bool> emplace_result = this->index.emplace(::boost::forward<Args>(args)...);
      insert_mode::emplace(*this, p, )
      //как и с insert нужно 2 вида эмплейса в зависимости от шаблонной стратегии 
    }

    template <class... Args>
    iterator emplace_hint(const_local_iterator hint, const_iterator p, Args&&... args)
    {
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!     !!!!!
        NodePtr pnode(AllocHolder::create_node(boost::forward<Args>(args)...));
      return iterator(this->icont().insert(p.get(), *pnode));
    }

#else //#ifdef BOOST_CONTAINER_PERFECT_FORWARDING
    ???
#endif

private:
    void priv_push_back (const value_type &x)  
    {  (void)this->insert(this->cend(), x);    }

    void priv_push_back (BOOST_RV_REF(value_type) x)
    {  (void)this->insert(this->cend(), ::boost::move(x));    }

    void priv_push_front (const value_type &x)  
    {  (void)this->insert(this->cbegin(), x);  }

    void priv_push_front (BOOST_RV_REF(value_type) x)
    {  (void)this->insert(this->cbegin(), ::boost::move(x));  }

    std::pair<iterator,bool> priv_insert(const value_type& value ){
      return this->insert(this->cend(), value);
    }

    std::pair<iterator,bool> priv_insert(BOOST_RV_REF(value_type) value ){
      return this->insert(this->cend(), ::boost::move(value));
    }

    std::pair<iterator,bool> priv_insert( const_iterator position, const value_type& value ){
      typedef insert_mode::BOOST_NESTED_TEMPLATE Executer<map_list> Executer;
      return Executer::insert(*this, position, value);
    }
    std::pair<iterator,bool> priv_insert( const_iterator position, BOOST_RV_REF(value_type) value ){
      typedef insert_mode:: BOOST_NESTED_TEMPLATE Executer<map_list> Executer;
      return Executer::insert(*this, position, ::boost::move(value));
    }
  };
  
  class not_change_if_exist
  {
  public:
    template<class Container>
    class Executer
    {
      typedef typename Container::const_iterator const_iterator;
      typedef typename Container::iterator iterator;
      typedef typename Container::const_local_iterator const_local_iterator;
      typedef typename Container::index_type::iterator index_iterator;
      typedef typename Container::value_type value_type;

      std::pair<index_iterator, bool> index_insert(Container& c, BOOST_RV_REF(value_type) value, index_iterator hint=index_iterator())
      {
        if(index_iterator()!=hint)
          return std::make_pair(c.index.emplace_hint(hint, ::boost::move(value.first), ::boost::move(value.second)), true);
        else
          return c.index.emplace(hint, ::boost::move(value.first), ::boost::move(value.second));
        //нужно нормальный std::pair перемещать, а не эмплейсить в новом стандарте, а дл€ старого ок
        //emplace дл€ *set будет не на три параметра, а просто на n
      }

      std::pair<index_iterator, bool> index_insert(Container& c, const value_type& value, index_iterator hint=index_iterator())
      {
        if(index_iterator()!=hint)
          return std::make_pair(c.index.insert(hint, value), true);
        else
          return c.index.insert(hint, value);
      }

      std::pair<iterator, bool> list_update(Container& c, const const_iterator position, const std::pair<index_iterator, bool>& insert_result)
      {
        list_type::iterator it_result;
        if(insert_result.second)
          it_result = c.list.insert(position.base(), *insert_result.first);
        else
          it_result = boost::iterator_to(*insert_result.first);
        it_result->index_iterator_getter = this;
        return std::make_pair(iterator(it_result), insert_result.second);
      }

    public:
      template<class T> 
      static std::pair<iterator, bool> 
        insert(Container& c, const const_iterator position, BOOST_FWD_REF(T) value, const const_local_iterator hint = const_local_iterator())
      {
        return Executer::list_update(c, position, index_insert(c, hint.base(), ::boost::forward<T>(value)));
      }
    };
  };

  class move_if_exist
  {
  public:
    template<class Container>
    class Executer
    {
      typedef typename Container::const_iterator const_iterator;
      typedef typename Container::iterator iterator;
      typedef typename Container::index_type::iterator index_iterator;
      typedef not_change_if_exist:: BOOST_NESTED_TEMPLATE Executer<Container> SimpleInserter;
      typedef typename Container::const_local_iterator const_local_iterator;

    public:
      template<class T> 
      static std::pair<iterator, bool> 
        insert(Container& c, const_iterator position, BOOST_FWD_REF(T) value)
      { тут тоже нужно вынести, но пока не так все просто
        const index_iterator find_result = c.index.find(value.first);
        if(find_result==c.end())
          return SimpleInserter::insert(c, position, ::boost::forward<T>(value));
        const_iterator iterator_in_list = const_iterator(boost::iterator_to(*find_result));
        if(iterator_in_list == position)
          ++iterator_in_list;
        return SimpleInserter::insert(c, iterator_in_list, ::boost::forward<T>(value), const_local_iterator(find_result)/*лишн€€ работа*/);
      }
    };
  };
}

namespace boost{

  template<
    class K,
    class T,
    class H,
    class P,
    class A>
  struct has_nothrow_move< map_list<K, T, H, P, A> >: 
    ::boost::mpl::and_<
      ::boost::has_nothrow_move<typename map_list<K, T, H, P, A>::list_type>,
      ::boost::has_nothrow_move<typename map_list<K, T, H, P, A>::index_type>
  {};

  template<
    class K,
    class T,
    class H,
    class P,
    class A>
  struct has_trivial_destructor_after_move< map_list<K, T, H, P, A> >: 
    ::boost::mpl::and_<
      ::boost::has_trivial_destructor_after_move<typename map_list<K, T, H, P, A>::list_type>,
      ::boost::has_trivial_destructor_after_move<typename map_list<K, T, H, P, A>::index_type>
  {};

}  //namespace boost{


#endif // assoc_list_h__
