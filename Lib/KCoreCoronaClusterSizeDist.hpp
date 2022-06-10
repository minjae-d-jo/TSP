#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <HDTSpanningForest.hpp>
#include <EulerTourTreeSpanningForest.hpp>
#include <HDTSpanningForest.hpp>
#include <RandomNumber.hpp>

#ifdef KCORE_DEBUG
	#define ON_DEBUG(x) x
#else
	#define ON_DEBUG(x) ;
#endif

namespace Snu {
namespace Cnrc {
namespace KCore {
	
namespace SpanningForestAlgorithm = Snu::Cnrc::EulerTourTreeSpanningForest;
using Node = SpanningForestAlgorithm::EulerTourTreeSpanningForestNode;
using Edge = SpanningForestAlgorithm::EulerTourTreeSpanningForestEdge;
using SpanningForest = SpanningForestAlgorithm::EulerTourTreeSpanningForestAlgorithm<Node, Edge>;
using Size = unsigned int;
using Time = unsigned int;
using NodeName = Size;
using NodeVector = std::vector<Node>;
using EdgeVector = std::vector<Edge>;

struct PruningResult {
	Size avalancheSize;
	Time relaxationTime;
	SpanningForest::Cluster giantCluster;
};

struct RemovedEdge {
	Node* node1;
	Node* node2;
	Edge* edge;
};

struct PhantomVector {
	void push_back(RemovedEdge) {
	}
};

Node& theOtherNode(Node& n, Edge& e) {
	Node& n1 = SpanningForest::node1(e);
	Node& n2 = SpanningForest::node2(e);
	return n == n1 ? n2 : n1;
}

template<class T>
SpanningForest::Cluster removeNode(Node& n, T& removedEdges) {
	std::vector<Edge*> edges;
	edges.reserve(SpanningForest::edges(n).size());
	for(Edge& e : SpanningForest::edges(n)) {
		edges.push_back(&e);
	}
	for(Edge* e : edges) {
		Node& m = theOtherNode(n, *e);
		removedEdges.push_back(RemovedEdge{&n, &m, e});
		if(SpanningForest::deleteEdge(*e)) {
			SpanningForest::Cluster cN = SpanningForest::cluster(n);
			SpanningForest::Cluster cM = SpanningForest::cluster(m);
			ON_DEBUG(std::cerr << cN.size() << " | " << cM.size() << std::endl);
			if(cN.size() <= cM.size() || cN.size() == 1) {
				return cM;
			}
		}
	}
	return SpanningForest::Cluster();
}

SpanningForest::Cluster removeNode(Node& n) {
	PhantomVector v;
	return removeNode(n, v);
}

void removeNodeX(Node& n) {
	std::vector<Edge*> edges;
	edges.reserve(SpanningForest::edges(n).size());
	for(Edge& e : SpanningForest::edges(n)) {
		edges.push_back(&e);
	}
	for(Edge* e : edges) {
		SpanningForest::deleteEdge(*e);
	}
}

template<class T>
std::pair<Size, SpanningForest::Cluster> pruneOnce(SpanningForest::Cluster gc, const unsigned int k, T& removedEdges) {
	ON_DEBUG(std::cerr << "iteration-start:" << std::endl);
	if(gc.size() == 0) {
		return std::make_pair(0, gc);
	}
	std::vector<Node*> toRemove;
	for(Node& n : gc) {
		if(SpanningForest::edges(n).size() < k) {
			toRemove.push_back(&n);
		}
	}
	bool emptyGC = toRemove.size() == gc.size();
	for(auto n : toRemove) {
		ON_DEBUG(std::cerr << "removing: " << n << std::endl);
		if(SpanningForest::findClusterRep(*n) == gc.representative()) {
			gc = removeNode(*n, removedEdges);
		}
	}
	ON_DEBUG(std::cerr << "iteration-end:" << std::endl);
	return emptyGC ? 
		std::make_pair(toRemove.size(), SpanningForest::Cluster()) :
		std::make_pair(toRemove.size(), gc);
}

SpanningForest::Cluster findGC(NodeVector& nodes) {
	SpanningForest::Cluster gc = SpanningForest::cluster(nodes[0]);
	for(Node& n : nodes) {
		SpanningForest::Cluster c = SpanningForest::cluster(n);
		if(c.size() > gc.size()) {
			gc = c;
		}
	}
	return gc;
}

template<class T>
PruningResult prune(NodeVector& nodes, const unsigned int k, SpanningForest::Cluster gc, T& removedEdges) {
	Time t = 0;
	Size initialGCSize = gc.size();
	std::pair<Size, SpanningForest::Cluster> r = pruneOnce(gc, k, removedEdges);
	while(r.first > 0) {
		++t;
		r = pruneOnce(r.second, k, removedEdges);
	}
	return PruningResult{
		initialGCSize - r.second.size(),
		t,
		r.second
	};
}

PruningResult prune(NodeVector& nodes, const unsigned int k) {
	PhantomVector v;
	return prune(nodes, k, findGC(nodes), v);
}

Node& randomNodeFromGC(NodeVector& nodes, SpanningForest::Cluster gc) {
	//Do we need to improve this implementation?
	static RandomUnsignedIntGenerator randomNode(0, nodes.size() - 1);
	Node* n = &nodes[randomNode()];
	while(SpanningForest::findClusterRep(*n) != gc.representative()) {
		n = &nodes[randomNode()];
	}
	return *n;
}

/**
 * We do not count the initially removed node for the avalanche size or relaxation time.
 */
PruningResult avalanche(NodeVector& nodes, const unsigned int k, Node& n, std::vector<RemovedEdge>& removedEdges) {
	SpanningForest::Cluster gc = removeNode(n, removedEdges);
	return prune(nodes, k, gc, removedEdges);
}

/**
 * We do not count the initially removed node for the avalanche size or relaxation time.
 */
PruningResult avalancheFromRandomNode(NodeVector& nodes, const unsigned int k, SpanningForest::Cluster gc) {
	std::vector<RemovedEdge> removedEdges;
	Node& n = randomNodeFromGC(nodes, gc);
	PruningResult r = avalanche(nodes, k, n, removedEdges);
	for(auto e : removedEdges) {
		SpanningForest::createEdge(*e.node1, *e.node2, *e.edge);
	}
	r.giantCluster = SpanningForest::cluster(*removedEdges[0].node1);
	return r;
}

std::map<Size,Size> calcCoronaClusterSizeDist(SpanningForest::Cluster gc, const unsigned int k) {
	std::map<Size,Size> coronaClusterSizeDistribution;
	std::vector<Node*> toRemove;
	std::vector<Node*> coronaNode; 
	if(gc.size() == 0) {
		return std::map<Size,Size>();
	}
	for(Node& n : gc) {
		if(SpanningForest::edges(n).size() == k) 
			coronaNode.push_back(&n);
		else 
			toRemove.push_back(&n);
	}
	for(Node* n : toRemove) {
		ON_DEBUG(std::cerr << "removing: " << n << std::endl);
		removeNodeX(*n);
	}
	for(const Node* n : coronaNode) {
		if(SpanningForest::isClusterRep(*n)) {
			coronaClusterSizeDistribution[SpanningForest::cluster(*n).size()]++;
		}
	}
	return coronaClusterSizeDistribution;
}

} // end namespace KCore
} // end namespace Cnrc
} // end namespace Snu