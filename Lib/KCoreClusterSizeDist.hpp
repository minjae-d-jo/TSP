#pragma once

#include <iostream>
#include <vector>
#include <set>
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
namespace KCoreClusterSizeDist {

namespace SpanningForestAlgorithm = Snu::Cnrc::EulerTourTreeSpanningForest;
using Node = SpanningForestAlgorithm::EulerTourTreeSpanningForestNode;
using Edge = SpanningForestAlgorithm::EulerTourTreeSpanningForestEdge;
using SpanningForest = SpanningForestAlgorithm::EulerTourTreeSpanningForestAlgorithm<Node, Edge>;
using Size = unsigned int;
using Time = unsigned int;
using NodeName = Size;
using NodeVector = std::vector<Node>;
using EdgeVector = std::vector<Edge>;

Node& theOtherNode(Node& n, Edge& e) {
	Node& n1 = SpanningForest::node1(e);
	Node& n2 = SpanningForest::node2(e);
	return n == n1 ? n2 : n1;
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

void removeNode(Node& n) {
	std::vector<Edge*> edges;
	edges.reserve(SpanningForest::edges(n).size());
	for(Edge& e : SpanningForest::edges(n)) {
		edges.push_back(&e);
	}
	for(Edge* e : edges) {
		SpanningForest::deleteEdge(*e);
	}
}

Size pruneOnce(std::set<Node*>& survivingNodes, const unsigned int k) {
	ON_DEBUG(std::cerr << "iteration-start:" << std::endl);
	std::vector<Node*> toRemove;
	for(Node* n : survivingNodes) {
		if(SpanningForest::edges(*n).size() < k) {
			toRemove.push_back(n);
		}
	}
	for(Node* n : toRemove) {
		ON_DEBUG(std::cerr << "removing: " << n << std::endl);
		survivingNodes.erase(n);
		removeNode(*n);
	}
	ON_DEBUG(std::cerr << "iteration-end:" << std::endl);
	return toRemove.size();
}

std::map<Size, Size> calcClusterSizeDist(const std::set<Node*>& survivingNodes) {
	std::map<Size, Size> dist;
	for(const Node* n : survivingNodes) {
		if(SpanningForest::isClusterRep(*n)) {
			dist[SpanningForest::cluster(*n).size()]++;
		}
	}
	return dist;
}

std::tuple<Size, Size, std::map<Size, Size>> prune(NodeVector& nodes, const unsigned int k) {
	std::set<Node*> survivingNodes;
	for(Node& n : nodes) {
		survivingNodes.insert(&n);
	}
	Time t = 0;
	SpanningForest::Cluster gc = findGC(nodes);
	Size initialGCSize = gc.size();
	Size r = pruneOnce(survivingNodes, k);
	while(r > 0) {
		++t;
		r = pruneOnce(survivingNodes, k);
	}
	return std::make_tuple(initialGCSize, t, calcClusterSizeDist(survivingNodes));
}


} // end namespace KCoreClusterSizeDist
} // end namespace Cnrc
} // end namespace Snu