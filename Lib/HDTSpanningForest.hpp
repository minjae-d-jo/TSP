#pragma once

/**
 * An implementation of fully dynamic graph connectivity algorithm 
 * by Holm, de Lichetenberg and Thorup (J. ACM, 2001).
 */
 
#include <set>
#include <iterator>
#include "EulerTourTree.hpp"

namespace Snu {
namespace Cnrc {
namespace HDTSpanningForest {

class HDTSpanningForestModule {
	
public:
	
	using Level = unsigned int;
	using Size = unsigned int;
	
public:
	
	class HDTSpanningForestNode;
	class HDTSpanningForestEdge;
	template<class> class Cluster;
	template<class> class ConstCluster;
	template<class, class> class HDTSpanningForestAlgorithm;
	
private:
	
	struct LevelNode : public EulerTourTree::EulerTourTreeNode {
		
		using EdgeSet = std::set<HDTSpanningForestEdge*>;
		
		explicit LevelNode(HDTSpanningForestNode& n)
		: edges(), superNode(&n) {
		}
		
		bool operator==(LevelNode& other) const {
			return this == &other;
		}
		
		bool operator!=(LevelNode& other) const {
			return this != &other;
		}
		
		EdgeSet edges;
		HDTSpanningForestNode* superNode;
	};
	
	struct LevelEdge : public EulerTourTree::EulerTourTreeEdge {
		
		explicit LevelEdge(HDTSpanningForestEdge& e)
		: superEdge(&e) {
		}
		
		HDTSpanningForestEdge* superEdge;
	};
	
public:
	
	class HDTSpanningForestNode {
	
	public:
		
		HDTSpanningForestNode()
		: levelNodes() {
			levelNodes.push_back(new LevelNode(*this));
		}
		
		HDTSpanningForestNode(const HDTSpanningForestNode&) = delete;
		
		HDTSpanningForestNode(HDTSpanningForestNode&&) = delete;
		
		~HDTSpanningForestNode() {
			for(LevelNode* n : levelNodes) {
				delete n;
			}
		}
		
	public:
		
		HDTSpanningForestNode& operator=(const HDTSpanningForestNode&) = delete;
		
		HDTSpanningForestNode& operator=(HDTSpanningForestNode&&) = delete;
		
		bool operator==(const HDTSpanningForestNode& other) const {
			return this == &other;
		}
		
		bool operator!=(const HDTSpanningForestNode& other) const {
			return this != &other;
		}
		
	private:
		
		std::vector<LevelNode*> levelNodes;
		
		template<class> friend class Cluster;
		template<class> friend class ConstCluster;
		template<class, class> friend class HDTSpanningForestAlgorithm;
	};
	
	class HDTSpanningForestEdge {
		
	public:
		
		HDTSpanningForestEdge()
		: nodeA(nullptr), nodeB(nullptr), level(0), isTreeEdge(false), levelEdges() {
		}
		
		HDTSpanningForestEdge(const HDTSpanningForestEdge&) = delete;
		
		HDTSpanningForestEdge(HDTSpanningForestEdge&&) = delete;
		
		~HDTSpanningForestEdge() {
			for(LevelEdge* e : levelEdges) {
				delete e;
			}
		}
		
	private:
		
		HDTSpanningForestEdge& operator=(const HDTSpanningForestEdge&) = delete;
		
		HDTSpanningForestEdge& operator=(HDTSpanningForestEdge&&) = delete;
		
	public:
		
		bool isValid() const {
			return nodeA != nullptr && nodeB != nullptr;
		}
		
	private:
		
		HDTSpanningForestNode* nodeA;
		HDTSpanningForestNode* nodeB;
		Level level;
		bool isTreeEdge;
		std::vector<LevelEdge*> levelEdges;
		
		template<class, class> friend class HDTSpanningForestAlgorithm;
	};
	
public:
	
	template<class Node, class Edge>
	class HDTSpanningForestAlgorithm {
		
	private:
		
		using ETTAlgorithm = EulerTourTree::EulerTourTreeAlgorithm<LevelNode, LevelEdge>;
		
	private:
	
		template<class CNode, class BackendItr>
		class ClusterIteratorImpl : public std::iterator<std::forward_iterator_tag, CNode> {
			
		private:
			
			using This = ClusterIteratorImpl<CNode, BackendItr>;
	
		public:
			
			ClusterIteratorImpl() 
			: itr() {
			}
			
			explicit ClusterIteratorImpl(BackendItr itr)
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
			
			CNode& operator*() const {
				return static_cast<CNode&>(*itr->superNode);//safe
			}
			
			CNode* operator->() const {
				return static_cast<CNode*>(itr->superNode);//safe
			}
		
		private:
			
			BackendItr itr;
		};
		
		template<class Trait>
		class EdgeIteratorImpl : public std::iterator<std::forward_iterator_tag, typename Trait::CEdge> {
			
		private:
			
			using This = EdgeIteratorImpl<Trait>;
			
		public:
			
			EdgeIteratorImpl()
			: node(nullptr), i(), j() {
			}
			
			EdgeIteratorImpl(typename Trait::CNode& n, typename Trait::CLevelNodeItr i)
			: node(&n), i(i), j() {
				if(i != node->levelNodes.end()) {
					j = (*i)->edges.begin();
					stepForward();
				}
			}
			
		public:
			
			This& operator++() {
				++j;
				stepForward();
				return *this;
			}
			
			This operator++(int) {
				This tmp(*this);
				operator++();
				return tmp;
			}
			
			bool operator==(const This& other) const {
				return i == other.i && (i == node->levelNodes.end() || j == other.j);
			}
			
			bool operator!=(const This& other) const {
				return i != other.i || (i != node->levelNodes.end() && j != other.j);
			}
			
			typename Trait::CEdge& operator*() const {
				return **j;
			}
			
			typename Trait::CEdge* operator->() const {
				return *j;
			}
			
		private:
			void stepForward() {
				while(j == (*i)->edges.end()) {
					++i;
					if(i != node->levelNodes.end()) {
						j = (*i)->edges.begin();
					} else {
						break;
					}
				}
			}
		
		private:
			
			typename Trait::CNode* node;
			typename Trait::CLevelNodeItr i;
			typename LevelNode::EdgeSet::iterator j;
		};
		
	public:
		
		class Cluster {
			
		private:
			
			using Backend = ETTAlgorithm::NodeContainerView;
			
		private:
			
			explicit Cluster(HDTSpanningForestNode& r)
			: rep(r), backend(ETTAlgorithm::nodeContainerView(*r.levelNodes[0])) {
			}
			
		public:
			
			using iterator = ClusterIteratorImpl<Node, Backend::iterator>;
			using const_iterator = ClusterIteratorImpl<const Node, Backend::const_iterator>;
			
		public:
			
			iterator begin() {
				return iterator(backend.begin());
			}
			
			iterator end() {
				return iterator(backend.end());
			}
			
			const_iterator begin() const {
				return const_iterator(backend.cbegin());
			}
			
			const_iterator end() const {
				return const_iterator(backend.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(backend.cbegin());
			}
			
			const_iterator cend() const {
				return const_iterator(backend.cend());
			}
			
			HDTSpanningForestNode& representative() {
				return rep;
			}
			
			const HDTSpanningForestNode& representative() const {
				return rep;
			}
			
			Size size() const {
				return ETTAlgorithm::clusterSize(*rep.levelNodes[0]);
			}
		
		private:
			HDTSpanningForestNode& rep;
			Backend backend;
			
			template<class, class> friend class HDTSpanningForestAlgorithm;
		};
		
		class ConstCluster {
			
		private:
			
			using Backend = ETTAlgorithm::NodeContainerView;
			
		private:
			
			explicit ConstCluster(const HDTSpanningForestNode& r)
			: rep(r), backend(ETTAlgorithm::nodeContainerView(*r.levelNodes[0])) {
			}
			
		public:
			
			using const_iterator = ClusterIteratorImpl<const Node, Backend::const_iterator>;
			
		public:
			
			const_iterator begin() const {
				return const_iterator(backend.cbegin());
			}
			
			const_iterator end() const {
				return const_iterator(backend.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(backend.cbegin());
			}
			
			const_iterator cend() const {
				return const_iterator(backend.cend());
			}
			
			const HDTSpanningForestNode& representative() const {
				return rep;
			}
			
			Size size() const {
				return ETTAlgorithm::clusterSize(*rep.levelNodes[0]);
			}
		
		private:
			const HDTSpanningForestNode& rep;
			Backend backend;
			
			template<class, class> friend class HDTSpanningForestAlgorithm;
		};
		
		class Edges {
			
		private:
			
			explicit Edges(HDTSpanningForestNode& n)
			: node(n) {
			}
			
		private:
			
			struct T {
				using CNode = HDTSpanningForestNode;
				using CEdge = HDTSpanningForestEdge;
				using CLevelNodeItr = std::vector<LevelNode*>::iterator;
			};
			
			struct CT {
				using CNode = const HDTSpanningForestNode;
				using CEdge = const HDTSpanningForestEdge;
				using CLevelNodeItr = std::vector<LevelNode*>::const_iterator;
			};
			
		public:
			
			using iterator = EdgeIteratorImpl<T>;
			using const_iterator = EdgeIteratorImpl<CT>;
			
		public:
			
			iterator begin() {
				return iterator(node, node.levelNodes.begin());
			}
			
			iterator end() {
				return iterator(node, node.levelNodes.end());
			}
			
			const_iterator begin() const {
				return const_iterator(node, node.levelNodes.begin());
			}
			
			const_iterator end() const {
				return const_iterator(node, node.levelNodes.end());
			}
			
			const_iterator cbegin() const {
				return const_iterator(node, node.levelNodes.begin());
			}
			
			const_iterator cend() const {
				return const_iterator(node, node.levelNodes.end());
			}
			
		private:
			HDTSpanningForestNode& node;
			
			template<class, class> friend class HDTSpanningForestAlgorithm;
		};
		
		class ConstEdges {
			
		private:
			
			explicit ConstEdges(const HDTSpanningForestNode& n)
			: node(n) {
			}
			
		private:
			
			struct CT {
				using CNode = const HDTSpanningForestNode;
				using CEdge = const HDTSpanningForestEdge;
				using CLevelNodeItr = std::vector<LevelNode*>::const_iterator;
			};
			
		public:
			
			using const_iterator = EdgeIteratorImpl<CT>;
			
			
		public:
			
			const_iterator begin() const {
				return const_iterator(node, node.levelNodes.cbegin());
			}
			
			const_iterator end() const {
				return const_iterator(node, node.levelNodes.cend());
			}
			
			const_iterator cbegin() const {
				return const_iterator(node, node.levelNodes.cbegin());
			}
			
			const_iterator cend() const {
				return const_iterator(node, node.levelNodes.cend());
			}
			
		private:
			const HDTSpanningForestNode& node;
			
			template<class, class> friend class HDTSpanningForestAlgorithm;
		};
		
		
	public:
		
		static bool hasPath(const Node& n1, const Node& n2) {
			return ETTAlgorithm::hasPath(
				*archetype(n1).levelNodes[0], 
				*archetype(n2).levelNodes[0]
			);
		}
		
		static Cluster cluster(Node& n) {
			return Cluster(
				*ETTAlgorithm::findClusterRep(
					*archetype(n).levelNodes[0]
				).superNode
			); 
		}
		
		static ConstCluster cluster(const Node& n) {
			return ConstCluster(
				*ETTAlgorithm::findClusterRep(
					*archetype(n).levelNodes[0]
				).superNode
			); 
		}
		
		static Size clusterSize(const Node& n) {
			return ETTAlgorithm::clusterSize(
				ETTAlgorithm::findClusterRep(
					*archetype(n).levelNodes[0]
				)
			);
		}
		
		static bool isClusterRep(const Node& n) {
			return ETTAlgorithm::findClusterRep(
				*archetype(n).levelNodes[0]
			).superNode == &n;
		}
		
		static const Node& findClusterRep(const Node& n) {
			return static_cast<const Node&>(
				*ETTAlgorithm::findClusterRep(
					*archetype(n).levelNodes[0]
				).superNode
			);
		}
		
		static Node& findClusterRep(Node& n) {
			return static_cast<Node&>(
				*ETTAlgorithm::findClusterRep(
					*archetype(n).levelNodes[0]
				).superNode
			);
		}
		
		static Edges edges(Node& n) {
			return Edges(n);
		}
		
		static ConstEdges edges(const Node& n) {
			return ConstEdges(n);
		}
		
		static bool createEdge(Node& n1, Node& n2, Edge& he) {
			HDTSpanningForestEdge& e = archetype(he);
			e.nodeA = &archetype(n1);
			e.nodeB = &archetype(n2);
			e.level = 0;
			e.nodeA->levelNodes[0]->edges.insert(&e);
			e.nodeB->levelNodes[0]->edges.insert(&e);
			if(ETTAlgorithm::hasPath(
				*e.nodeA->levelNodes[0], 
				*e.nodeB->levelNodes[0]
			)) {
				e.isTreeEdge = false;
				return false;
			} else {
				replaceWith(e);
				return true;
			}
		}
		
		static bool deleteEdge(Edge& he) {
			HDTSpanningForestEdge& e = archetype(he);
			bool clusterSplitted = false;
			if(e.isTreeEdge) {
				eraseTreeEdge(e);
				clusterSplitted = ! checkReplacement(e);
			} else {
				eraseNonTreeEdge(e);
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
		
		static HDTSpanningForestNode& archetype(Node& n) {
			return static_cast<HDTSpanningForestNode&>(n);
		}
		
		static const HDTSpanningForestNode& archetype(const Node& n) {
			return static_cast<const HDTSpanningForestNode&>(n);
		}
		
		static HDTSpanningForestEdge& archetype(Edge& e) {
			return static_cast<HDTSpanningForestEdge&>(e);
		}
		
		static const HDTSpanningForestEdge& archetype(const Edge& e) {
			return static_cast<const HDTSpanningForestEdge&>(e);
		}
		
		static void eraseTreeEdge(HDTSpanningForestEdge& e) {
			for(LevelEdge* ee : e.levelEdges) {
				ETTAlgorithm::deleteEdge(*ee);
				delete ee;
			}
			e.levelEdges.clear();
			e.nodeA->levelNodes[e.level]->edges.erase(&e);
			e.nodeB->levelNodes[e.level]->edges.erase(&e);
		}
		
		static void eraseNonTreeEdge(HDTSpanningForestEdge& e) {
			e.nodeA->levelNodes[e.level]->edges.erase(&e);
			e.nodeB->levelNodes[e.level]->edges.erase(&e);
		}
		
		static void invalidate(HDTSpanningForestEdge& e) {
			e.nodeA = nullptr;
			e.nodeB = nullptr;
		}
		
		static bool checkReplacement(HDTSpanningForestEdge& e) {
			for(Level l = 0; l <= e.level; ++l) {
				if(checkReplacement(e, e.level - l)) {
					return true;
				}
			}
			return false;
		}
		
		static bool checkReplacement(HDTSpanningForestEdge& e, Level l) {
			LevelNode& r1 = ETTAlgorithm::findClusterRep(*e.nodeA->levelNodes[l]);
			LevelNode& r2 = ETTAlgorithm::findClusterRep(*e.nodeB->levelNodes[l]);
			ETTAlgorithm::Size sz1 = ETTAlgorithm::clusterSize(r1);
			ETTAlgorithm::Size sz2 = ETTAlgorithm::clusterSize(r2);
			if(sz1 < sz2) {
				return checkReplacement(r1, r2, l);
			} else {
				return checkReplacement(r2, r1, l);
			}
		}
		
		static bool checkReplacement(LevelNode& smaller, LevelNode& larger, Level l) {
			LevelNode& largerRep = ETTAlgorithm::findClusterRep(*larger.superNode->levelNodes[0]);
			LevelNode::EdgeSet replacements;
			std::vector<LevelNode*> nodesInSmaller;
			for(LevelNode& n : ETTAlgorithm::nodeContainerView(smaller)) {
				nodesInSmaller.push_back(&n);
				auto i = n.edges.begin(); 
				while(i != n.edges.end()) {
					HDTSpanningForestEdge* e = *i;
					LevelNode& m = *e->nodeA->levelNodes[l] == n ? 
						*e->nodeB->levelNodes[0]:
						*e->nodeA->levelNodes[0];
					if(! e->isTreeEdge && ETTAlgorithm::findClusterRep(m) == largerRep) {
						i = n.edges.erase(i);
						replacements.insert(e);
					} else {
						++i;
					}
				}
			}
			if(! replacements.empty()) {
				for(LevelNode* n : nodesInSmaller) {
					levelUpEdgesOf(*n, l + 1);
				}
				for(HDTSpanningForestEdge* e : replacements) {
					e->nodeA->levelNodes[l]->edges.insert(e);
					e->nodeB->levelNodes[l]->edges.insert(e);
				}
				replaceWith(**replacements.begin());
				return true;
			} else {
				return false;
			}
		}
		
		static void replaceWith(HDTSpanningForestEdge& e) {
			e.levelEdges.reserve(e.level + 1);
			for(Level l = 0; l <= e.level; ++l) {
				LevelEdge* le = new LevelEdge(e);
				e.levelEdges.push_back(le);
				ETTAlgorithm::createEdge(*e.nodeA->levelNodes[l], *e.nodeB->levelNodes[l], *le);
			}
			e.isTreeEdge = true;
		}
		
		static void levelUpEdgesOf(LevelNode& n, Level l) {
			LevelNode& nn = levelNode(*n.superNode, l);
			if(nn.edges.empty()) {
				nn.edges = std::move(n.edges);
			} else {
				nn.edges.insert(n.edges.begin(), n.edges.end());
			}
			n.edges.clear();
			for(HDTSpanningForestEdge* e : nn.edges) {
				if(e->isTreeEdge && e->level != l) {
					levelUpTreeEdge(*e, l);
				} else {
					e->level = l;
				}
			}
		}
		
		static void levelUpTreeEdge(HDTSpanningForestEdge& e, Level l) {
			e.level = l;
			LevelEdge* le = new LevelEdge(e);
			e.levelEdges.push_back(le);
			ETTAlgorithm::createEdge(
				levelNode(*e.nodeA, l),
				levelNode(*e.nodeB, l),
				*le
			);
		}
		
		static LevelNode& levelNode(HDTSpanningForestNode& n, Level l) {
			if(l == n.levelNodes.size()) {
				n.levelNodes.push_back(new LevelNode(n));
			}
			return *n.levelNodes[l];
		}
	};
	
};


/**
 * Public interface from HDTSpanningForestModule
 */

using HDTSpanningForestNode = HDTSpanningForestModule::HDTSpanningForestNode;

using HDTSpanningForestEdge = HDTSpanningForestModule::HDTSpanningForestEdge;

template<class Node, class Edge>
using HDTSpanningForestAlgorithm = HDTSpanningForestModule::HDTSpanningForestAlgorithm<Node, Edge>;


} //end namespace HDTSpanningForest
} //end namespace Cnrc
} //end namespace Snu
