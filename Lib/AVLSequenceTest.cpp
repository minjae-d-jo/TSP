#define BOOST_TEST_MODULE AVLSequenceTest
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <memory>
#include <iterator>
#include <algorithm>
#include <utility>
#include "AVLSequence.hpp"

using namespace std;
using namespace Snu::Cnrc::AVLSequence;

struct Node : public AVLSequenceNodeMixin<Node> {
	
	Node()
	: name(0) {
	}
	
	explicit Node(unsigned int v)
	: name(v) {
	}
	
	//for convenience of test
	bool operator==(const Node& other) const {
		return this == &other;
	}
	
	//for convenience of test
	bool operator!=(const Node& other) const {
		return this != &other;
	}
	
	unsigned int name;
};

using ASTrait = AVLSequenceNodeTrait<Node>;
using AVLSeq = AVLSequenceAlgorithm<ASTrait>;

int showNode(const Node& n) {
	return n.name;
}

bool checkTreeSanity(const Node& n) {
	return AVLSeq::Debug::checkSanity(AVLSeq::findRoot(n), showNode);
}

unsigned int randomUInt(const unsigned int i) {
	static default_random_engine g((random_device())());
	uniform_int_distribution<unsigned int> dist(0, i);
	return dist(g);
}

bool randomBool() {
	static default_random_engine g((random_device())());
	static uniform_int_distribution<unsigned int> dist(0, 1);
	return dist(g) == 0;
}

Node* rangeA(const unsigned int n1, const unsigned int n2) {
	const unsigned int n = n2 - n1;
	Node* const nodes = new Node[n];
	for(unsigned int i = 0; i < n; ++i) {
		nodes[i].name = n1 + i;
	}
	for(unsigned int i = 0; i < n - 1; ++i) {
		AVLSeq::insertNodeAfter(nodes[i], nodes[i + 1]);
	}
	return nodes;
}

Node* rangeB(const unsigned int n1, const unsigned int n2) {
	const unsigned int n = n2 - n1;
	Node* const nodes = new Node[n];
	for(unsigned int i = 0; i < n; ++i) {
		nodes[i].name = n1 + i;
	}
	for(unsigned int i = n - 1; i > 0; --i) {
		AVLSeq::insertNodeBefore(nodes[i], nodes[i - 1]);
	}
	return nodes;
}

Node* rangeR(const unsigned int n1, const unsigned int n2) {
	const unsigned int n = n2 - n1;
	Node* const nodes = new Node[n];
	for(unsigned int i = 0; i < n - 1; ++i) {
		AVLSeq::insertNodeAfter(nodes[i], nodes[i + 1]);
	}
	if(n != 1) {
		for(unsigned int i = 0; i < n; ++i) {
			const unsigned int r1 = randomUInt(n - 1);
			AVLSeq::removeNode(nodes[r1]);
			unsigned int r2 = randomUInt(n - 1);
			while(r1 == r2) {
				r2 = randomUInt(n - 1);
			}
			AVLSeq::insertNodeAfter(nodes[r2], nodes[r1]);
		}
	}
	unsigned int nn = n1;
	for(Node& n : AVLSeq::containerView(AVLSeq::findRoot(nodes[0]))) {
		n.name = nn++;
	}
	return nodes;
}

vector<unsigned int> rangeVector(const unsigned int n1, const unsigned int n2) {
	vector<unsigned int> vec;
	for(unsigned int i = n1; i < n2; ++i) {
		vec.push_back(i);
	}
	return vec;
}

vector<unsigned int> reverseRangeVector(const unsigned int n1, const unsigned int n2) {
	vector<unsigned int> vec;
	for(unsigned int i = n1; i < n2; ++i) {
		vec.push_back(i);
	}
	reverse(vec.begin(), vec.end());
	return vec;
}

vector<unsigned int> toVector(const Node& n) {
	vector<unsigned int> v;
	AVLSeq::ConstContainerView s = AVLSeq::containerView(AVLSeq::findRoot(n));
	for_each(s.begin(), s.end(), [&](const Node& n){v.push_back(n.name);});
	return v;
}

void printTree(const Node& n) {
	AVLSeq::Debug::printTree(AVLSeq::findRoot(n), [](const Node& n){return n.name;});
}

BOOST_AUTO_TEST_CASE(TestSingleNodeSequence) {
	
	Node n(0);
	
	const unsigned int name1 = 10;
	const unsigned int name2 = 10;
	
	BOOST_REQUIRE(AVLSeq::size(n) == 1);
	BOOST_REQUIRE(AVLSeq::findRoot(n) == n);
	BOOST_REQUIRE(AVLSeq::findHead(n) == n);
	BOOST_REQUIRE(AVLSeq::findTail(n) == n);
	BOOST_REQUIRE(! AVLSeq::next(n));
	BOOST_REQUIRE(! AVLSeq::previous(n));
	
	{
		typedef AVLSeq::ContainerView CV;
		
		CV s = AVLSeq::containerView(n);
		BOOST_REQUIRE(*s.begin() == n);
		BOOST_REQUIRE(*s.rbegin() == n);
		BOOST_REQUIRE(++s.begin() == s.end());
		{
			CV::iterator i = s.begin();
			BOOST_REQUIRE(i->name = name1);
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.end());
			BOOST_REQUIRE(n.name == name1);
		}
		{
			const CV& rs = s;
			CV::const_iterator i = rs.begin();
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == rs.end());
		}
		{
			CV::reverse_iterator i = s.rbegin();
			BOOST_REQUIRE(i->name = name2);
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.rend());
			BOOST_REQUIRE(n.name == name2);
		}
		{
			const CV& rs = s;
			CV::const_reverse_iterator i = rs.rbegin();
			BOOST_REQUIRE(i->name == name2);
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == rs.rend());
		}
	}
	
	{
		typedef AVLSeq::ContainerView CV;
		CV s = AVLSeq::containerView(n);
		BOOST_REQUIRE(*s.cbegin() == n);
		BOOST_REQUIRE(*s.crbegin() == n);
		BOOST_REQUIRE(++s.cbegin() == s.cend());
		{
			CV::const_iterator i = s.cbegin();
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.cend());
		}
		{
			CV::const_reverse_iterator i = s.crbegin();
			BOOST_REQUIRE(i->name == name2);
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.crend());
		}
	}
	
	{
		const Node& cn = n;
		typedef AVLSeq::ConstContainerView CV;
		CV s = AVLSeq::containerView(cn);
		BOOST_REQUIRE(*s.begin() == n);
		BOOST_REQUIRE(*s.rbegin() == n);
		BOOST_REQUIRE(++s.begin() == s.end());
		{
			CV::const_iterator i = s.begin();
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.end());
		}
		{
			CV::const_reverse_iterator i = s.rbegin();
			BOOST_REQUIRE(i->name == name2);
			BOOST_REQUIRE(*i++ == n);
			BOOST_REQUIRE(i == s.rend());
		}
	}
}

template<class...T>
bool checkSequence(const Node& n, T&&... t) {
	vector<unsigned int> v1{t...};
	vector<unsigned int> v2;
	AVLSeq::ConstContainerView s = AVLSeq::containerView(AVLSeq::findRoot(n));
	for_each(s.begin(), s.end(), [&](const Node& m){v2.push_back(m.name);});
	return v1 == v2;
}

BOOST_AUTO_TEST_CASE(TestTwoNodesSequence) {
	{
		Node n1(1), n2(2);
		AVLSeq::insertNodeAfter(n1, n2);
		
		BOOST_REQUIRE(checkTreeSanity(n1));
		BOOST_REQUIRE(AVLSeq::findRoot(n1) == AVLSeq::findRoot(n2));
		
		BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(n1)) == 2);
		BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(n1)) == n1);
		BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(n1)) == n2);
		
		BOOST_REQUIRE(n2 == *AVLSeq::next(n1));
		BOOST_REQUIRE(n1 == *AVLSeq::previous(n2));
		BOOST_REQUIRE(! AVLSeq::previous(n1));
		BOOST_REQUIRE(! AVLSeq::next(n2));
		
		BOOST_REQUIRE(checkSequence(n1, 1u, 2u));
	}
	
	{
		Node n1(1), n2(2);
		AVLSeq::insertNodeBefore(n2, n1);
		
		BOOST_REQUIRE(checkTreeSanity(n1));
		BOOST_REQUIRE(AVLSeq::findRoot(n1) == AVLSeq::findRoot(n2));
		
		BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(n1)) == 2);
		BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(n1)) == n1);
		BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(n1)) == n2);
		
		BOOST_REQUIRE(n2 == *AVLSeq::next(n1));
		BOOST_REQUIRE(n1 == *AVLSeq::previous(n2));
		BOOST_REQUIRE(! AVLSeq::previous(n1));
		BOOST_REQUIRE(! AVLSeq::next(n2));
		
		BOOST_REQUIRE(checkSequence(n1, 1u, 2u));
	}
}

BOOST_AUTO_TEST_CASE(MoveOfNodeMixin) {
	Node n1(1), n2(2);
	AVLSeq::insertNodeBefore(n2, n1);
	
	Node n3(3);
	n3 = move(n1);
	BOOST_REQUIRE(checkTreeSanity(n1));
	BOOST_REQUIRE(checkTreeSanity(n3));
	BOOST_REQUIRE(n2 == *AVLSeq::next(n3));
	BOOST_REQUIRE(n3 == *AVLSeq::previous(n2));
	
	Node n4 = move(n2);
	BOOST_REQUIRE(checkTreeSanity(n2));
	BOOST_REQUIRE(checkTreeSanity(n4));
	BOOST_REQUIRE(n4 == *AVLSeq::next(n3));
	BOOST_REQUIRE(n3 == *AVLSeq::previous(n4));
}

BOOST_AUTO_TEST_CASE(TestNameConflict) {
	
	struct Node : public AVLSequenceNodeMixin<Node> {
		bool operator==(const Node& other) const {
			return this == &other;
		}
		
		bool operator!=(const Node& other) const {
			return this != &other;
		}
	
		Node* parent;
		Node* leftChild;
		Node* rightChild;
		unsigned int height;
		unsigned int volume;
	};
	
	using ASTrait = AVLSequenceNodeTrait<Node>;
	using AVLSeq = AVLSequenceAlgorithm<ASTrait>;
	
	auto checkTreeSanity = [](const Node& n) {
		return AVLSeq::Debug::checkSanity(AVLSeq::findRoot(n), [](const Node &n){ return &n; });
	};
	
	{
		Node n1, n2;
		AVLSeq::insertNodeAfter(n1, n2);
		
		n1.parent = &n2;
		n1.leftChild = &n1;
		n1.rightChild = &n1;
		n1.height = 0;
		n1.volume = 0;
		
		n2.parent = &n1;
		n2.leftChild = &n2;
		n2.rightChild = &n2;
		n2.height = 0;
		n2.volume = 0;
		
		BOOST_REQUIRE(checkTreeSanity(n1));
		BOOST_REQUIRE(AVLSeq::findRoot(n1) == AVLSeq::findRoot(n2));
		
		BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(n1)) == 2);
		BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(n1)) == n1);
		BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(n1)) == n2);
		
		BOOST_REQUIRE(n2 == *AVLSeq::next(n1));
		BOOST_REQUIRE(n1 == *AVLSeq::previous(n2));
		BOOST_REQUIRE(! AVLSeq::previous(n1));
		BOOST_REQUIRE(! AVLSeq::next(n2));
	}
}

BOOST_AUTO_TEST_CASE(TestThreeNodesSequence) {
	{
		Node n1(1), n2(2), n3(3);
		AVLSeq::insertNodeAfter(n1, n2);
		AVLSeq::insertNodeAfter(n2, n3);
		
		BOOST_REQUIRE(checkTreeSanity(n1));
		BOOST_REQUIRE(AVLSeq::findRoot(n1) == AVLSeq::findRoot(n2));
		BOOST_REQUIRE(AVLSeq::findRoot(n2) == AVLSeq::findRoot(n3));
		
		BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(n1)) == 3);
		BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(n1)) == n1);
		BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(n1)) == n3);
		
		BOOST_REQUIRE(n2 == *AVLSeq::next(n1));
		BOOST_REQUIRE(n3 == *AVLSeq::next(n2));
		BOOST_REQUIRE(n1 == *AVLSeq::previous(n2));
		BOOST_REQUIRE(n2 == *AVLSeq::previous(n3));
		BOOST_REQUIRE(! AVLSeq::previous(n1));
		BOOST_REQUIRE(! AVLSeq::next(n3));
		
		BOOST_REQUIRE(checkSequence(n1, 1u, 2u, 3u));
	}
	
	{
		Node n1(1), n2(2), n3(3);
		AVLSeq::insertNodeBefore(n3, n2);
		AVLSeq::insertNodeBefore(n2, n1);
		
		BOOST_REQUIRE(checkTreeSanity(n1));
		BOOST_REQUIRE(AVLSeq::findRoot(n1) == AVLSeq::findRoot(n2));
		BOOST_REQUIRE(AVLSeq::findRoot(n2) == AVLSeq::findRoot(n3));
		
		BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(n1)) == 3);
		BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(n1)) == n1);
		BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(n1)) == n3);
		
		BOOST_REQUIRE(n2 == *AVLSeq::next(n1));
		BOOST_REQUIRE(n3 == *AVLSeq::next(n2));
		BOOST_REQUIRE(n1 == *AVLSeq::previous(n2));
		BOOST_REQUIRE(n2 == *AVLSeq::previous(n3));
		BOOST_REQUIRE(! AVLSeq::previous(n1));
		BOOST_REQUIRE(! AVLSeq::next(n3));
		
		BOOST_REQUIRE(checkSequence(n1, 1u, 2u, 3u));
	}
}

BOOST_AUTO_TEST_CASE(TestManyNodesSequenceA) {
	const unsigned int N = 64;
	for(unsigned int i = 3; i <= N; ++i) {
		{
			unique_ptr<Node[]> nodes(rangeA(0, i));
			BOOST_REQUIRE(rangeVector(0, i) == toVector(nodes[0]));
			for(unsigned int j = 0; j < i; ++j) {
				BOOST_REQUIRE(AVLSeq::findRoot(nodes[j]) == AVLSeq::findRoot(nodes[0]));
				BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(nodes[j])) == nodes[0]);
				BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(nodes[j])) == nodes[i - 1]);
				BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(nodes[j])) == i);
			}
		}
		{
			unique_ptr<Node[]> nodes(rangeB(0, i));
			BOOST_REQUIRE(rangeVector(0, i) == toVector(nodes[0]));
			for(unsigned int j = 0; j < i; ++j) {
				BOOST_REQUIRE(AVLSeq::findRoot(nodes[j]) == AVLSeq::findRoot(nodes[0]));
				BOOST_REQUIRE(AVLSeq::findHead(AVLSeq::findRoot(nodes[j])) == nodes[0]);
				BOOST_REQUIRE(AVLSeq::findTail(AVLSeq::findRoot(nodes[j])) == nodes[i - 1]);
				BOOST_REQUIRE(AVLSeq::size(AVLSeq::findRoot(nodes[j])) == i);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(TestManyNodesSequenceB) {
	const unsigned int N = 64;
	for(unsigned int i = 3; i <= N; ++i) {
		{
			unique_ptr<Node[]> nodes(rangeA(0, i));
			AVLSeq::ContainerView s = AVLSeq::containerView(AVLSeq::findRoot(nodes[0]));
			vector<unsigned int> vec;
			for_each(s.rbegin(), s.rend(), [&](const Node& n){vec.push_back(n.name);});
			BOOST_REQUIRE(reverseRangeVector(0, i) == vec);
		}
		{
			unique_ptr<Node[]> nodes(rangeB(0, i));
			AVLSeq::ContainerView s = AVLSeq::containerView(AVLSeq::findRoot(nodes[0]));
			vector<unsigned int> vec;
			for_each(s.rbegin(), s.rend(), [&](const Node& n){vec.push_back(n.name);});
			BOOST_REQUIRE(reverseRangeVector(0, i) == vec);
		}
	}
}

template<class MakeSequence>
void testRemoveNode(const unsigned int n, MakeSequence makeSequence) {
	for(unsigned int i = 0; i < n; ++i) {
		unique_ptr<Node[]> nodes(makeSequence(0, n));
		AVLSeq::removeNode(nodes[i]);
		vector<unsigned int> vec = rangeVector(0, n);
		vec.erase(vec.begin() + nodes[i].name);
		BOOST_REQUIRE(checkTreeSanity(nodes[i == 0 ? 1 : 0]));
		BOOST_REQUIRE(toVector(nodes[i == 0 ? 1 : 0]) == vec);
	}
}

BOOST_AUTO_TEST_CASE(TestSimpleRemove) {
	for(unsigned int i = 2; i <= 64; ++i) {
		testRemoveNode(i, rangeA);
		testRemoveNode(i, rangeB);
		for(unsigned int z = 0; z < 2 * i; ++z) {
			testRemoveNode(i, rangeR);
		}
	}
}

BOOST_AUTO_TEST_CASE(TestWithRandomInserstAndRemoves) {
	for(unsigned int i = 0; i < 100; ++i) {
		const unsigned int N = 128;
		unique_ptr<Node[]> nodes(rangeA(0, N));
		vector<unsigned int> vec = rangeVector(0, N);
		BOOST_REQUIRE(checkTreeSanity(nodes[0]));
		BOOST_REQUIRE(vec == toVector(nodes[0]));
		for(unsigned int j = 0; j <= 64; ++j) {
			vector<Node*> removed;
			const unsigned int K = randomUInt(N - 1);
			for(unsigned int k = 0; k < K; ++k) {
				unsigned int r = randomUInt(N - 1);
				if(r != 0 && AVLSeq::size(AVLSeq::findRoot(nodes[r])) != 1) {
					AVLSeq::removeNode(nodes[r]);
					vec.erase(find(vec.begin(), vec.end(), nodes[r].name));
					removed.push_back(nodes.get() + r);
				}
			}
			BOOST_REQUIRE(checkTreeSanity(nodes[0]));
			BOOST_REQUIRE(vec == toVector(nodes[0]));
			while(! removed.empty()) {
				Node& n = *removed.back();
				unsigned int r = randomUInt(N - 1);
				if(AVLSeq::size(AVLSeq::findRoot(nodes[r])) != 1) {
					AVLSeq::insertNodeBefore(nodes[r], n);
					vec.insert(find(vec.begin(), vec.end(), nodes[r].name), n.name);
					removed.pop_back();
				}
			}
			BOOST_REQUIRE(checkTreeSanity(nodes[0]));
			BOOST_REQUIRE(vec == toVector(nodes[0]));
		}
	}
}

template<class MakeSequence>
void testJoin(const int n1, const int n2, MakeSequence makeSequence) {
	{
		unique_ptr<Node[]> nodes1(makeSequence(0, n1)); 
		unique_ptr<Node[]> nodes2(makeSequence(n1, n1 + n2));
		AVLSeq::join(AVLSeq::findTail(nodes1[0]), AVLSeq::findHead(nodes2[0]));
		vector<unsigned int> vec = rangeVector(0, n1 + n2);
		BOOST_REQUIRE(checkTreeSanity(nodes1[0]));
		BOOST_REQUIRE(vec == toVector(nodes1[0]));
	}
}

BOOST_AUTO_TEST_CASE(TestJoin) {
	const unsigned int N = 64;
	for(unsigned int i = 1; i < N; ++i) {
		for(unsigned int j = 1; j < N; ++j) {
			testJoin(i, j, rangeA);
			testJoin(i, j, rangeB);
			for(unsigned int z = 0; z < 2 * (i + j); ++z) {
				testJoin(i, j, rangeR);
			}
		}
	}
}

template<class MakeSequence>
void testSplitBefore(const unsigned int size, const unsigned int at, MakeSequence makeSequence) {
	unique_ptr<Node[]> nodes(makeSequence(0, size));
	Node* a = find_if(nodes.get(), nodes.get() + size, [&](const Node& n){ return n.name == at; });
	auto b = AVLSeq::previous(*a);
	AVLSeq::splitBefore(*a);
	if(b) {
		BOOST_REQUIRE(checkTreeSanity(*b));
		BOOST_REQUIRE(rangeVector(0, at) == toVector(*b));
	}
	BOOST_REQUIRE(checkTreeSanity(*a));
	BOOST_REQUIRE(rangeVector(at, size) == toVector(*a));
}

template<class MakeSequence>
void testSplitAfter(const unsigned int size, const unsigned int at, MakeSequence makeSequence) {
	unique_ptr<Node[]> nodes(makeSequence(0, size));
	Node* a = find_if(nodes.get(), nodes.get() + size, [&](const Node& n){ return n.name == at; });
	auto b = AVLSeq::next(*a);
	AVLSeq::splitAfter(*a);
	BOOST_REQUIRE(checkTreeSanity(*a));
	BOOST_REQUIRE(rangeVector(0, at + 1) == toVector(*a));
	if(b) {
		BOOST_REQUIRE(checkTreeSanity(*b));
		BOOST_REQUIRE(rangeVector(at + 1, size) == toVector(*b));
	}
}

BOOST_AUTO_TEST_CASE(TestSplit) {
	for(unsigned int i = 2; i <= 64; ++i) {
		for(unsigned int j = 0; j < i; ++j) {
			testSplitBefore(i, j, rangeA);
			testSplitBefore(i, j, rangeB);
			testSplitAfter(i, j, rangeA);
			testSplitAfter(i, j, rangeB);
			for(unsigned int z = 0; z < 2 * i; ++z) {
				testSplitBefore(i, j, rangeR);
				testSplitAfter(i, j, rangeR);
			}
		}
	}
}
