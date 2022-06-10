#pragma once

/**
 * An implementation of Euler tour tree described by Henzinger and King (J. ACM, 1999).
 * This implementation doesn't augment the number of active occurrences.
 */


#include <iterator>
#include "AVLSequence.hpp"

namespace Snu {
namespace Cnrc {
namespace EulerTourTree {
	
class EulerTourTreeModule {
	
public:
	
	using Size = unsigned int;
	
private:
	
	template<class S, class T>
	using SequenceNodeTmpl = AVLSequence::AVLSequenceNodeMixin<S, T>;
	
	template<class T>
	using SequenceNodeTraitTmpl = AVLSequence::AVLSequenceNodeTrait<T>;
	
	template<class T>
	using SequenceAlgorithmTmpl = AVLSequence::AVLSequenceAlgorithm<T>;
	
public:
	
	class EulerTourTreeNode;
	class EulerTourTreeEdge;
	
private:
	
	class Occurrence : public SequenceNodeTmpl<Occurrence, Size> {
		
	public:
		
		Occurrence(EulerTourTreeNode* const n, bool a)
		: node(n), leftEdge(nullptr), rightEdge(nullptr), isActive(a) {
		}
		
		Occurrence(const Occurrence&) = delete;
		
	public:
		
		Occurrence& operator=(const Occurrence&) = delete;
		
		Occurrence& operator=(Occurrence&&) = delete;
		
		bool operator==(const Occurrence& other) const {
			return this == &other;
		}
		
		bool operator!=(const Occurrence& other) const {
			return this != &other;
		}
		
	public:
		
		EulerTourTreeNode* const node;
		EulerTourTreeEdge* leftEdge;
		EulerTourTreeEdge* rightEdge;
		bool isActive;
	};
	
public:
	
	template<class Node, class Edge>
	class EulerTourTreeAlgorithm {
		
	public:
		
		using Size = EulerTourTreeModule::Size;
	
	private:
		
		using SequenceAlgorithm = SequenceAlgorithmTmpl<SequenceNodeTraitTmpl<Occurrence>>;
		
	private:
	
		template<class CEdge, class NodeSequenceItr>
		class EdgeIteratorImpl : public std::iterator<std::forward_iterator_tag, CEdge> {
			
		private:
			
			using This = EdgeIteratorImpl<CEdge, NodeSequenceItr>;
			
		public:
			
			EdgeIteratorImpl(NodeSequenceItr i, NodeSequenceItr e)
			: backend(i), end(e) {
				if(i != e) {
					operator++();
				}
			}
			
		public:
			
			This& operator++() {
				++backend;
				while(backend != end &&
					  (*backend->leftEdge->occurrence2 != *backend)
				) {
					++backend;
				}
				return *this;
			}
			
			This operator++(int) {
				This tmp(*this);
				operator++();
				return tmp;
			}
			
			bool operator==(const This& other) const {
				return backend == other.backend;
			}
			
			bool operator!=(const This& other) const {
				return backend != other.backend;
			}
			
			CEdge& operator*() const {
				return static_cast<Edge&>(*(backend->leftEdge));//safe
			}
			
			CEdge* operator->() const {
				return static_cast<Edge*>(backend->leftEdge);//safe
			}
			
		private:
			NodeSequenceItr backend;
			NodeSequenceItr end;
		};
		
		
		template<class CNode>
		class NodeIteratorImpl : public std::iterator<std::forward_iterator_tag, CNode> {
		
		private:
			
			using This = NodeIteratorImpl<CNode>;
			
		public:
			
			NodeIteratorImpl() 
			: p(nullptr), q(nullptr) {
			}
			
			NodeIteratorImpl(Occurrence* op, Occurrence* oq)
			: p(op), q(oq) {
				if(! q) {
					while(p && ! p->isActive) {
						stepForward();
					}
				}
			}
			
		public:
			
			This& operator++() {
				stepForward();
				while(p && ! p->isActive) {
					stepForward();
				}
				return *this;
			}
			
			This operator++(int) {
				This tmp(*this);
				operator++();
				return tmp;
			}
			
			bool operator==(const This& other) const {
				return p == other.p && q == other.q;
			}
			
			bool operator!=(const This& other) const {
				return p != other.p || q != other.q;
			}
			
			CNode& operator*() const {
				return static_cast<Node&>(*p->node);//safe
			}
			
			CNode* operator->() const {
				return static_cast<Node*>(p->node);//safe
			}
			
		private:
			void stepForward() {
				if(! p) {
					p = q;
					q = nullptr;
				} else if(auto r = SequenceAlgorithm::next(*p)) {
					p = &(r->get());
				} else {
					q = p;
					p = nullptr;
				}
			}
		
		private:
			
			Occurrence* p;
			Occurrence* q;
		};
		
	public:
		
		class NodeContainerView {
			
		public:
			
			using iterator = NodeIteratorImpl<Node>;
			using const_iterator = NodeIteratorImpl<const Node>;
			
		public:
			explicit NodeContainerView()
			: root(nullptr) {
			}
			
			explicit NodeContainerView(Occurrence& root)
			: root(&root) {
			}
			
		public:
			
			iterator begin() {
				return iterator(&SequenceAlgorithm::findHead(*root), nullptr);
			}
			
			iterator end() {
				return iterator(nullptr, &SequenceAlgorithm::findTail(*root));
			}
			
			const_iterator begin() const {
				return const_iterator(&SequenceAlgorithm::findHead(*root), nullptr);
			}
			
			const_iterator end() const {
				return const_iterator(nullptr, &SequenceAlgorithm::findTail(*root));
			}
			
			const_iterator cbegin() const {
				return const_iterator(&SequenceAlgorithm::findHead(*root), nullptr);
			}
			
			const_iterator cend() const {
				return const_iterator(nullptr, &SequenceAlgorithm::findTail(*root));
			}
			
		private:
			
			Occurrence* root;
		};
		
		class ConstNodeContainerView {
			
		public:
			
			using const_iterator = NodeIteratorImpl<const Node>;
			
		public:
			
			explicit ConstNodeContainerView(Occurrence& root)
			: root(root) {
			}
			
		public:
			
			const_iterator begin() const {
				return const_iterator(&SequenceAlgorithm::findHead(root), nullptr);
			}
			
			const_iterator end() const {
				return const_iterator(nullptr, &SequenceAlgorithm::findTail(root));
			}
			
			const_iterator cbegin() const {
				return const_iterator(&SequenceAlgorithm::findHead(root), nullptr);
			}
			
			const_iterator cend() const {
				return const_iterator(nullptr, &SequenceAlgorithm::findTail(root));
			}
			
		private:
			
			Occurrence& root;
		};
		
		class EdgeContainerView {
			
		private:
			
			using SequenceContainerView = SequenceAlgorithm::ContainerView;
			using SequenceNodeIterator = SequenceContainerView::iterator;
			using ConstSequenceNodeIterator = SequenceContainerView::const_iterator;
			
		public:
			
			using iterator = EdgeIteratorImpl<Edge, SequenceNodeIterator>;
			using const_iterator = EdgeIteratorImpl<const Edge, ConstSequenceNodeIterator>;
			
		public:
			
			explicit EdgeContainerView(SequenceContainerView b)
			: backend(b) {
			}
			
		public:
			
			iterator begin() {
				return iterator(backend.begin(), backend.end());
			}
			
			iterator end() {
				return iterator(backend.end(), backend.end());
			}
			
			const_iterator begin() const {
				return const_iterator(backend.cbegin(), backend.cend());
			}
			
			const_iterator end() const {
				return const_iterator(backend.cend(), backend.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(backend.cbegin(), backend.cend());
			}
			
			const_iterator cend() const {
				return const_iterator(backend.cend(), backend.cend());
			}
			
			
		private:
			
			SequenceContainerView backend;
		};
		
		class ConstEdgeContainerView {
			
		private:
			
			using SequenceContainerView = SequenceAlgorithm::ContainerView;
			using ConstSequenceNodeIterator = SequenceContainerView::const_iterator;
			
		public:
			
			using const_iterator = EdgeIteratorImpl<const Edge, ConstSequenceNodeIterator>;
			
		public:
			
			explicit ConstEdgeContainerView(SequenceContainerView b)
			: backend(b) {
			}
			
		public:
			
			const_iterator begin() const {
				return const_iterator(backend.begin(), backend.end());
			}
			
			const_iterator end() const {
				return const_iterator(backend.end(), backend.end());
			}
			
			const_iterator cbegin() const {
				return const_iterator(backend.cbegin(), backend.end());
			}
			
			const_iterator cend() const {
				return const_iterator(backend.cend(), backend.end());
			}
			
		private:
			
			SequenceContainerView backend;
		};
		
	public:
		
		/**
		 * NodeContainerView has container-like interface for the nodes in the cluster of 'n'.
		 */
		static NodeContainerView nodeContainerView(Node& n) {
			return NodeContainerView(
				SequenceAlgorithm::findRoot(
					*archetype(n).activeOccurrence
				)
			);
		}
		
		/**
		 * ConstNodeContainerView has container-like interface for the nodes in the cluster of 'n'.
		 */
		static ConstNodeContainerView nodeContainerView(const Node& n) {
			return ConstNodeContainerView(
				SequenceAlgorithm::findRoot(
					*archetype(n).activeOccurrence
				)
			);
		}
		
		/**
		 * EdgeContainerView has container-like interface for the edges in the the spanning tree 
		 * of the cluster of 'n'.
		 */
		static EdgeContainerView edgeContainerView(Node& n) {
			return EdgeContainerView(
				SequenceAlgorithm::containerView(
					SequenceAlgorithm::findRoot(
						*archetype(n).activeOccurrence
					)
				)
			);
		}
		
		/**
		 * EdgeContainerView has container-like interface for the edges in the the spanning tree 
		 * of the cluster of 'n'.
		 */
		static ConstEdgeContainerView edgeContainerView(const Node& n) {
			return ConstEdgeContainerView(
				SequenceAlgorithm::containerView(
					SequenceAlgorithm::findRoot(
						*archetype(n).activeOccurrence
					)
				)
			);
		}
		
		static void createEdge(Node& n1, Node& n2, Edge& e) {
			createArchetypeEdge(n1, n2, e);
		}
		
		static void deleteEdge(Edge& e) {
			deleteArchetypeEdge(e);
		}
		
		static bool hasPath(const Node& n1, const Node& n2) {
			return 
				SequenceAlgorithm::findRoot(*archetype(n1).activeOccurrence) ==
				SequenceAlgorithm::findRoot(*archetype(n2).activeOccurrence);
		}
		
		static bool isClusterRep(const Node& n) {
			return &n == &findClusterRep(n);
		}
		
		static Node& findClusterRep(Node& n) {
			return static_cast<Node&>(//safe
				*SequenceAlgorithm::findRoot(
					*archetype(n).activeOccurrence
				).node
			);
		}
		
		static const Node& findClusterRep(const Node& n) {
			return static_cast<const Node&>(//safe
				*SequenceAlgorithm::findRoot(
					*archetype(n).activeOccurrence
				).node
			);
		}
		
		static Size clusterSize(const Node& n) {
			Occurrence& r = SequenceAlgorithm::findRoot(
				*archetype(n).activeOccurrence
			);
			return (SequenceAlgorithm::size(r) + 1) / 2;
		}
		
	private:
		
		static EulerTourTreeNode& archetype(Node& n) {
			return static_cast<EulerTourTreeNode&>(n);
		}
		
		static const EulerTourTreeNode& archetype(const Node& n) {
			return static_cast<const EulerTourTreeNode&>(n);
		}
		
		static EulerTourTreeEdge& archetype(Edge& n) {
			return static_cast<EulerTourTreeEdge&>(n);
		}
		
		static void createArchetypeEdge(EulerTourTreeNode& n1, EulerTourTreeNode& n2, EulerTourTreeEdge& e) {
			Occurrence& o1h = makeHead(n1);
			Occurrence& o1r = SequenceAlgorithm::findRoot(o1h);
			Occurrence& o1t = SequenceAlgorithm::findTail(o1r);
			Occurrence& o2h = makeHead(n2);
			Occurrence& o2r = SequenceAlgorithm::findRoot(o2h);
			Occurrence& o2t = SequenceAlgorithm::findTail(o2r);
			SequenceAlgorithm::join(o1t, o2h);
			Occurrence& ont = *(new Occurrence(static_cast<Node*>(&n1), false));
			SequenceAlgorithm::insertNodeAfter(o2t, ont);
			o1t.rightEdge = &e;
			o2h.leftEdge  = &e;
			e.occurrence1 = &o1t;
			e.occurrence2 = &o2h;
			ont.leftEdge  = &e;
			o2t.rightEdge = &e;
			e.occurrence3 = &o2t;
			e.occurrence4 = &ont;
		}
		
		static void deleteArchetypeEdge(EulerTourTreeEdge& e) {
			SequenceAlgorithm::splitAfter(*e.occurrence1);
			SequenceAlgorithm::splitAfter(*e.occurrence3);
			if( SequenceAlgorithm::findRoot(*e.occurrence1) == 
				SequenceAlgorithm::findRoot(*e.occurrence4)
			) {
				join(*e.occurrence3, *e.occurrence2);
				e.occurrence4->leftEdge  = nullptr;
				e.occurrence1->rightEdge = nullptr;
			} else {
				join(*e.occurrence1, *e.occurrence4);
				e.occurrence2->leftEdge  = nullptr;
				e.occurrence3->rightEdge = nullptr;
			}
			e.occurrence1 = nullptr;
			e.occurrence2 = nullptr;
			e.occurrence3 = nullptr;
			e.occurrence4 = nullptr;
		}
		
		static void join(Occurrence& p, Occurrence& q) {
			if(auto pp = SequenceAlgorithm::previous(p)) {
				SequenceAlgorithm::removeNode(p);
				SequenceAlgorithm::join(*pp, q);
				putOccurrencOnEdge(*pp, q);
			}
			if(p.isActive) {
				p.node->activeOccurrence = &q;
				q.isActive = true;
			}
			delete &p;
		};
		
		static Occurrence& makeHead(EulerTourTreeNode& n) {
			Occurrence& newHead = *n.activeOccurrence;
			Occurrence& oldRoot = SequenceAlgorithm::findRoot(newHead);
			Occurrence& oldHead = SequenceAlgorithm::findHead(oldRoot);
			if(oldHead.node == &n) {
				return oldHead;
			}
			Occurrence& oldTail = SequenceAlgorithm::findTail(oldRoot);
			Occurrence& leftOfOldTail = *SequenceAlgorithm::previous(oldTail);
			Occurrence& leftOfNewTail = *SequenceAlgorithm::previous(newHead);
			Occurrence& newTail = *(new Occurrence(static_cast<Node*>(&n), false));
			SequenceAlgorithm::splitBefore(newHead);
			SequenceAlgorithm::removeNode(oldTail);
			SequenceAlgorithm::join(leftOfOldTail, oldHead);
			SequenceAlgorithm::insertNodeAfter(leftOfNewTail, newTail);
			newHead.leftEdge = nullptr;
			putOccurrencOnEdge(leftOfOldTail, oldHead);
			putOccurrencOnEdge(leftOfNewTail, newTail);
			if(oldTail.isActive) {
				oldTail.node->activeOccurrence = &oldHead;
				oldHead.isActive = true;
			}
			delete &oldTail;
			return newHead;
		}
		
		static void putOccurrencOnEdge(Occurrence& left, Occurrence& right) {
			EulerTourTreeEdge* e = left.rightEdge;
			right.leftEdge = e;
			if(*e->occurrence1 == left) {
				e->occurrence2 = &right;
			} else {
				e->occurrence4 = &right;
			}
		}
		
	};

	class EulerTourTreeNode {
		
	public:
		
		EulerTourTreeNode()
		: activeOccurrence(new Occurrence(this, true)) {
		}
		
		~EulerTourTreeNode() {
			using SequenceAlgorithm = SequenceAlgorithmTmpl<SequenceNodeTraitTmpl<Occurrence>>;
			if(activeOccurrence) {
				Occurrence& r = SequenceAlgorithm::findRoot(*activeOccurrence);
				std::vector<Occurrence*> occs(SequenceAlgorithm::size(r));
				for(Occurrence& o : SequenceAlgorithm::containerView(r)) {
					occs.push_back(&o);
					if(o.isActive) {
						o.node->activeOccurrence = nullptr;
					}
				}
				for(Occurrence* o : occs) {
					delete o;
				}
			}
		}
		
		EulerTourTreeNode(const EulerTourTreeNode&) = delete;
		
		EulerTourTreeNode(EulerTourTreeNode&&) = delete;
		
		EulerTourTreeNode& operator=(const EulerTourTreeNode&) = delete;
		
		EulerTourTreeNode& operator=(EulerTourTreeNode&&) = delete;
		
	private:
		
		Occurrence* activeOccurrence;
			
		template<class N, class E> friend class EulerTourTreeAlgorithm;	
	};

	class EulerTourTreeEdge {
		
	public:
		
		EulerTourTreeEdge() {
		}
		
		EulerTourTreeEdge(const EulerTourTreeEdge&) = delete;
		
		EulerTourTreeEdge(EulerTourTreeEdge&& other) = delete;
		
		EulerTourTreeEdge& operator=(const EulerTourTreeEdge&) = delete;
		
		EulerTourTreeEdge& operator=(EulerTourTreeEdge&& other) = delete;
		
	private:
		
		Occurrence* occurrence1;
		Occurrence* occurrence2;
		Occurrence* occurrence3;
		Occurrence* occurrence4;
			
		template<class N, class E> friend class EulerTourTreeAlgorithm;
	};
		
};



/**
 * Public interface from EulerTourTreeModule.
 */

using EulerTourTreeNode = EulerTourTreeModule::EulerTourTreeNode;

using EulerTourTreeEdge = EulerTourTreeModule::EulerTourTreeEdge;

template<class Node, class Edge> 
using EulerTourTreeAlgorithm = EulerTourTreeModule::EulerTourTreeAlgorithm<Node, Edge>;

} //end namespace EulerTourTree
} //end namespace Cnrc
} //end namespace Snu
