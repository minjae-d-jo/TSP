#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <HDTSpanningForest.hpp>
#include <EulerTourTreeSpanningForest.hpp>
#include <HDTSpanningForest.hpp>

#include <map>

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


Node& theOtherNode(Node& n, Edge& e) {
	Node& n1 = SpanningForest::node1(e);
	Node& n2 = SpanningForest::node2(e);
	return n == n1 ? n2 : n1;
}

SpanningForest::Cluster removeNode(Node& n) {
	std::vector<Edge*> edges;
	edges.reserve(SpanningForest::edges(n).size());
	for(Edge& e : SpanningForest::edges(n)) {
		edges.push_back(&e);
	}
	for(Edge* e : edges) {
		Node& m = theOtherNode(n, *e);
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

std::pair<Size, SpanningForest::Cluster> pruneOnce(SpanningForest::Cluster gc, const unsigned int k) {
	ON_DEBUG(std::cerr << "iteration-start:" << std::endl);
	if(gc.size() == 0) {
		return std::make_pair(0, gc);
	}
	std::vector<Node*> toRemove;
	for(Node& n : gc) {
		if(SpanningForest::edges(n).size() < k) {
			toRemove.push_back(&n);
			std::cout << "THIS " << &n << "\t"<< toRemove[0] << "\t"<< &n - toRemove[0] << "\t"<< "  "<<SpanningForest::edges(n).size() << std::endl;
		}
	}
	std::cout << "toRemove : " << toRemove.size()<< std::endl;
	std::cout << "gc : " << gc.size()<< std::endl;
	bool emptyGC = toRemove.size() == gc.size();
	std::cout << "empty Gc : " << emptyGC<< std::endl;
	for(auto n : toRemove) {
		ON_DEBUG(std::cerr << "removing: " << n << std::endl);
		if(SpanningForest::findClusterRep(*n) == gc.representative()) {
			std::cout<<"representative??"<<&gc.representative()<<std::endl;
			gc = removeNode(*n);
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
std::map<Size,Size> genCluster(SpanningForest::Cluster gc, const unsigned int k) {
	std::map<Size,Size> clusterSizeDistribution;
	SpanningForest::Cluster aliasGC = gc;
	std::vector<Node*> toRemove;
	//std::set<Node*> currentBreadth, nextBreadth;
	std::vector<Node*> coronaNode; //(nodes.size())
	//std::cout<<"HERE1"<<std::endl;
	//std::cout<<"GC SIZE " << aliasGC.size()<<std::endl;
	if(aliasGC.size() == 0) {
		return std::map<Size,Size>();
	}
	for(Node& n : aliasGC) {
		if(SpanningForest::edges(n).size() == k) 
			coronaNode.push_back(&n);
//		else if(aliasGC.size()!=0 && SpanningForest::findClusterRep(n) == gc.representative())
//			aliasGC = removeNode(n);			
		else 
			toRemove.push_back(&n);
	}
	for(Node* n : toRemove) {
		ON_DEBUG(std::cerr << "removing: " << n << std::endl);
		aliasGC = removeNode(*n);
	}
//std::cout<<"HERE2"<<std::endl;
//if(gc.size()!=0){
	for(const Node* n : coronaNode) {
		if(SpanningForest::isClusterRep(*n)) {
			clusterSizeDistribution[SpanningForest::cluster(*n).size()]++;
		}
	}
	return clusterSizeDistribution;
}
	/*std::cout << clusterInformation << std::endl;
	Size sizeOfCluster, numberOfCluster = 0;
	auto it = clusterInformation[0];
	//cout << clusterInformation[0] <<endl;
	if(gc.size()!=0){
		for(Node& n : gc) {
				if(SpanningForest::isClusterRep(n)) {
				clusterSizeDistribution[SpanningForest::cluster(n).size()]++;
			}
		}*/
		
			



/*		if(SpanningForest::edges(n).size() == k) {

	while(it != clusterInformation.end()){
		currentBreadth.insert(&n);
		clusterInformation[&n] = &n;
		sizeOfCluster = 1;
		while(currentBreadth.size()!=0){
			for(auto i : currentBreadth){
//				std::cout<<"parent Node : "<< i <<std::endl;
				for(const Edge& e : SpanningForest::edges(*i)) {
				//for(auto j : nodeToNode[i])
					if(SpanningForest::node1(e) != *i) {
						if(cluster[*i]==true){
							continue;
						}
						//std::cout<<"next Node : "<< nodes[i] << std::endl;
						nextBreadth.insert(*i);
						cluster[*i] = true;	
						++sizeOfCluster;
					}
				}
			}
			currentBreadth=nextBreadth;
			nextBreadth.clear();
		}
//		std::cout<<"clustersize : " << sizeOfCluster << std::endl;
		++numberOfCluster;
		++clusterSizeDistribution[sizeOfCluster];	
		it = find(cluster.begin(), cluster.end(), 0);
  	}

			
			std::cout << "THIS " << &n << "  "<<SpanningForest::edges(n).size() << std::endl;
		}
	}
	
*/
	


struct Result {
	Size initialGCSize;
	Size steadyGCSize;
	Time time;
};

std::tuple<Size, Size, Time, std::map<Size,Size>> prune(NodeVector& nodes, const unsigned int k) {
	Time t = 0;
	SpanningForest::Cluster gc = findGC(nodes);
	Size initialGCSize = gc.size();
	std::pair<Size, SpanningForest::Cluster> r = pruneOnce(gc, k);
	while(r.first > 0) {
		++t;
		r = pruneOnce(r.second, k);
	}
	std::cout<<"HEEE"<<std::endl;
	//genCluster(r.second, k);
	return std::tuple<Size, Size, Time,std::map<Size,Size>>(
		initialGCSize,
		r.second.size(), 
		t,
		genCluster(r.second, k));
	
}


} // end namespace KCore
} // end namespace Cnrc
} // end namespace Snu