#ifndef random_access_list_h__
#define random_access_list_h__
#include "common\accessory\accessory.h"
#include <boost/functional/hash.hpp>
#include <boost/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/scope_exit.hpp>
#include <boost/container/allocator_traits.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/algorithm/lexicographical_compare.hpp>
#include <boost/range.hpp>
#include <functional>
#include <utility>
#include <boost/call_traits.hpp>
#include <boost/compressed_pair.hpp>
#include <Common/ForBoost/unique_ptr_extension.h>
#include <boost/intrusive/list.hpp>
#include <boost/config/suffix.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/swap.hpp>
#include <boost/utility.hpp>

COMMENT
((
	Random access list (ral).

Ral based on self-balancing binary search AVL-tree with implicit keys.
Ral have stable iterators, basic thread safety guarantee, strong exception safety guarantee.
Average and worst case algorithmic complexity is O(log n) for all operations with one element.
Memory consumption is O(1) for all operations with one element.
Average algorithmic complexity of iterate to next element and get its value is O(1).
Memory consumption of container is O(n).
C++03 move emulation with boost.
))

/*Поддержка аллокатора весьма пока условна, тк как минимум
typedef unspecified       propagate_on_container_copy_assignment;
typedef unspecified       propagate_on_container_move_assignment;
typedef unspecified       propagate_on_container_swap;           
static Alloc select_on_container_copy_construction(const Alloc &);
scoped allocators 
and others
не поддерживаются.*/


#define BOOST_CONTAINER_NOEXCEPT_IF_BINARY_PREDICATE_NOEXCEPT(predicate) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(predicate(::boost::declval<value_type>(), ::boost::declval<value_type>())))

namespace container {
	namespace container_detail{
		
		template<class TSource>
		struct iterator_value_t
		{
			typedef typename boost::mpl::if_<boost::is_const<TSource>, const typename TSource::value_type, typename TSource::value_type >::type type;
		};
		
		template<class TSource, class TType = void*>
		struct TEnableIfIsNotConstIterator: ::boost::enable_if<::boost::mpl::not_< ::boost::is_const<typename iterator_value_t<TSource>::type> >, TType> {};
		template<class TPossibleIterator>
		struct TIsInputOrMoreIterator: ::boost::is_convertible<typename ::std::iterator_traits<TPossibleIterator>::iterator_category, ::std::input_iterator_tag>{};
		
		template<class TValue>
		struct TAvlTreeTypes{
			typedef ::std::size_t size_type;
			typedef ::std::ptrdiff_t difference_type;
			typedef TValue value_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;
		};
		
		namespace {const class construct_t{} construct;};
		
		template<class TValue>
		struct TAvlTreeNode: public TAvlTreeTypes<TValue> {
			typedef typename ::boost::aligned_storage<sizeof(TValue), ::boost::alignment_of<TValue>::value >::type TValueStorage;
			TValueStorage _value;
			TAvlTreeNode* _parent;
			size_type _height, _size;
			TAvlTreeNode *_left, *_right;
			TAvlTreeNode(void) BOOST_CONTAINER_NOEXCEPT :
				_parent(NULL),
				_left(NULL),
				_right(NULL),
				_height(1),
				_size(1)
			{}
			void resetInvariant(void) BOOST_CONTAINER_NOEXCEPT {
				_right=_left=_parent=NULL;
				_size=_height=1;
			}
			reference getValue(void) BOOST_CONTAINER_NOEXCEPT {
				return reinterpret_cast<reference>(this->_value);
			}
			const_reference getValue(void) const BOOST_CONTAINER_NOEXCEPT {
				return reinterpret_cast<const_reference>(this->_value);
			}
			void destructValue(void) const{
				this->getValue().~value_type();
			}
			#define AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER 10
			#define GENERATE_AVL_TREE_NODE_CONSTRUCTOR(_, ARG_COUNTER, __)					\
				PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)											\
				PP_EXPLICIT_IF_ZERO(ARG_COUNTER) TAvlTreeNode(construct_t/*,*/ PP_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) ):\
					_parent(NULL),															\
					_left(NULL),															\
					_right(NULL),															\
					_height(1),																\
					_size(1)																\
				{																			\
					::new(static_cast<void*>(&this->_value)) value_type( PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER) );\
				}
			BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_AVL_TREE_NODE_CONSTRUCTOR, BOOST_PP_EMPTY())
			#undef GENERATE_AVL_TREE_NODE_CONSTRUCTOR
		private:
			TAvlTreeNode& operator=(const TAvlTreeNode&);//forbidden member
			TAvlTreeNode(const TAvlTreeNode&);//forbidden member
		public:
			bool operator==(const TAvlTreeNode& other) const {
				return this->getValue()==other.getValue();
			}
			bool operator<(const TAvlTreeNode& other) const {
				return this->getValue()<other.getValue();
			}
		};
		
		template<class TValue>
		struct TAvlTreeNodeIntrusiveTraits
		{
			typedef TValue				                       node;
			typedef node *			                           node_ptr;
			typedef const node *							   const_node_ptr;

			static node_ptr get_next(const_node_ptr n)         {  return n->_right;  }
			static void set_next(node_ptr n, node_ptr next)    {  n->_right = next;  }
			static node_ptr get_previous(const_node_ptr n)     {  return n->_left;   }
			static void set_previous(node_ptr n, node_ptr prev){  n->_left = prev;   }
		};
		template <class T, class R>
		R nothing_do(T){return R();}
		template<class TInternalAllocatorForStoreAtDeleter, class TInternalAllocatorTraits, class T, class TFinalizationFunctor =void(*)(T*)>
		struct TDeleter{
		private:
			::boost::compressed_pair<TInternalAllocatorForStoreAtDeleter, TFinalizationFunctor> _allocator_and_finalizer;
		public:
			explicit TDeleter(typename ::boost::call_traits<TInternalAllocatorForStoreAtDeleter>::reference/*param_type*/ allocator, TFinalizationFunctor finalizer = &nothing_do<T*, void>):
				_allocator_and_finalizer(allocator, finalizer)
			{}
			void operator()(T* const forDelete){
				TInternalAllocatorForStoreAtDeleter allocator_ = this->_allocator_and_finalizer.first();
				BOOST_SCOPE_EXIT(&allocator_, forDelete){
					TInternalAllocatorTraits::deallocate(allocator_, forDelete, 1);
				}BOOST_SCOPE_EXIT_END
				this->_allocator_and_finalizer.second()(forDelete);
				TInternalAllocatorTraits::destroy(allocator_, forDelete);
			}
			typename ::boost::call_traits<TInternalAllocatorForStoreAtDeleter>::const_reference getInternalAllocator(void) const{
				return this->_allocator_and_finalizer.first();
			}
			friend void swap(TDeleter& l, TDeleter& r){
				::boost::swap(l._allocator_and_finalizer, r._allocator_and_finalizer);
			}
		};
		template<class TValue>
		void exactly_destroy_node_value(TAvlTreeNode<TValue>* const node){node->destructValue();}
		
		template<class TValue, class TInternalAllocator>
		class TCache{
		public:
			TAvlTreeNode<TValue> *_before_begin, *_begin, *_end;
			TCache(void):
				_before_begin(NULL),
				_begin(NULL), 
				_end(NULL)
			{}
		private:
			TCache(const TCache&);//forbidden member
			TCache& operator=(const TCache&);//forbidden member
		};
		
		template<class TValue, class TInternalAllocator, class TAllocatorTraits>
		class TCacheAndAllocator{
			typedef TAvlTreeNode<TValue> TAvlTreeNode;
			TCacheAndAllocator(void);//forbidden member
			TCacheAndAllocator(const TCacheAndAllocator&);//forbidden member
			TCacheAndAllocator& operator=(const TCacheAndAllocator&);//forbidden member
		public:
			explicit TCacheAndAllocator(const TInternalAllocator& allocator):
				_allocator_and_cache(allocator)
			{
				this->makeEmptyNode(this->_allocator_and_cache.second()._before_begin);
				BOOST_SCOPE_EXIT(this_){
					if(::std::uncaught_exception())
						this_->deleteEmptyNode(this_->_allocator_and_cache.second()._before_begin);
				}BOOST_SCOPE_EXIT_END
				this->makeEmptyNode(this->_allocator_and_cache.second()._end);
			}
			~TCacheAndAllocator(void){
				this->deleteEmptyNode(this->_allocator_and_cache.second()._before_begin);
				this->deleteEmptyNode(this->_allocator_and_cache.second()._end);
			}
			/*mutable*/ ::boost::compressed_pair<TInternalAllocator, TCache<TValue, TInternalAllocator> > _allocator_and_cache;
			
			void makeEmptyNode(TAvlTreeNode* & node){
				node = TAllocatorTraits::allocate(this->_allocator_and_cache.first(), 1);
				BOOST_SCOPE_EXIT(this_, node){
					if(::std::uncaught_exception()){
						TAllocatorTraits::deallocate(this_->_allocator_and_cache.first(), node, 1);
						node = NULL;
					}
				}BOOST_SCOPE_EXIT_END
				TAllocatorTraits::construct(this->_allocator_and_cache.first(), node);
			}
			void deleteEmptyNode(TAvlTreeNode* & node){
				if(node){
					BOOST_SCOPE_EXIT(this_, node){
						TAllocatorTraits::deallocate(this_->_allocator_and_cache.first(), node, 1);
						node = NULL;
					}BOOST_SCOPE_EXIT_END
					TAllocatorTraits::destroy(this->_allocator_and_cache.first(), node);
				}
			}
			void resetInvariant(void){
				this->_allocator_and_cache.second()._before_begin->resetInvariant();
				this->_allocator_and_cache.second()._end->resetInvariant();
			}
			TAvlTreeNode*& before_begin(void){
				return this->_allocator_and_cache.second()._before_begin;
			}
			TAvlTreeNode*& begin(void){
				return this->_allocator_and_cache.second()._begin;
			}
			TAvlTreeNode*& end(void){
				return this->_allocator_and_cache.second()._end;
			}
			TAvlTreeNode* const& before_begin(void)const{
				return this->_allocator_and_cache.second()._before_begin;
			}
			TAvlTreeNode*const& begin(void)const{
				return this->_allocator_and_cache.second()._begin;
			}
			TAvlTreeNode*const& end(void)const{
				return this->_allocator_and_cache.second()._end;
			}
			TInternalAllocator& getInternalAllocator(void){
				return this->_allocator_and_cache.first();
			}
			TInternalAllocator const& getInternalAllocator(void)const{
				return this->_allocator_and_cache.first();
			}
		};
		
		struct TNodeType{ enum E {LEFT = 0, RIGHT}; };
		
		template<class T, class TAllocator >
		class avl_tree_stump: public TAvlTreeTypes<T> {
			avl_tree_stump(const avl_tree_stump&);//forbidden member
			avl_tree_stump& operator=(const avl_tree_stump&);//forbidden member
			
		public:
			typedef ::boost::container::allocator_traits<TAllocator> TExternalAllocatorTraits;
			typedef TAllocator allocator_type;
			typedef typename TExternalAllocatorTraits::pointer pointer;
			typedef typename TExternalAllocatorTraits::const_pointer const_pointer;
			typedef value_type  value_type_copy;
			typedef TAvlTreeNode<value_type> TAvlTreeNode;
			//typedef typename TAllocator::BOOST_NESTED_TEMPLATE rebind <TAvlTreeNode>::other TInternalAllocator;
			typedef typename TExternalAllocatorTraits::BOOST_NESTED_TEMPLATE portable_rebind_alloc<TAvlTreeNode>::type TInternalAllocator;
			typedef ::boost::container::allocator_traits<TInternalAllocator> TAllocatorTraits;
			typedef TCacheAndAllocator<value_type, TInternalAllocator, TAllocatorTraits> TCacheAndAllocator;
			typedef container_detail::TDeleter<TInternalAllocator&, TAllocatorTraits, TAvlTreeNode> TAvlTreeNodeDeleter;
		private:
			typedef ::boost::intrusive::trivial_value_traits<TAvlTreeNodeIntrusiveTraits<TAvlTreeNode>, ::boost::intrusive::normal_link>		avl_tree_value_intrusive_traits;
			typedef ::boost::intrusive::value_traits<avl_tree_value_intrusive_traits>															avl_tree_value_intrusive_options;
		public:
			typedef ::boost::intrusive::list<TAvlTreeNode, avl_tree_value_intrusive_options, ::boost::intrusive::constant_time_size<false> >	TAvlTreeNodeIntrusiveList;
			explicit avl_tree_stump(const TAllocator& alloc = TAllocator()):
				_root(),
				_cacheAndAllocator(alloc)
			{
				this->resetInvariant();
			}
			~avl_tree_stump(void)
			{
				this->destruct_value_and_delete_node_since_begin_to_before_end();
			}
			TInternalAllocator& getInternalAllocator(void) BOOST_CONTAINER_NOEXCEPT {
				return this->_cacheAndAllocator.getInternalAllocator();
			}
			const TInternalAllocator& getInternalAllocator(void) const BOOST_CONTAINER_NOEXCEPT {
				return this->_cacheAndAllocator.getInternalAllocator();
			}
			const TAvlTreeNode* getBeforeBegin(void) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_cacheAndAllocator.before_begin());
				return this->_cacheAndAllocator.before_begin();
			}
			TAvlTreeNode* getBegin(void) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_cacheAndAllocator.begin());
				return this->_cacheAndAllocator.begin();
			}
			TAvlTreeNode* getEnd(void) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_cacheAndAllocator.end());
				return this->_cacheAndAllocator.end();
			}
			size_type algorithmic_complexity() const BOOST_CONTAINER_NOEXCEPT {
				return this->height(this->_root)+1;
			}
		private:
			void resetInvariant(void){
				this->_cacheAndAllocator.resetInvariant();
				this->_root=NULL;
				TAvlTreeNode* const end_ = this->insert_before(NULL, this->_cacheAndAllocator.end());
				this->insert_before(end_, this->_cacheAndAllocator.before_begin());
				this->_cacheAndAllocator.begin() = end_;
			}
			TCacheAndAllocator _cacheAndAllocator;
			TAvlTreeNode* _root;
			void recalculate_height_and_size_with_data_from_two_childs(TAvlTreeNode* lowest_incorrect) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(lowest_incorrect);
				if(!lowest_incorrect)
					return;
				lowest_incorrect->_size=size(lowest_incorrect->_left) + size(lowest_incorrect->_right) + 1;
				lowest_incorrect->_height=(::std::max)(height(lowest_incorrect->_left), height(lowest_incorrect->_right)) + 1;
			}
			void replace_subnode_if_equal_to_such_and_node_not_null(TAvlTreeNode* const node, TAvlTreeNode* const such, TAvlTreeNode* const replacer){
				if(node){
					if(node->_left==such)
						node->_left=replacer;
					else if(node->_right==such)
						node->_right=replacer;
					else SHOULD_NEVER_REACH_HERE;
				}
			}
			void right_rotate(TAvlTreeNode*& root_, const bool needHeightAndSizeRestore = true) BOOST_CONTAINER_NOEXCEPT {//don't setup right this->_root
				ASSERT(&root_);
				TAvlTreeNode* const root = root_;
				ASSERT(root);
				ASSERT(root->_left);
				if(!(root && root->_left))
					return;
				TAvlTreeNode* const pivot = root->_left;
				TAvlTreeNode* const pivot_inner_child = pivot->_right;
				pivot->_right = root;
				pivot->_parent = root->_parent;
				root->_parent = pivot;
				root->_left = pivot_inner_child;
				if(pivot_inner_child)
					pivot_inner_child->_parent = root;
				replace_subnode_if_equal_to_such_and_node_not_null(pivot->_parent, root, pivot);
				if(needHeightAndSizeRestore){
					recalculate_height_and_size_with_data_from_two_childs(root);
					recalculate_height_and_size_with_data_from_two_childs(pivot);
				}
				root_ = pivot;
			}
			void left_rotate(TAvlTreeNode*& root_, const bool needHeightAndSizeRestore = true) BOOST_CONTAINER_NOEXCEPT {//don't setup right this->_root
				ASSERT(&root_);
				TAvlTreeNode* const root = root_;
				ASSERT(root);
				ASSERT(root->_right);
				if(!(root && root->_right))
					return;
				TAvlTreeNode* const pivot = root->_right;
				TAvlTreeNode* const pivot_inner_child = pivot->_left;
				pivot->_left = root;
				pivot->_parent = root->_parent;
				root->_parent = pivot;
				root->_right = pivot_inner_child;
				if(pivot_inner_child)
					pivot_inner_child->_parent = root;
				replace_subnode_if_equal_to_such_and_node_not_null(pivot->_parent, root, pivot);
				if(needHeightAndSizeRestore){
					recalculate_height_and_size_with_data_from_two_childs(root);
					recalculate_height_and_size_with_data_from_two_childs(pivot);
				}
				root_ = pivot;
			}
			void check_and_restore_balance_of_level(TAvlTreeNode*& root) BOOST_CONTAINER_NOEXCEPT {//don't setup right this->_root
				ASSERT(&root);
				ASSERT(root);
				if(!root)
					return;
				const size_type 
					balance_border = 1,
					left_height = height(root->_left),
					right_height = height(root->_right);
				if(left_height > right_height + balance_border)
					right_rotate(root);
				else if(right_height > left_height + balance_border)
					left_rotate(root);
			}
			static size_type size(const TAvlTreeNode* const node) BOOST_CONTAINER_NOEXCEPT {
				if(node)
					return node->_size;
				return 0;
			}
			static size_type height(const TAvlTreeNode* const node) BOOST_CONTAINER_NOEXCEPT {
				if(node)
					return node->_height;
				return 0;
			}
			size_type left_subtree_size(const TAvlTreeNode* const node) const BOOST_CONTAINER_NOEXCEPT {
				return this->size(node->_left);
			}
			size_type right_subtree_size(const TAvlTreeNode* const node) const BOOST_CONTAINER_NOEXCEPT {
				return this->size(node)-this->size(node->_left)-1;
			}
			size_type right_path_size(const TAvlTreeNode* const node) const BOOST_CONTAINER_NOEXCEPT {
				return this->right_subtree_size(node)-this->right_subtree_size(node->_right);
			}
			size_type left_path_size( const TAvlTreeNode* const node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				if(!node)
					return 0;
				if(!node->_left)
					return 0;
				return this->left_subtree_size(node)-left_subtree_size(node->_left);
			}
			typename TNodeType::E child_type(const TAvlTreeNode* const node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				ASSERT(this->_root!=node);
				ASSERT(node->_parent);
				if(node->_parent->_left==node)
					return TNodeType::LEFT;
				ASSERT(node->_parent->_right==node);
				return TNodeType::RIGHT;
			};
		public:
			TAvlTreeNode* forward(TAvlTreeNode* node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				ASSERT(this->_root);
				if(!node)
					return NULL;
				ASSERT(node->_parent||node==this->_root);
				if(node->_right)
					return this->find_leftmost(node->_right);
				if(!(node->_parent))
					return NULL;
				if(this->child_type(node)==TNodeType::LEFT)
					return node->_parent;
				ASSERT(this->child_type(node)==TNodeType::RIGHT);
				for(;node->_parent && this->child_type(node)==TNodeType::RIGHT;node = node->_parent);
				return node->_parent;
			}
			TAvlTreeNode* backward(TAvlTreeNode* node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				ASSERT(this->_root);
				if(!node)
					return NULL;
				ASSERT(node->_parent||node==this->_root);
				if(node->_left)
					return this->find_rightmost(node->_left);
				if(!(node->_parent))
					return NULL;
				if(this->child_type(node)==TNodeType::RIGHT)
					return node->_parent;
				ASSERT(this->child_type(node)==TNodeType::LEFT);
				for(;node->_parent && this->child_type(node)==TNodeType::LEFT;node = node->_parent);
				return node->_parent;
			}
		private:
			void restore_height_size_and_balance_of_tree(TAvlTreeNode* lowest_incorrect) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(lowest_incorrect);
				if(!lowest_incorrect)
					return;
				//if(this->size(this->_root)>1) this->selfCheckIfConstructed(this->getInternalAllocator());
				//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
				for(;lowest_incorrect->_parent;lowest_incorrect=lowest_incorrect->_parent){
					/*static size_t ii=0;
					++ii;
					if(ii==13)
						this->selfCheckIfConstructed(this->getInternalAllocator());*/
					this->recalculate_height_and_size_with_data_from_two_childs(lowest_incorrect);
					this->check_and_restore_balance_of_level(lowest_incorrect);
					/*if(ii==13)
						this->selfCheckIfConstructed(this->getInternalAllocator());*/
					//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
				}
				this->recalculate_height_and_size_with_data_from_two_childs(lowest_incorrect);
				this->check_and_restore_balance_of_level(lowest_incorrect);
				//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
				this->_root=lowest_incorrect;
			}
			TAvlTreeNode* find_rightmost(TAvlTreeNode* node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				if(!node)
					return NULL;
				for(;node->_right;node = node->_right);
				return node;
			}
			TAvlTreeNode* find_leftmost(TAvlTreeNode* node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				if(!node)
					return NULL;
				for(;node->_left;node = node->_left);
				return node;
			}
			template<class T, class F>
			static T applyNTimes(T value, const F& functor, size_type n){
				for(;n;--n)
					value = functor(value);
				return value;
			}
			void replace_node(TAvlTreeNode* const replaceable, TAvlTreeNode* const replacer) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(replaceable);
				if(!replaceable->_parent){
					ASSERT(replaceable==this->_root);
					if(replacer)
						replacer->_parent = NULL;
					this->_root = replacer;
					return;
				} 
				if(this->child_type(replaceable)==TNodeType::LEFT)
					replaceable->_parent->_left = replacer;
				else
					replaceable->_parent->_right = replacer;
				if(replacer)
					replacer->_parent = replaceable->_parent;
			}
			void delete_(TAvlTreeNode* const node, const bool needDestroyNode = true, const bool needDestroyValue = true){//don't setup right _cacheAndAllocator.begin
				ASSERT(this->_root);
				ASSERT(node);
				if(!node)
					return;
				ASSERT(this->isNotEmptyNode(node));
				if(node==this->getBeforeBegin() || node==this->getEnd())
					return;
				if(!node->_left){
					const bool node_have_parent = node->_parent!=NULL;
					this->replace_node(node, node->_right);
					if(node_have_parent)
						this->restore_height_size_and_balance_of_tree(node->_parent);
				} else {
					TAvlTreeNode* const rightmost = this->find_rightmost(node->_left);
					TAvlTreeNode* const old_parent_of_rightmost = rightmost->_parent;
					this->replace_node(rightmost, rightmost->_left);
					this->replace_node(node, rightmost);
					rightmost->_left = node->_left;
					rightmost->_right = node->_right;
					if(rightmost->_left)
						rightmost->_left->_parent = rightmost;
					if(rightmost->_right)
						rightmost->_right->_parent = rightmost;
					const bool old_parent_of_rightmost_was_deleted = old_parent_of_rightmost==node;
					TAvlTreeNode* const lowest_incorrect = old_parent_of_rightmost_was_deleted ? rightmost : old_parent_of_rightmost;
					this->restore_height_size_and_balance_of_tree(lowest_incorrect);
				}
				if(!needDestroyNode)
					return;
				BOOST_SCOPE_EXIT(node, this_){
					TAllocatorTraits::deallocate(this_->getInternalAllocator(), node, 1);
				}BOOST_SCOPE_EXIT_END
				if(needDestroyValue)
					node->destructValue();
				TAllocatorTraits::destroy(this->getInternalAllocator(), node);
			}
			void dealocateNodeIfRaisedException_Internal(TInternalAllocator& allocator, TAvlTreeNode* const node){
				if(::std::uncaught_exception())
					TAllocatorTraits::deallocate(allocator, node, 1);
			}
			TAvlTreeNode* const allocateNode_Internal(TInternalAllocator& allocator){
				return TAllocatorTraits::allocate(allocator, 1);
			}
		public:
			#define GENERATE_MAKE_NODE(_, ARG_COUNTER, __)											\
				PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)													\
				TAvlTreeNode* makeNode(PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER) ){						\
					TInternalAllocator& allocator = this->getInternalAllocator();					\
					TAvlTreeNode* const node = this->allocateNode_Internal(allocator);				\
					{																				\
						BOOST_SCOPE_EXIT(this_, &allocator, node){									\
							this_->dealocateNodeIfRaisedException_Internal(allocator, node);		\
						}BOOST_SCOPE_EXIT_END														\
						TAllocatorTraits::construct(allocator, node, construct/*,*/ PP_CALL_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER));\
					}																				\
					return node;																	\
				}
			BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_MAKE_NODE, BOOST_PP_EMPTY())
			#undef GENERATE_MAKE_NODE
			bool isNotEmptyNode(const TAvlTreeNode* const node){
				ASSERT(this->getBeforeBegin() && this->getEnd() && this->getBegin());
				return node!=this->getBeforeBegin() && node!=this->getEnd();
			}
			size_type getIndex(const TAvlTreeNode* node) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				ASSERT(this->_root);
				size_type index_accumulator = 0;
				for(;node->_parent;node=node->_parent)
					if(this->child_type(node)==TNodeType::LEFT)
						index_accumulator += this->left_path_size(node->_parent);
					else
						index_accumulator -= this->right_path_size(node->_parent);
				ASSERT(node==this->_root);
				return left_subtree_size(node) - index_accumulator;
			}
			TAvlTreeNode* getByIndex(const size_type searched_index) const BOOST_CONTAINER_NOEXCEPT {// 0 ==> _cacheAndAllocator.before_begin(); size(this->_root) ==> _cacheAndAllocator.end();
				TAvlTreeNode* current_node = this->_root;
				ASSERT(current_node);
				if(!current_node)
					return NULL;
				if(searched_index>=size(current_node))
					return NULL;
				size_type left_index_border = 0;
				size_type current_index = this->left_subtree_size(current_node);
				while(true){
					if(current_index>searched_index){
						current_node = current_node->_left;
						if(!current_node){
							SHOULD_NEVER_REACH_HERE;
							return NULL;
						}
						const size_type left_subtree_size = this->left_subtree_size(current_node);
						current_index = left_index_border + left_subtree_size;
					} else if(current_index<searched_index){
						left_index_border = left_index_border + this->left_subtree_size(current_node) + 1;
						current_node = current_node->_right;
						if(!current_node){
							SHOULD_NEVER_REACH_HERE;
							return NULL;
						}
						const size_type left_subtree_size = this->left_subtree_size(current_node);
						current_index = left_index_border + left_subtree_size;
					} else if(current_index == searched_index)
						return current_node;
					ASSERT(current_index>=left_index_border);
				}
			}
			TAvlTreeNode* insert_before(TAvlTreeNode* it, TAvlTreeNode* const new_node) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(new_node);
				if(!new_node)
					return it;
				ASSERT(new_node->_size==1);
				ASSERT(new_node->_height==1);
				new_node->_size=1;
				new_node->_height=1;
				new_node->_left=NULL;
				new_node->_right=NULL;
				if(it==this->_cacheAndAllocator.begin())
					this->_cacheAndAllocator.begin() = new_node;
				const bool firstInsertion = !(it || this->_root);
				ASSERT(firstInsertion || (this->_root && it));
				if(firstInsertion){
					this->_root = new_node;
					new_node->_parent=NULL;
					ASSERT(this->_root);
					return new_node;
				}
				ASSERT(it && this->_root);
				if(!it->_left){
					it->_left = new_node;
					new_node->_parent=it;
					//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
					this->restore_height_size_and_balance_of_tree(it);
					//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
					ASSERT(this->_root);
					return new_node;
				}
				it = this->find_rightmost(it->_left);
				it->_right = new_node;
				new_node->_parent=it;
				//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
				this->restore_height_size_and_balance_of_tree(it);
				//if(this->_cacheAndAllocator.begin() && fff())ASSERT(this->_cacheAndAllocator.begin()->_parent->_left == this->_cacheAndAllocator.begin() || this->_cacheAndAllocator.begin()->_parent->_right == this->_cacheAndAllocator.begin());
				ASSERT(this->_root);
				return new_node;
			}
			TAvlTreeNode* go_to(TAvlTreeNode* const node, difference_type stepAmount) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(node);
				const size_type absStepAmount = abs(stepAmount),
								maxLinearStepAmount = 2;
				if(absStepAmount <= maxLinearStepAmount)
					return this->applyNTimes(node, ::boost::bind((stepAmount >= 0 ? &avl_tree_stump::forward : &avl_tree_stump::backward), this, ::_1), absStepAmount);
				const size_type sourceNodeIndex = this->getIndex(node);
				const size_type destinationNodeIndex = sourceNodeIndex+stepAmount;
				return this->getByIndex(destinationNodeIndex);
			}
			TAvlTreeNode* delete_and_get_next(TAvlTreeNode* const node, const bool needDestroyNode = true, const bool needDestroyValue = true) {
				ASSERT(this->_root);
				ASSERT(node);
				ASSERT(needDestroyNode || (!needDestroyValue));
				if(!node)
					return NULL;
				ASSERT(node!=this->getBeforeBegin());
				ASSERT(node!=this->getEnd());
				TAvlTreeNode* const next = this->forward(node);
				const bool setup_begin_required=this->getBegin()==node;
				ASSERT(next!=node);
				ASSERT(next!=NULL);
				/*if(node==(void*)0x405176e4)
					int jhjkdd=6;*/
				this->delete_(node, needDestroyNode, needDestroyValue);
				if (setup_begin_required)
					this->_cacheAndAllocator.begin()=next;
				return next;
			}
			void destruct_value_and_delete_node_since_begin_to_before_end(void) {
				//TBreakPoint bp((void*)0x404d7d04);
				//this->selfCheckIfConstructed();
				for(TAvlTreeNode* it(this->_cacheAndAllocator.begin()); it!=this->_cacheAndAllocator.end()/*||(!it)*/; it=this->delete_and_get_next(it))
					/*this->selfCheckWithoutHeightAndSizeIfConstructed()*/;
				//this->selfCheckWithoutHeightAndSizeIfConstructed();
				//this->selfCheckIfConstructed();
			}
			difference_type distance(const TAvlTreeNode* const left, const TAvlTreeNode* const right) const BOOST_CONTAINER_NOEXCEPT { 
				return this->getIndex(right) - this->getIndex(left);
			}
			size_type size(void) const{
				return this->size(this->_root);
			}
			TAvlTreeNode* getNextAndRemoveNode(TAvlTreeNode* goingToRemove) BOOST_CONTAINER_NOEXCEPT {
				return this->delete_and_get_next(goingToRemove, false);
			}
			TAvlTreeNode* treeToVineWithPreserveInvariantsOnlyForMember_right(void) BOOST_CONTAINER_NOEXCEPT {//Disbalance and break invariants of root, container and nodes.
				TAvlTreeNode* current(this->find_rightmost(this->_root));
				ASSERT(current);
				for(;;){
					while(current->_left)
					{
						right_rotate(current, false);
						current = current->_right;
					}
					if(!(current->_parent))
						return current;
					current = current->_parent;
				}
				this->_root = NULL;
			}
			TAvlTreeNodeIntrusiveList moveNodes(void) BOOST_CONTAINER_NOEXCEPT {
				TAvlTreeNodeIntrusiveList result;
				TAvlTreeNode* current(this->treeToVineWithPreserveInvariantsOnlyForMember_right()), * const end_(this->getEnd());
				ASSERT(current);
				ASSERT(current==this->getBeforeBegin());
				ASSERT(!(end_->_right));
				current = current->_right;
				ASSERT(current);
				while(current!=end_){
					TAvlTreeNode* const nodeGoingToList(current);
					current = current->_right;
					ASSERT(current);
					nodeGoingToList->resetInvariant();
					result.push_back(*nodeGoingToList);
				}
				this->resetInvariant();
				return ::boost::move(result);
			}
			TAvlTreeNodeIntrusiveList moveNodes(TAvlTreeNode* const from, TAvlTreeNode* const to) BOOST_CONTAINER_NOEXCEPT {
				TAvlTreeNodeIntrusiveList result;
				TAvlTreeNode* current(from), * const end_(to);
				while(current!=end_){
					TAvlTreeNode* const nodeGoingToList(current);
					current = getNextAndRemoveNode(current);
					nodeGoingToList->resetInvariant();
					result.push_back(*nodeGoingToList);
				}
				return ::boost::move(result);
			}
			bool isConstructed(void) const {
				return this->_cacheAndAllocator.before_begin() && this->_cacheAndAllocator.end();
			}
			void selfCheck(void) const {
				if(this->isConstructed())
					this->selfCheckIfConstructed();
			}
			template<class TImplementationAllocator>
			void selfCheck(const TImplementationAllocator& allocator) const {
				if(this->isConstructed())
					this->selfCheckIfConstructed(allocator);
			}
			template<class TImplementationAllocator>
			void selfCheckAllocator(const TImplementationAllocator& allocator) const {
				ASSERT(/*TImplementationAllocator(*/this->getInternalAllocator()/**/==allocator);
			}
			template<class TImplementationAllocator>
			void selfCheckIfConstructed(const TImplementationAllocator& allocator) const {
				ASSERT(this->isConstructed());
				this->selfCheckAllocator(allocator);
				this->selfCheckIfConstructed();
			}
			void selfCheckIfConstructed(void) const {
				ASSERT(this->isConstructed());
				this->selfCheckWithoutHeightAndSizeIfConstructed();
				struct _{
					static size_type getAndCheckHeight(const TAvlTreeNode* const node){
						if(!node)
							return 0;
						const size_type real_height = 1 + (::std::max) (getAndCheckHeight(node->_left), getAndCheckHeight(node->_right));
						ASSERT(real_height==node->_height);
						return real_height;
					}
					static bool checkHeights(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(getAndCheckHeight(this_->_root)==this_->height(this_->_root));
						return true;
					}
					static size_type getAndCheckSize(const TAvlTreeNode* const node){
						if(!node)
							return 0;
						const size_type real_size = 1 + getAndCheckSize(node->_left) + getAndCheckSize(node->_right);
						ASSERT(real_size==node->_size);
						return real_size;
					}
					static bool checkSizes(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(getAndCheckSize(this_->_root)==this_->size(this_->_root));
						ASSERT(getAmount(this_)==this_->size(this_->_root));
						return true;
					}
					static size_type getAmount(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(this_->_cacheAndAllocator.begin());
						ASSERT(this_->_cacheAndAllocator.end());
						TAvlTreeNode* node=this_->_cacheAndAllocator.begin();
						size_type amount=0;
						for(;node && node!=this_->_cacheAndAllocator.end(); node = (++amount, this_->forward(node)));
						return amount+2;
					}
				};
				ASSERT(_::checkHeights(this));
				ASSERT(_::checkSizes(this));
			}
			void selfCheckWithoutHeightAndSize(void) const {
				if(this->isConstructed())
					this->selfCheckWithoutHeightAndSizeIfConstructed();
			}
			void selfCheckWithoutHeightAndSizeIfConstructed(void) const {
				ASSERT(this->isConstructed());
				ASSERT(this->_root);
				ASSERT(this->_cacheAndAllocator.before_begin());
				ASSERT(this->_cacheAndAllocator.begin());
				ASSERT(this->_cacheAndAllocator.end());
				struct _{
					static void checkChildParent(const TAvlTreeNode* const node){
						if(node->_left){
							ASSERT(node->_left->_parent==node);
							checkChildParent(node->_left);
						}
						if(node->_right){
							ASSERT(node->_right->_parent==node);
							checkChildParent(node->_right);
						}
					}
					static bool checkAllParents(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(!(this_->_root->_parent));
						checkChildParent(this_->_root);
						return true;
					}
					static bool checkCouple(const avl_tree_stump* const this_, TAvlTreeNode* node){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(node);
						if(node->_parent){
							ASSERT(node->_parent);
							ASSERT(node->_parent->_left == node || node->_parent->_right == node);
							for (;node->_parent;node = node->_parent);
							ASSERT(node==this_->_root);
							return true;
						}
						ASSERT(node==this_->_root);
						return true;
					}
					static bool checkBeforeBegin(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(this_->_cacheAndAllocator.before_begin());
						checkCouple(this_, this_->_cacheAndAllocator.before_begin());
						ASSERT(this_->_cacheAndAllocator.before_begin()==this_->find_leftmost(this_->_root));
						ASSERT(!this_->_cacheAndAllocator.before_begin()->_left);
						return true;
					}
					static bool checkEnd(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(this_->_cacheAndAllocator.before_begin());
						checkCouple(this_, this_->_cacheAndAllocator.end());
						ASSERT(this_->_cacheAndAllocator.end()==this_->find_rightmost(this_->_root));
						ASSERT(!this_->_cacheAndAllocator.end()->_right);
						return true;
					}
					static bool checkBegin(const avl_tree_stump* const this_){
						ASSERT(this_);
						ASSERT(this_->_root);
						ASSERT(this_->_cacheAndAllocator.begin());
						ASSERT(this_->_cacheAndAllocator.before_begin());
						checkCouple(this_, this_->_cacheAndAllocator.begin());
						ASSERT(this_->forward(this_->_cacheAndAllocator.before_begin())==this_->_cacheAndAllocator.begin());
						return true;
					}
				};
				ASSERT(_::checkAllParents(this));
				ASSERT(_::checkBeforeBegin(this));
				ASSERT(_::checkEnd(this));
				ASSERT(_::checkBegin(this));
			}
		};
		template<class TSource>
		class TRandomAccessIterator: public ::boost::iterator_facade<
			TRandomAccessIterator<TSource>,
			typename iterator_value_t<TSource>::type,
			::boost::random_access_traversal_tag>
		{
			typedef iterator_facade_ Base;
			friend class ::boost::iterator_core_access;
			friend class TRandomAccessIterator<TSource>;
			friend class TRandomAccessIterator<const TSource>;
			typedef TAvlTreeNode<typename TSource::value_type> TAvlTreeNode;
			typedef typename ::boost::remove_const<TSource>::type TUnconstedSource;
			TAvlTreeNode* _current;
			TSource* _collection;
			reference dereference(void) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current);
				ASSERT(this->_collection);
				ASSERT(this->_current!=this->_collection->getBeforeBegin());
				ASSERT(this->_current!=this->_collection->getEnd());
				return this->_current->getValue();
			}
			bool equal(const TRandomAccessIterator& that) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current != that._current || this->_collection == that._collection);
				return this->_current == that._current;
			}
			void increment(void) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current);
				ASSERT(this->_collection);
				ASSERT(this->_current!=this->_collection->getEnd());
				this->_current=this->_collection->forward(this->_current);
				ASSERT(this->_current);
				if(!this->_current)
					this->_current=this->_collection->getEnd();
			}
			void decrement(void) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current);
				ASSERT(this->_collection);
				ASSERT(this->_current!=this->_collection->getBegin());
				this->_current=this->_collection->backward(this->_current);
				ASSERT(this->_current);
				if(!this->_current)
					this->_current=this->_collection->getEnd();
			}
			void advance(const difference_type iteratorIncrement) BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current);
				ASSERT(this->_collection);
				ASSERT((this->_current!=this->_collection->getEnd() && iteratorIncrement>0) || iteratorIncrement<=0);
				ASSERT((this->_current!=this->_collection->getBegin() && iteratorIncrement<0) || iteratorIncrement>=0);
				this->_current=this->_collection->go_to(this->_current, iteratorIncrement);
				//if(!iteratorIncrement)
					ASSERT(this->_current);
				if(!this->_current)
					this->_current=this->_collection->getEnd();
			}
			difference_type distance_to(const TRandomAccessIterator& that) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(this->_current);
				ASSERT(this->_collection);
				ASSERT(that._current);
				ASSERT(that._collection);
				ASSERT(this->_collection==that._collection);
				return this->_collection->distance(this->_current, that._current);
			}
		public:
			typedef const value_type& const_reference;
			typedef const value_type* const_pointer;
			TRandomAccessIterator(const TRandomAccessIterator& that) BOOST_CONTAINER_NOEXCEPT : 
				_current(that._current),
				_collection(that._collection)
			{}
			TRandomAccessIterator(void) BOOST_CONTAINER_NOEXCEPT :
				_current(NULL),
				_collection(NULL)
			{}
			template <class TOtherSource>
			typename TEnableIfIsNotConstIterator<TOtherSource, TRandomAccessIterator>::type& operator=(const TRandomAccessIterator<TOtherSource>& other) BOOST_CONTAINER_NOEXCEPT {
				TRandomAccessIterator coped(other._current, *(other._collection));
				::boost::swap(coped, *this);
				return *this;
			}
			TRandomAccessIterator& operator=(TRandomAccessIterator other) BOOST_CONTAINER_NOEXCEPT {
				::boost::swap(other, *this);
				return *this;
			}
			template<class TOtherSource>
			TRandomAccessIterator(const TRandomAccessIterator<TOtherSource>& fromMutableIteratorToConstIterator,
				typename TEnableIfIsNotConstIterator<TOtherSource>::type* = NULL) BOOST_CONTAINER_NOEXCEPT : 
					_current(fromMutableIteratorToConstIterator._current),
					_collection(fromMutableIteratorToConstIterator._collection)
			{}
			TRandomAccessIterator(TAvlTreeNode* node, TSource& collection) BOOST_CONTAINER_NOEXCEPT : 
				_current(node),
				_collection(&collection)
			{}
			TRandomAccessIterator<TUnconstedSource> toMutableIterator(void) const BOOST_CONTAINER_NOEXCEPT {
				return TRandomAccessIterator<TUnconstedSource>(this->_current, *const_cast<TUnconstedSource*>(this->_collection));
			}
			void checkIteratorBelongsToCoollection(const TSource& collection) const BOOST_CONTAINER_NOEXCEPT {
				ASSERT(&collection==this->_collection);
			}
			TAvlTreeNode* getNode(void) const BOOST_CONTAINER_NOEXCEPT {
				return this->_current;
			}
			::std::size_t hash_value(void) const {
				return ::boost::hash_value(this->getNode());
			}
			friend void swap(TRandomAccessIterator& l, TRandomAccessIterator& r){
				::boost::swap(static_cast<Base&>(l), static_cast<Base&>(r));
				::boost::swap(l._current, r._current);
				::boost::swap(l._collection, r._collection);
			};
		};
	};
	
	template <class T, class TAllocator = ::std::allocator<T> >
	class random_access_list: 
		::boost::equality_comparable<random_access_list<T, TAllocator> >, 
		::boost::partially_ordered<random_access_list<T, TAllocator> >
	{
	private:
		BOOST_COPYABLE_AND_MOVABLE(random_access_list)
		typedef container_detail::avl_tree_stump<T, TAllocator> TImplementation;
		typedef typename TImplementation::TAvlTreeNode TAvlTreeNode;
		typedef typename TImplementation::TAvlTreeNodeIntrusiveList TAvlTreeNodeIntrusiveList;
		typedef typename TImplementation::TAvlTreeNodeDeleter TAvlTreeNodeDeleter;
		typedef typename TImplementation::TAllocatorTraits TAllocatorTraits;
		typedef typename TImplementation::TExternalAllocatorTraits TExternalAllocatorTraits;
		typedef typename TImplementation::TInternalAllocator TInternalAllocator;
		typedef typename TExternalAllocatorTraits::BOOST_NESTED_TEMPLATE portable_rebind_alloc<TImplementation>::type TImplementationAllocator;
		typedef ::boost::container::allocator_traits<TImplementationAllocator> TImplementationAllocatorTraits;
		typedef container_detail::TDeleter<TImplementationAllocator, TImplementationAllocatorTraits, TImplementation> TImplementatationDeleter;
		typedef typename unique_ptr_type_generator_<TImplementation, TImplementatationDeleter>::type PImplementation;
		PImplementation _implementation;
		static PImplementation makeImplementation(const TAllocator& allocator){
			TImplementationAllocator implAllocator(allocator);
			PImplementation result(TImplementationAllocatorTraits::allocate(implAllocator, 1), TImplementatationDeleter(implAllocator));
			TImplementationAllocatorTraits::construct(implAllocator, &*result, allocator);
			return ::boost::move(result);
		}
		static PImplementation makeImplementation(void){return makeImplementation(TAllocator());}
	public:
		typedef typename TImplementation::value_type value_type;
		typedef typename TImplementation::allocator_type allocator_type;
		typedef typename TImplementation::size_type size_type;
		typedef typename TImplementation::difference_type difference_type;
		typedef typename TImplementation::reference reference;
		typedef typename TImplementation::const_reference const_reference;
		typedef typename TImplementation::pointer pointer;
		typedef typename TImplementation::const_pointer const_pointer;
		typedef container_detail::TRandomAccessIterator<TImplementation> iterator;
		typedef container_detail::TRandomAccessIterator<const TImplementation> const_iterator;
		typedef ::std::reverse_iterator<iterator> reverse_iterator;
		typedef ::std::reverse_iterator<const_iterator> const_reverse_iterator;
		random_access_list(void):_implementation(makeImplementation()){}
		explicit random_access_list(const TAllocator& alloc):_implementation(makeImplementation(alloc)){}
		explicit random_access_list(size_type count):_implementation(makeImplementation()){
			this->assign(count);
		}
		random_access_list(size_type count, 
			const_reference value):_implementation(makeImplementation()){
				this->assign(count, value);
		}
		random_access_list(size_type count, 
			const_reference value,
			const TAllocator& alloc):_implementation(makeImplementation(alloc)){
				this->assign(count, value);
		}
		template< class InputIt >
		random_access_list(InputIt first, InputIt last, 
			const TAllocator& alloc, typename ::boost::enable_if<container::container_detail::TIsInputOrMoreIterator<InputIt> >::type* = NULL):_implementation(makeImplementation(alloc)){
				this->assign(first, last);
		}
		template< class InputIt >
		random_access_list(InputIt first, InputIt last, typename ::boost::enable_if<container::container_detail::TIsInputOrMoreIterator<InputIt> >::type* = NULL):_implementation(makeImplementation()){
			this->assign(first, last);
		}
		random_access_list(const random_access_list& other):_implementation(makeImplementation(other._implementation->getInternalAllocator())){
			this->assign(::boost::const_begin(other), ::boost::const_end(other));
		}
		random_access_list(const random_access_list& other, const TAllocator& alloc):_implementation(makeImplementation(alloc)){
			this->assign(::boost::const_begin(other), ::boost::const_end(other));
		}
		random_access_list(BOOST_RV_REF(random_access_list) other):_implementation(makeImplementation(other._implementation->getInternalAllocator())){
			this->swap(other);
		}
		random_access_list(BOOST_RV_REF(random_access_list) other, const TAllocator& alloc):_implementation(makeImplementation(alloc)){
			if(other._implementation->getInternalAllocator()==this->_implementation->getInternalAllocator())
				this->swap(other);
			else
				this->assign(::boost::make_move_iterator(other.begin()), ::boost::make_move_iterator(other.end()));
		}
		random_access_list& operator=(BOOST_COPY_ASSIGN_REF(random_access_list) other){
			this->assign(other.cbegin(), other.cend());
			return *this;
		}
		random_access_list& operator=(BOOST_RV_REF(random_access_list) other){
			if(other._implementation->getInternalAllocator()==this->_implementation->getInternalAllocator())
				this->swap(other);
			else
				this->assign(::boost::make_move_iterator(::boost::begin(other)), ::boost::make_move_iterator(::boost::end(other)));
			return *this;
		}
		void assign(size_type count){
			random_access_list tmp(this->_implementation->getInternalAllocator());
			for(size_type i = 0; i!=count; ++i)
				tmp.emplace_back();
			::boost::swap(tmp, *this);
		}
		void assign(size_type count, const_reference value){
			random_access_list tmp(this->_implementation->getInternalAllocator());
			for(size_type i = 0; i!=count; ++i)
				tmp.push_back(value);
			::boost::swap(tmp, *this);
		}
		template< class InputIt >
		typename ::boost::enable_if<container::container_detail::TIsInputOrMoreIterator<InputIt> >::type
			assign(InputIt first, InputIt last){ //C++ standart UB when insert first and last belonging to *this is elimnated.
			random_access_list tmp(this->_implementation->getInternalAllocator());
			::boost::push_back(tmp, ::boost::make_iterator_range(first, last));
			::boost::swap(tmp, *this);
		}
		allocator_type get_allocator() const{
			return allocator_type(this->_implementation->getInternalAllocator());
		}
		iterator begin()BOOST_CONTAINER_NOEXCEPT{
			return iterator(this->_implementation->getBegin(), *(this->_implementation));
		}
		const_iterator begin() const BOOST_CONTAINER_NOEXCEPT{
			return const_cast<random_access_list*>(this)->begin();
		}
		const_iterator cbegin() const BOOST_CONTAINER_NOEXCEPT{
			return const_cast<random_access_list*>(this)->begin();
		}
		iterator end() BOOST_CONTAINER_NOEXCEPT{
			return iterator(this->_implementation->getEnd(), *(this->_implementation));
		}
		const_iterator end() const BOOST_CONTAINER_NOEXCEPT{
			return const_cast<random_access_list*>(this)->end();
		}
		const_iterator cend() const BOOST_CONTAINER_NOEXCEPT{
			return const_cast<random_access_list*>(this)->end();
		}
		reverse_iterator rbegin() BOOST_CONTAINER_NOEXCEPT{
			return reverse_iterator(this->end());
		}
		const_reverse_iterator rbegin() const BOOST_CONTAINER_NOEXCEPT{
			return const_reverse_iterator(this->end());
		}
		const_reverse_iterator crbegin() const BOOST_CONTAINER_NOEXCEPT{
			return const_reverse_iterator(this->end());
		}
		reverse_iterator rend() BOOST_CONTAINER_NOEXCEPT {
			return reverse_iterator(this->begin());
		}
		const_reverse_iterator rend() const BOOST_CONTAINER_NOEXCEPT {
			return const_reverse_iterator(this->begin());
		}
		const_reverse_iterator crend() const BOOST_CONTAINER_NOEXCEPT {
			return const_reverse_iterator(this->begin());
		}
		bool operator==(const random_access_list<T, TAllocator>& r) const{
			return ::boost::equal(*this, r);
		}
		bool operator<(const random_access_list<T, TAllocator>& r) const{
			return ::boost::range::lexicographical_compare(*this, r);
		}
		::std::size_t hash_value(void) const{
			return ::boost::hash_range(::boost::const_begin(*this), ::boost::const_end(*this));
		}
		#define GENERATE_EMPLACE(_, ARG_COUNTER, __)										\
			PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)												\
			iterator emplace(const_iterator it/*,*/ PP_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) ){\
				it.checkIteratorBelongsToCoollection(*(this->_implementation));				\
				TAvlTreeNode* const node = this->_implementation->makeNode(PP_CALL_FUNCTION_FORWARD_ARGS(ARG_COUNTER) );\
				return this->insertNode(it, node);											\
			}
		BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_EMPLACE, BOOST_PP_EMPTY())
		#undef GENERATE_EMPLACE
		#define GENERATE_EMPLACE_BACK(_, ARG_COUNTER, __)									\
			PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)												\
			iterator emplace_back(PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER) ){					\
				return this->emplace(this->cend()/*,*/ PP_CALL_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) );\
			}
		BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_EMPLACE_BACK, BOOST_PP_EMPTY())
		#undef GENERATE_EMPLACE_BACK
		#define GENERATE_EMPLACE_FRONT(_, ARG_COUNTER, __)									\
			PP_FULL_TEMPLATE_ARGS(ARG_COUNTER)												\
			iterator emplace_front(PP_FUNCTION_FORWARD_ARGS(ARG_COUNTER) ){					\
				return this->emplace(this->cbegin()/*,*/ PP_CALL_FUNCTION_FORWARD_TRAILING_ARGS(ARG_COUNTER) );\
			}
		BOOST_PP_REPEAT(AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER, GENERATE_EMPLACE_FRONT, BOOST_PP_EMPTY())
		#undef GENERATE_EMPLACE_FRONT
		template<class T>//Wrong implementation and interface. To do: rewrite.
		iterator insert( const_iterator pos, BOOST_FWD_REF(T) value ){
			return this->emplace(pos, ::boost::forward<T>(value));
		}
		template<class T>//Wrong implementation and interface. To do: rewrite.
		iterator push_back(BOOST_FWD_REF(T) value ){
			return this->emplace_back(::boost::forward<T>(value));
		}
		template<class T>//Wrong implementation and interface. To do: rewrite.
		iterator push_front(BOOST_FWD_REF(T) value ){
			return this->emplace_front(::boost::forward<T>(value));
		}
		iterator insert( const_iterator pos, size_type count, const_reference value ){
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			TAvlTreeNodeIntrusiveList constructedNodesForStrongExeptionSafety;
			{
				BOOST_SCOPE_EXIT(&constructedNodesForStrongExeptionSafety, this_){
					if(::std::uncaught_exception())
						constructedNodesForStrongExeptionSafety.clear_and_dispose(TAvlTreeNodeDeleter(this_->_implementation->getInternalAllocator(), &container::container_detail::exactly_destroy_node_value<value_type>));
				}BOOST_SCOPE_EXIT_END
				for(;count>0;--count)
					constructedNodesForStrongExeptionSafety.push_back(*(this->_implementation->makeNode(value)));
			}
			return this->insertNodesFromListAndReturnFirstInserted(constructedNodesForStrongExeptionSafety, pos);
		}
		template< class InputIt >
		iterator insert( const_iterator pos, InputIt first, InputIt last ){//C++ standart UB when insert first and last belonging to *this is elimnated.
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			TAvlTreeNodeIntrusiveList constructedNodesForStrongExeptionSafety;
			{
				BOOST_SCOPE_EXIT(&constructedNodesForStrongExeptionSafety, this_){
					if(::std::uncaught_exception())
						constructedNodesForStrongExeptionSafety.clear_and_dispose(TAvlTreeNodeDeleter(this_->_implementation->getInternalAllocator(), &container::container_detail::exactly_destroy_node_value<value_type>));
				}BOOST_SCOPE_EXIT_END
				while(first!=last)
					constructedNodesForStrongExeptionSafety.push_back(*(this->_implementation->makeNode(*(first++))));
			}
			return this->insertNodesFromListAndReturnFirstInserted(constructedNodesForStrongExeptionSafety, pos);
		}
		void clear(void){
			this->_implementation->destruct_value_and_delete_node_since_begin_to_before_end();
		}
		size_type max_size() const {
			return TAllocatorTraits::max_size(this->_implementation->getInternalAllocator());
		}
		size_type size() const BOOST_CONTAINER_NOEXCEPT {
			enum {GAP_NODE_AMOUNT = 2};
			const size_type avl_tree_size(this->_implementation->size());
			ASSERT(avl_tree_size>=GAP_NODE_AMOUNT);
			return avl_tree_size - GAP_NODE_AMOUNT;
		}
		size_type algorithmic_complexity() const BOOST_CONTAINER_NOEXCEPT {
			return this->_implementation->algorithmic_complexity();
		}
		bool empty() const BOOST_CONTAINER_NOEXCEPT {
			return this->size()==0;
		}
		reference front() BOOST_CONTAINER_NOEXCEPT {
			ASSERT(!this->empty());
			return *this->begin();
		}
		const_reference front() const BOOST_CONTAINER_NOEXCEPT {
			ASSERT(!this->empty());
			return *this->begin();
		}
		reference back() BOOST_CONTAINER_NOEXCEPT {
			ASSERT(!this->empty());
			return *::boost::prev(this->end());
		}
		const_reference back() const BOOST_CONTAINER_NOEXCEPT {
			ASSERT(!this->empty());
			return *::boost::prior(this->end());
		}
		void resize( size_type count ){
			const size_type container_size = this->size();
			if(count>=container_size)
				this->insert(this->cbegin(), count-container_size, value_type());
			else
				for(size_type goingToDelete = container_size - count;goingToDelete>0;--goingToDelete)
					this->pop_back();
		}
		void resize( size_type count, const_reference value ){
			const size_type container_size = this->size();
			if(count>=container_size)
				this->insert(this->cbegin(), count-container_size, value);
			else
				for(size_type goingToDelete = container_size - count;goingToDelete>0;--goingToDelete)
					this->pop_back();
		}
		void swap( random_access_list& other ){
			::boost::swap(this->_implementation, other._implementation);
		}
		reference operator[]( size_type pos ) BOOST_CONTAINER_NOEXCEPT {
			ASSERT(pos<this->size());
			TAvlTreeNode* const result = this->_implementation->getByIndex(pos+1);
			ASSERT(result);
			ASSERT(this->_implementation->isNotEmptyNode(result));
			return result->getValue();
		}
		const_reference operator[]( size_type pos ) const BOOST_CONTAINER_NOEXCEPT {
			return (*const_cast<random_access_list*>(this))[pos];
		}
		reference at( size_type pos ){
			if(pos<this->size())
				throw ::std::out_of_range();
			else
				return (*this)[pos];
		}
		const_reference at( size_type pos ) const{
			return const_cast<random_access_list*>(this)->at(pos);
		}
		void pop_back(){
			ASSERT(!this->empty());
			this->erase(::boost::prior(this->cend()));
		}
		void pop_front(){
			ASSERT(!this->empty());
			this->erase(this->cbegin());
		}
		iterator erase( const_iterator pos ){
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			ASSERT(pos.getNode()!=this->cend().getNode());
			ASSERT(pos.getNode()!=this->_implementation->getBeforeBegin());
			return iterator(this->_implementation->delete_and_get_next(pos.getNode()), *(this->_implementation));
		}
		iterator erase( const_iterator first, const_iterator last ){
			for(; first!=last; first = this->erase(first));
			return iterator(first.getNode(), *(this->_implementation));
		}
		//Iterator validity violated for moved elements at splice function!!!!:
		void splice( const_iterator pos, BOOST_RV_REF(random_access_list) other ) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n)
			this->splice_private(pos, other);}
		void splice( const_iterator pos, random_access_list& other ) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n) 
			this->splice_private(pos, other);}
		void splice( const_iterator pos, BOOST_RV_REF(random_access_list) other , const_iterator it ) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n)
			this->splice_private(pos, other, it);}
		void splice( const_iterator pos, random_access_list& other , const_iterator it ) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n)
			this->splice_private(pos, other, it);}
		void splice( const_iterator pos, BOOST_RV_REF(random_access_list) other, const_iterator first, const_iterator last) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n)
			this->splice_private(pos, other, first, last);}
		void splice( const_iterator pos, random_access_list& other, const_iterator first, const_iterator last) BOOST_CONTAINER_NOEXCEPT { //AC: O(n*log n)
			this->splice_private(pos, other, first, last);}
		void remove( const T& value ){
			using namespace ::boost::phoenix::arg_names;
			return this->remove_if(arg1==::boost::phoenix::cref(value));
		}
		template< class UnaryPredicate >
		void remove_if( UnaryPredicate p ){
			for(const_iterator current(this->cbegin()), end_(this->cend());current!=end_;)
				if(p(*current))
					current = this->erase(current);
		}
		void merge( random_access_list& other )/*не уверен BOOST_CONTAINER_NOEXCEPT*/{//AC: O(n*log n)
			this->merge_private(other);}
		void merge( BOOST_RV_REF(random_access_list) other )/*не уверен BOOST_CONTAINER_NOEXCEPT*/{//AC: O(n*log n)
			this->merge_private(other);}
		template <class Comparator> //BOOST_NOEXCEPT_OR_NOTHROW
		void merge( random_access_list& other, Comparator comp )BOOST_CONTAINER_NOEXCEPT_IF_BINARY_PREDICATE_NOEXCEPT(comp) {//AC: O(n*log n)
			this->merge_private(other, comp);}
		template <class Comparator> 
		void merge( BOOST_RV_REF(random_access_list) other, Comparator comp )BOOST_CONTAINER_NOEXCEPT_IF_BINARY_PREDICATE_NOEXCEPT(comp) {//AC: O(n*log n)
			this->merge_private(other, comp);}
		void reverse()BOOST_CONTAINER_NOEXCEPT{//AC: O(n*log n)//is possible to optimize up to O(n) by swap all left and right nodes at tree and preserve position of gap-nodes (before_begin() and end() nodes) (e.g. remove gap-nodes, than reverse tree and paste gap-nodes after all).
			TAvlTreeNodeIntrusiveList nodes = this->_implementation->moveNodes();
			ASSERT(this->empty());
			nodes.reverse();
			(void)this->insertNodesFromListAndReturnFirstInserted(nodes, this->begin());
		}
		void unique(){
			using namespace ::boost::phoenix::arg_names;
			this->unique(arg1==arg2);
		}
		template< class BinaryPredicate >
		void unique( BinaryPredicate p ){
			random_access_list::const_iterator 
				current(this->cbegin()),
				checked(this->cbegin()),
				last(this->cend());
			if(checked==last)
				return;
			++checked;
			while(checked!=last){
				ASSERT(current!=last);
				ASSERT(current!=checked);
				//ASSERT(current==::boost::prior(checked));
				//ASSERT(checked==::boost::next(current));
				if(p(*checked, *current))
					checked=this->erase(checked);
				else
				{
					++checked;
					++current;
				}
			}
			ASSERT(current!=last);
			ASSERT(current!=checked);
			//ASSERT(current==::boost::prior(checked));
			//ASSERT(checked==::boost::next(current));
		}
		void sort()/*не уверен BOOST_CONTAINER_NOEXCEPT, проверь ::boost::phoenix*/{
			using namespace ::boost::phoenix::arg_names;
			this->sort(arg1<arg2);
		}
		template< class Compare > 
		void sort( Compare comp )BOOST_CONTAINER_NOEXCEPT_IF_BINARY_PREDICATE_NOEXCEPT(comp){
			TAvlTreeNodeIntrusiveList nodes = this->_implementation->moveNodes();
			ASSERT(this->empty());
			nodes.sort(comp);
			(void)this->insertNodesFromListAndReturnFirstInserted(nodes, this->begin());
		}
		TInternalAllocator& getInternalAllocator(void) BOOST_CONTAINER_NOEXCEPT {
			return this->_implementation.getInternalAllocator();
		}
		const TInternalAllocator& getInternalAllocator(void) const BOOST_CONTAINER_NOEXCEPT {
			return this->_implementation.getInternalAllocator();
		}
		BOOST_STATIC_CONSTANT(size_type, NODE_SIZE=sizeof(TAvlTreeNode));
		BOOST_STATIC_CONSTANT(size_type, IMPLEMENTATION_SIZE=sizeof(TImplementation));
		BOOST_STATIC_CONSTANT(size_type, MAX_SIZE_FOR_ALLOCATOR=(typename ::boost::mpl::max<::boost::mpl::size_t<NODE_SIZE>, ::boost::mpl::size_t<IMPLEMENTATION_SIZE> >::type::value));
		void selfCheck(void)const{
			ASSERT(this->_implementation);
			ASSERT(this->_implementation.get());
			this->_implementation->selfCheckIfConstructed(this->_implementation.get_deleter().getInternalAllocator());
		}
	private:
		friend void swap(random_access_list& l, random_access_list& r){
			l.swap(r);
		}
		iterator insertNode(const_iterator it, TAvlTreeNode* const value)BOOST_CONTAINER_NOEXCEPT {
			it.checkIteratorBelongsToCoollection(*(this->_implementation));
			return iterator(this->_implementation->insert_before(it.getNode(), value), *(this->_implementation));
		}
		void splice_private( const_iterator pos, random_access_list& other )BOOST_CONTAINER_NOEXCEPT{
			ASSERT(this!=&other);
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			ASSERT(this->_implementation->getInternalAllocator()==other._implementation->getInternalAllocator());
			TAvlTreeNodeIntrusiveList nodes = other._implementation->moveNodes();
			ASSERT(other.empty());
			(void)this->insertNodesFromListAndReturnFirstInserted(nodes, pos);
		}
		void splice_private( const_iterator pos, random_access_list& other, const_iterator it )BOOST_CONTAINER_NOEXCEPT{
			ASSERT(this!=&other);
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			it.checkIteratorBelongsToCoollection(*(other._implementation));
			ASSERT(this->_implementation->getInternalAllocator()==other._implementation->getInternalAllocator());
			this->splice_private(pos, other, it, other.end());
		}
		void splice_private( const_iterator pos, random_access_list& other, const_iterator first, const_iterator last)BOOST_CONTAINER_NOEXCEPT{
			ASSERT(this!=&other);
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			first.checkIteratorBelongsToCoollection(*(other._implementation));
			last.checkIteratorBelongsToCoollection(*(other._implementation));
			ASSERT(this->_implementation->getInternalAllocator()==other._implementation->getInternalAllocator());
			TAvlTreeNodeIntrusiveList nodes = other._implementation->moveNodes(first.getNode(), last.getNode());
			(void)this->insertNodesFromListAndReturnFirstInserted(nodes, pos);
		}
		iterator insertNodesFromListAndReturnFirstInserted(TAvlTreeNodeIntrusiveList& nodes/*the nodes going to destroy invariant after the method call*/, const_iterator pos) BOOST_CONTAINER_NOEXCEPT {
			pos.checkIteratorBelongsToCoollection(*(this->_implementation));
			TAvlTreeNodeIntrusiveList::iterator 
				node (nodes.begin()),
				end_ (nodes.end());
			const_iterator firstInserted (pos);
			bool firstInsertedAlready = false;
			while(node!=end_){
				const_iterator lastInserted = this->insertNode(pos, &*(node++));
				if (!firstInsertedAlready){
					firstInserted = lastInserted;
					firstInsertedAlready = true;
				}
			}
			return firstInserted.toMutableIterator();
		}
		void merge_private( random_access_list& other) /*не уверен BOOST_CONTAINER_NOEXCEPT, проверь ::boost::phoenix*/ {
			using namespace ::boost::phoenix::arg_names;
			this->merge_private(other, (arg1<arg2));
		}
		template <class Comparator> 
		void merge_private( random_access_list& other, Comparator comp ) BOOST_CONTAINER_NOEXCEPT {
			if(this==&other)
				return;
			ASSERT(this->_implementation->getInternalAllocator()==other._implementation->getInternalAllocator());
			TAvlTreeNodeIntrusiveList nodes0 = this->_implementation->moveNodes();
			TAvlTreeNodeIntrusiveList nodes1 = other._implementation->moveNodes();
			TAvlTreeNodeIntrusiveList nodesMerged;
			//::boost::range::merge(nodes0, nodes1, ::std::back_inserter(nodesMerged), comp);
			TAvlTreeNodeIntrusiveList::iterator first1(nodes0.begin()), last1(nodes0.end()), first2(nodes1.begin()), last2(nodes1.end());
			while (true) {
				if (first1==last1) {
					while (first2!=last2)
						nodesMerged.push_back(*(first2++));
					break;
				}
				if (first2==last2) {
					while (first1!=last1)
						nodesMerged.push_back(*(first1++));
					break;
				}
				nodesMerged.push_back(comp(*first2,*first1) ? *(first2++) : *(first1++));
			}
			ASSERT(this->empty());
			ASSERT(other.empty());
			(void)this->insertNodesFromListAndReturnFirstInserted(nodesMerged, this->begin());
		}
	};
}
#undef AMOUNT_OF_EMPLACE_CONSTRUCTOR_PARAMETER
#undef BOOST_CONTAINER_NOEXCEPT_IF_BINARY_PREDICATE_NOEXCEPT

namespace std {
	namespace tr1 {
		template <class T, class Allocator>
		struct hash<::container::random_access_list<T, Allocator> >: ::std::unary_function<::container::random_access_list<T, Allocator>, size_t >{
			::std::size_t operator()(const ::container::random_access_list<T, Allocator> &x ) const{
				return x.hash_value();
			}
		};
		
		template <class TSource>
		struct hash<::container::container_detail::TRandomAccessIterator<TSource> >: ::std::unary_function<::container::container_detail::TRandomAccessIterator<TSource>, size_t >{
			::std::size_t operator()(const ::container::container_detail::TRandomAccessIterator<TSource> &x ) const{
				return x.hash_value();
			}
		};
		template <class TSource>
		struct hash<::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > >: ::std::unary_function<::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > , size_t >{
			::std::size_t operator()(const ::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > &x ) const{
				return x.base().hash_value();
			}
		};
		template <class TSource>
		struct hash<::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > >: ::std::unary_function<::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > , size_t >{
			::std::size_t operator()(const ::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > &x ) const{
				return x.base().hash_value();
			}
		};
	}
	/*template <class T, class Allocator>
	struct hash<::container::random_access_list<T, Allocator> >: ::std::unary_function<::container::random_access_list<T, Allocator>, size_t >{
		::std::size_t operator()(const ::container::random_access_list<T, Allocator> &x ) const{
			return x.hash_value();
		}
	};

	template <class TSource>
	struct hash<::container::container_detail::TRandomAccessIterator<TSource> >: ::std::unary_function<::container::container_detail::TRandomAccessIterator<TSource>, size_t >{
		::std::size_t operator()(const ::container::container_detail::TRandomAccessIterator<TSource> &x ) const{
			return x.hash_value();
		}
	};
	
	template <class TSource>
	struct hash<::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > >: ::std::unary_function<::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > , size_t >{
		::std::size_t operator()(const ::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> > &x ) const{
			return x.base().hash_value();
		}
	};
	
	template <class TSource>
	struct hash<::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > >: ::std::unary_function<::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > , size_t >{
		::std::size_t operator()(const ::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> > &x ) const{
			return x.base().hash_value();
		}
	};*/
}

namespace boost{
	template<class T, class A>
	::std::size_t hash_value(const ::container::random_access_list<T, A>& b){
		return b.hash_value();
	}
	template<class TSource>
	::std::size_t hash_value(const ::container::container_detail::TRandomAccessIterator<TSource>& b){
		return b.hash_value();
	}
	template<class TSource>
	::std::size_t hash_value(const ::std::reverse_iterator<::container::container_detail::TRandomAccessIterator<TSource> >& b){
		return b.base().hash_value();
	}
	template<class TSource>
	::std::size_t hash_value(const ::std::reverse_iterator<const ::container::container_detail::TRandomAccessIterator<TSource> >& b){
		return b.base().hash_value();
	}
};
#endif // random_access_list_h__