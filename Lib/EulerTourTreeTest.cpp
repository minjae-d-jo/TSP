#define BOOST_TEST_MODULE EulerTourTreeTest
#include <boost/test/included/unit_test.hpp>

#include <memory>
#include "EulerTourTree.hpp"

using namespace std;
using namespace Snu::Cnrc::EulerTourTree;

struct MyNode : public EulerTourTreeNode {
	
	//for convenience of test
	bool operator==(const MyNode& other) const {
		return this == &other;
	}
	
	//for convenience of test
	bool operator!=(const MyNode& other) const {
		return this != &other;
	}
	
	//for convenience of test.
	bool operator<(const MyNode& other) const {
		return this < &other;
	}
	
	//for convenience of test.
	unsigned int name;
};

struct MyEdge : public EulerTourTreeEdge {
	
	//for convenience of test.
	unsigned int node1;
	unsigned int node2;
};

using ETT = EulerTourTreeAlgorithm<MyNode, MyEdge>;

BOOST_AUTO_TEST_CASE(TestSingleNodeTree) {
	MyNode node;
	node.name = 1234;
	BOOST_REQUIRE(ETT::hasPath(node, node));
	BOOST_REQUIRE(ETT::isClusterRep(node));
	BOOST_REQUIRE(ETT::clusterSize(node) == 1);
	BOOST_REQUIRE(ETT::findClusterRep(node) == node);
	
	{
		ETT::NodeContainerView seq = ETT::nodeContainerView(node);
		ETT::NodeContainerView::iterator itr = seq.begin();
		BOOST_REQUIRE(*itr == node);
		BOOST_REQUIRE(itr->name == 1234);
		BOOST_REQUIRE(ETT::isClusterRep(*itr));
		itr->name = 4321;
		BOOST_REQUIRE(*itr++ == node);
		BOOST_REQUIRE(itr == seq.end());
	}

	{
		ETT::ConstNodeContainerView seq = ETT::nodeContainerView(static_cast<const MyNode&>(node));
		ETT::ConstNodeContainerView::const_iterator itr = seq.begin();
		BOOST_REQUIRE(*itr == node);
		BOOST_REQUIRE(itr->name == 4321);
		BOOST_REQUIRE(ETT::isClusterRep(*itr));
		BOOST_REQUIRE(*itr++ == node);
		BOOST_REQUIRE(itr == seq.end());
	}
}

set<const MyNode*> toNodeSet(const MyNode& n) {
	set<const MyNode*> s;
	for(const MyNode& m : ETT::nodeContainerView(n)) {
		BOOST_REQUIRE(s.insert(&m).second);
	}
	return s;
}

set<const MyEdge*> toEdgeSet(const MyNode& n) {
	set<const MyEdge*> s;
	for(const MyEdge& e : ETT::edgeContainerView(n)) {
		BOOST_REQUIRE(s.insert(&e).second);
	}
	return s;
}

MyNode* makeNodes(unsigned int n) {
	MyNode* const nodes = new MyNode[n];
	for(unsigned int i = 0;  i < n; ++i) {
		nodes[i].name = i;
	}
	return nodes;
}

MyEdge* makeEdges(unsigned int n) {
	MyEdge* const edges = new MyEdge[n];
	for(unsigned int i = 0;  i < n; ++i) {
		edges[i].node1 = i;
		edges[i].node2 = i + 1;
	}
	return edges;
}

void connectRange(
	MyNode* const nodes,
	MyEdge* const edges,
	const unsigned int from, 
	const unsigned to
) {
	for(unsigned int i = from;  i < to - 1; ++i) {
		ETT::createEdge(nodes[i], nodes[i + 1], edges[i]);
	}
}

void assertRangeConnected(
	const MyNode* const nodes,
	const MyEdge* const edges, 
	const unsigned int from, 
	const unsigned to
) {
	const MyNode& r = *find_if(nodes + from, nodes + to, [](const MyNode& n) {
		return ETT::isClusterRep(n);
	});
	set<const MyNode*> ns;
	set<const MyEdge*> es;
	for(unsigned int i = from; i < to; ++i) {
		ns.insert(nodes + i);
	}
	for(unsigned int i = from; i < to - 1; ++i) {
		es.insert(edges + i);
	}
	for(unsigned int i = from; i < to; ++i) {
		if(nodes[i] != r) {
			BOOST_REQUIRE(! ETT::isClusterRep(nodes[i]));
		}
		BOOST_REQUIRE(ETT::findClusterRep(nodes[i]) == r);
		BOOST_REQUIRE(ETT::clusterSize(nodes[i]) == to - from);
		for(unsigned int j = from; j < to; ++j) {
			BOOST_REQUIRE(ETT::hasPath(nodes[i], nodes[j]));
		}
		BOOST_REQUIRE(toNodeSet(nodes[i]) == ns);
		BOOST_REQUIRE(toEdgeSet(nodes[i]) == es);
	}
}

void assertRangeNotConnected(
	const MyNode* const nodes, 
	const unsigned int from1, 
	const unsigned int to1,
	const unsigned int from2, 
	const unsigned int to2
) {
	for(unsigned int i = from1; i < to1; ++i) {
		for(unsigned int j = from2; j < to2; ++j) {
			BOOST_REQUIRE(! ETT::hasPath(nodes[i], nodes[j]));
		}
	}
}

BOOST_AUTO_TEST_CASE(TestTwoNodesTree) {
	MyNode nodes[2];
	MyEdge edge;
	assertRangeNotConnected(nodes, 0, 1, 1, 2);
	ETT::createEdge(nodes[0], nodes[1], edge);
	assertRangeConnected(nodes, &edge, 0, 2);
	ETT::deleteEdge(edge);
	assertRangeNotConnected(nodes, 0, 1, 1, 2);
	assertRangeConnected(nodes, &edge, 0, 1);
	assertRangeConnected(nodes, &edge, 1, 2);
}

BOOST_AUTO_TEST_CASE(TestNameConflict) {
	
	struct MyNode : public EulerTourTreeNode {
		
		bool operator==(MyNode& other) {
			return this == &other;
		}
		
		int activeOccurrence;
	};
	
	struct MyEdge : public EulerTourTreeEdge {
		
		bool operator==(MyEdge& other) {
			return this == &other;
		}
		
		int occurrence1;
		int occurrence2;
		int occurrence3;
		int occurrence4;
		
	};
	
	using ETT = EulerTourTreeAlgorithm<MyNode, MyEdge>;

	MyNode nodes[2];
	BOOST_REQUIRE(*ETT::nodeContainerView(nodes[0]).begin() == nodes[0]);
	BOOST_REQUIRE(*ETT::nodeContainerView(const_cast<MyNode&>(nodes[0])).begin() == nodes[0]);
	MyEdge edge;
	ETT::createEdge(nodes[0], nodes[1], edge);
	BOOST_REQUIRE(ETT::hasPath(nodes[0], nodes[1]));
	BOOST_REQUIRE(ETT::clusterSize(nodes[0]) == 2);
	BOOST_REQUIRE(ETT::clusterSize(nodes[1]) == 2);
	BOOST_REQUIRE(*ETT::edgeContainerView(nodes[0]).begin() == edge);
	BOOST_REQUIRE(*ETT::edgeContainerView(const_cast<MyNode&>(nodes[0])).begin() == edge);
	ETT::deleteEdge(edge);
	BOOST_REQUIRE(! ETT::hasPath(nodes[0], nodes[1]));
	BOOST_REQUIRE(ETT::clusterSize(nodes[0]) == 1);
	BOOST_REQUIRE(ETT::isClusterRep(nodes[0]));
	BOOST_REQUIRE(&ETT::findClusterRep(nodes[0]) == nodes + 0);
	BOOST_REQUIRE(ETT::clusterSize(nodes[1]) == 1);
	BOOST_REQUIRE(ETT::isClusterRep(static_cast<const MyNode&>(nodes[1])));
	BOOST_REQUIRE(&ETT::findClusterRep(static_cast<const MyNode&>(nodes[1])) == nodes + 1);
}

BOOST_AUTO_TEST_CASE(TestManyNodesTreeA) {
	for(unsigned int n = 3; n < 16; ++n) {
		unique_ptr<MyNode[]> nodes(makeNodes(n));
		unique_ptr<MyEdge[]> edges(makeEdges(n - 1));
		for(unsigned int i = 0; i < n - 1; ++i) {
			ETT::createEdge(nodes[i], nodes[i + 1], edges[i]);
			assertRangeConnected(nodes.get(), edges.get(), 0, i + 2);
			assertRangeNotConnected(nodes.get(), 0, i + 2, i + 2, n);
		}
		for(unsigned int i = n - 1; i > 0; --i) {
			ETT::deleteEdge(edges[i - 1]); //deleting the edge between i - 1 and i.
			assertRangeConnected(nodes.get(), edges.get(), 0, i);
			BOOST_REQUIRE(toNodeSet(nodes[i]) == set<const MyNode*>{nodes.get() + i});
			assertRangeNotConnected(nodes.get(), 0, i, i, n);
			BOOST_REQUIRE(ETT::isClusterRep(nodes[i]));
			BOOST_REQUIRE(ETT::clusterSize(nodes[i]) == 1);
		}
	}
}

BOOST_AUTO_TEST_CASE(TestManyNodesTreeB) {
	const unsigned int n = 32;
	for(unsigned int i = 1; i < n; ++i) {
		unique_ptr<MyNode[]> nodes(makeNodes(n));
		unique_ptr<MyEdge[]> edges(makeEdges(n - 1));
		connectRange(nodes.get(), edges.get(), 0, i);
		connectRange(nodes.get(), edges.get(), i, n);
		assertRangeConnected(nodes.get(), edges.get(), 0, i);
		assertRangeConnected(nodes.get(), edges.get(), i, n);
		assertRangeNotConnected(nodes.get(), 0, i, i, n);
		ETT::createEdge(nodes[i - 1], nodes[i], edges[i - 1]);
		assertRangeConnected(nodes.get(), edges.get(), 0, n);
		for(unsigned int j = 0; j < n - 1; ++j) {
			ETT::deleteEdge(edges[j]); //deleting the edge between j and j + 1.
			assertRangeConnected(nodes.get(), edges.get(), 0, j + 1);
			assertRangeConnected(nodes.get(), edges.get(), j + 1, n);
			assertRangeNotConnected(nodes.get(), 0, j + 1, j + 1, n);
			ETT::createEdge(nodes[j], nodes[j + 1], edges[j]);
			assertRangeConnected(nodes.get(), edges.get(), 0, n);
		}
	}
}
