#pragma once

/**
 * An implementation of fully dynamic graph connectivity algorithm 
 * by Holm, de Lichetenberg and Thorup (J. ACM, 2001).
 */
 
#include <iterator>
#include <boost/container/flat_set.hpp>
#include "EulerTourTree.hpp"

namespace Snu {
namespace Cnrc {
namespace EulerTourTreeSpanningForest {

class EulerTourTreeSpanningForestModule {
	
public:
	
	using Level = unsigned int;
	using Size = unsigned int;
	
public:
	
	class EulerTourTreeSpanningForestNode;
	class EulerTourTreeSpanningForestEdge;
	template<class> class Cluster;
	template<class> class ConstCluster;
	template<class, class> class EulerTourTreeSpanningForestAlgorithm;
	
public:
	
	class EulerTourTreeSpanningForestNode : public EulerTourTree::EulerTourTreeNode {
		
	public:
		using EdgeSet = boost::container::flat_set<EulerTourTreeSpanningForestEdge*>;
	
	public:
		
		EulerTourTreeSpanningForestNode()
		: edges() {
		}
		
		EulerTourTreeSpanningForestNode(const EulerTourTreeSpanningForestNode&) = delete;
		
		EulerTourTreeSpanningForestNode(EulerTourTreeSpanningForestNode&&) = delete;
		
	public:
		
		EulerTourTreeSpanningForestNode& operator=(const EulerTourTreeSpanningForestNode&) = delete;
		
		EulerTourTreeSpanningForestNode& operator=(EulerTourTreeSpanningForestNode&&) = delete;
		
		bool operator==(const EulerTourTreeSpanningForestNode& other) const {
			return this == &other;
		}
		
		bool operator!=(const EulerTourTreeSpanningForestNode& other) const {
			return this != &other;
		}
		
	private:
		
		EdgeSet edges;
		
		template<class> friend class Cluster;
		template<class> friend class ConstCluster;
		template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
	};
	
	class EulerTourTreeSpanningForestEdge : public EulerTourTree::EulerTourTreeEdge {
		
	public:
		
		EulerTourTreeSpanningForestEdge()
		: nodeA(nullptr), nodeB(nullptr), treeEdge(false) {
		}
		
		EulerTourTreeSpanningForestEdge(const EulerTourTreeSpanningForestEdge&) = delete;
		
		EulerTourTreeSpanningForestEdge(EulerTourTreeSpanningForestEdge&&) = delete;
		
	private:
		
		EulerTourTreeSpanningForestEdge& operator=(const EulerTourTreeSpanningForestEdge&) = delete;
		
		EulerTourTreeSpanningForestEdge& operator=(EulerTourTreeSpanningForestEdge&&) = delete;
		
	public:
		
		bool isValid() const {
			return nodeA != nullptr && nodeB != nullptr;
		}
		
		bool isTreeEdge() const {
			return treeEdge;
		}
		
	private:
		
		EulerTourTreeSpanningForestNode* nodeA;
		EulerTourTreeSpanningForestNode* nodeB;
		bool treeEdge;
		
		template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
	};
	
public:
	
	template<class Node, class Edge>
	class EulerTourTreeSpanningForestAlgorithm {
		
	private:
		
		using ETTAlgorithm = EulerTourTree::EulerTourTreeAlgorithm<Node, Edge>;
		
		
	public:
		
		class Cluster : public ETTAlgorithm::NodeContainerView {
			
		private:
			
			explicit Cluster(Node& r)
			: ETTAlgorithm::NodeContainerView(ETTAlgorithm::nodeContainerView(r)), rep(&r) {
			}
			
		public:
			Cluster()
			: ETTAlgorithm::NodeContainerView(), rep(nullptr) {
			}
			
			Cluster(const Cluster& other) 
			: ETTAlgorithm::NodeContainerView(other), rep(other.rep) {
			}
			
		public:
			
			EulerTourTreeSpanningForestNode& representative() {
				return *rep;
			}
			
			const EulerTourTreeSpanningForestNode& representative() const {
				return *rep;
			}
			
			Size size() const {
				return rep ? ETTAlgorithm::clusterSize(*rep) : 0;
			}
		
		private:
			Node* rep;
			
			template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
		};
		
		class ConstCluster : public ETTAlgorithm::ConstNodeContainerView {
			
		private:
			
			explicit ConstCluster(const Node& r)
			: ETTAlgorithm::ConstNodeContainerView(ETTAlgorithm::nodeContainerView(r)), rep(r) {
			}
			
		public:	
			
			const EulerTourTreeSpanningForestNode& representative() const {
				return rep;
			}
			
			Size size() const {
				return ETTAlgorithm::clusterSize(rep);
			}
		
		private:
			const Node& rep;
			
			template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
		};
		
	private:
		
		template<class CEdge, class BackendItr>
		class EdgeIteratorImpl : public std::iterator<std::forward_iterator_tag, CEdge> {
			
		private:
			
			using This = EdgeIteratorImpl<CEdge, BackendItr>;
	
		public:
			
			EdgeIteratorImpl() 
			: itr() {
			}
			
			explicit EdgeIteratorImpl(BackendItr itr)
			: itr(itr) {
			}
			
		public:
			
			This& operator++() {
				++itr;
				return *this;
			}
			
			This operator++(int) {
				This tmp(*this);
				operator++();
				return tmp;
			}
			
			bool operator==(const This& other) const {
				return itr == other.itr;
			}
			
			bool operator!=(const This& other) const {
				return itr != other.itr;
			}
			
			CEdge& operator*() const {
				return **itr;
			}
			
			CEdge* operator->() const {
				return *itr;
			}
		
		private:
			
			BackendItr itr;
		};
		
	public:
		
		class Edges {
			
		private:
			
			explicit Edges(Node& n)
			: node(n) {
			}
			
		public:
			
			using iterator = EdgeIteratorImpl<Edge, typename Node::EdgeSet::iterator>;
			using const_iterator = EdgeIteratorImpl<const Edge, typename Node::EdgeSet::const_iterator>;
			
		public:
			
			iterator begin() {
				return iterator(node.edges.begin());
			}
			
			iterator end() {
				return iterator(node.edges.end());
			}
			
			const_iterator begin() const {
				return const_iterator(node.edges.cbegin());
			}
			
			const_iterator end() const {
				return const_iterator(node.edges.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(node.edges.cbegin());
			}
			
			const_iterator cend() const {
				return const_iterator(node.edges.cend());
			}
			
			size_t size() const {
				return node.edges.size();
			}
			
		private:
			EulerTourTreeSpanningForestNode& node;
			
			template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
		};
		
		class ConstEdges {
			
		private:
			
			explicit ConstEdges(const EulerTourTreeSpanningForestNode& n)
			: node(n) {
			}
			
		public:
			
			using const_iterator = EdgeIteratorImpl<const Edge, typename Node::EdgeSet::const_iterator>;
			
		public:
			
			const_iterator begin() const {
				return const_iterator(node.edges.cbegin());
			}
			
			const_iterator end() const {
				return const_iterator(node.edges.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(node.edges.cbegin());
			}
			
			const_iterator cend() const {
				return const_iterator(node.edges.cend());
			}
			
			size_t size() const {
				return node.edges.size();
			}
			
		private:
			const EulerTourTreeSpanningForestNode& node;
			
			template<class, class> friend class EulerTourTreeSpanningForestAlgorithm;
		};
		
		
	public:
		
		static bool hasPath(const Node& n1, const Node& n2) {
			return ETTAlgorithm::hasPath(n1, n2);
		}
		
		static Cluster cluster(Node& n) {
			return Cluster(ETTAlgorithm::findClusterRep(n)); 
		}
		
		static ConstCluster cluster(const Node& n) {
			return ConstCluster(ETTAlgorithm::findClusterRep(n)); 
		}
		
		static Size clusterSize(const Node& n) {
			return ETTAlgorithm::clusterSize(ETTAlgorithm::findClusterRep(n));
		}
		
		static bool isClusterRep(const Node& n) {
			return &ETTAlgorithm::findClusterRep(n) == &n;
		}
		
		static const Node& findClusterRep(const Node& n) {
			return ETTAlgorithm::findClusterRep(n);
		}
		
		static Node& findClusterRep(Node& n) {
			return ETTAlgorithm::findClusterRep(n);
		}
		
		static Edges edges(Node& n) {
			return Edges(n);
		}
		
		static ConstEdges edges(const Node& n) {
			return ConstEdges(n);
		}
		
		static bool createEdge(Node& n1, Node& n2, Edge& e) {
			e.nodeA = &n1;
			e.nodeB = &n2;
			e.nodeA->edges.insert(&e);
			e.nodeB->edges.insert(&e);
			if(ETTAlgorithm::hasPath(
				static_cast<Node&>(*e.nodeA), 
				static_cast<Node&>(*e.nodeB)
			)) {
				e.treeEdge = false;
				return false;
			} else {
				replaceWith(e);
				return true;
			}
		}
		
		static bool deleteEdge(Edge& e) {
			bool clusterSplitted = false;
			e.nodeA->edges.erase(&e);
			e.nodeB->edges.erase(&e);
			if(e.treeEdge) {
				ETTAlgorithm::deleteEdge(e);
				clusterSplitted = ! checkReplacement(e);
			}
			invalidate(e);
			return clusterSplitted;
		}
		
		static const Node& node1(const Edge& e) {
			return static_cast<const Node&>(*e.nodeA);
		}
		
		static Node& node1(Edge& e) {
			return static_cast<Node&>(*e.nodeA);
		}
		
		static const Node& node2(const Edge& e) {
			return static_cast<const Node&>(*e.nodeB);
		}
		
		static Node& node2(Edge& e) {
			return static_cast<Node&>(*e.nodeB);
		}
		
	private:
		
		static void invalidate(Edge& e) {
			e.nodeA = nullptr;
			e.nodeB = nullptr;
		}
		
		static void replaceWith(Edge& e) {
			ETTAlgorithm::createEdge(static_cast<Node&>(*e.nodeA), static_cast<Node&>(*e.nodeB), e);
			e.treeEdge = true;
		}
		
		static bool checkReplacement(Edge& e) {
			Node& r1 = ETTAlgorithm::findClusterRep(static_cast<Node&>(*e.nodeA));
			Node& r2 = ETTAlgorithm::findClusterRep(static_cast<Node&>(*e.nodeB));
			typename ETTAlgorithm::Size sz1 = ETTAlgorithm::clusterSize(r1);
			typename ETTAlgorithm::Size sz2 = ETTAlgorithm::clusterSize(r2);
			if(sz1 < sz2) {
				return checkReplacement(r1, r2);
			} else {
				return checkReplacement(r2, r1);
			}
		}

		static bool checkReplacement(Node& smaller, Node& larger) {
			for(Node& n : ETTAlgorithm::nodeContainerView(smaller)) {
				for(EulerTourTreeSpanningForestEdge* e : n.edges) {
					EulerTourTreeSpanningForestNode& m = *e->nodeA == n ? 
						*e->nodeB:
						*e->nodeA;
					if(ETTAlgorithm::findClusterRep(static_cast<Node&>(m)) == larger) {
						replaceWith(static_cast<Edge&>(*e));
						return true;
					}
				}
			}
			return false;
		}
	};
	
};


/**
 * Public interface from EulerTourTreeSpanningForestModule
 */

using EulerTourTreeSpanningForestNode = EulerTourTreeSpanningForestModule::EulerTourTreeSpanningForestNode;

using EulerTourTreeSpanningForestEdge = EulerTourTreeSpanningForestModule::EulerTourTreeSpanningForestEdge;

template<class Node, class Edge>
using EulerTourTreeSpanningForestAlgorithm = EulerTourTreeSpanningForestModule::EulerTourTreeSpanningForestAlgorithm<Node, Edge>;


} //end namespace EulerTourTreeSpanningForest
} //end namespace Cnrc
} //end namespace Snu
