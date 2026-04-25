#include "SkipList.h"

namespace lsm {

	using Node = SkipList::Node;
	using iterator = SkipList::iterator;

	//Node functions.
	/*
	bool is_tombstone;
	std::string key;
	std::string val;

	std::vector<Node*> next;
	*/

	size_t Node::getSizeBytes() {
		size_t footprint = sizeof(Node) + key.size() + val.size() + (next.size() * sizeof(Node*));
		return footprint;
	};

	bool Node::isTombstone() const {
		return is_tombstone;
	}
	//End Node functions.

	//iterator functions.
	/*
	Node* ptr;
	*/

	std::string& iterator::operator*() {
		return ptr->val;
	}

    Node* iterator::operator->() {
        return ptr;
    }

	iterator iterator::operator++() {
		if (!ptr) {throw std::domain_error("Accessing out of bounds memory.");}
		return iterator(ptr->next[0]);
	}

	bool iterator::operator==(const iterator& other) const{
		return ptr == other.ptr;
	}

	bool iterator::operator!=(const iterator& other) const {
        return !(*this == other);
    }
    //End iterator functions.


    //SkipList functions.
    /*
    const size_t MAX_LVL = 20;
	const double PROBABILITY = 0.5;
	std::mt19937 gen;
	std::geometric_distribution<int> dist;

	size_t byte_counter;

	Node* header;
    */
    SkipList::SkipList() : gen(std::random_device{}()), dist(PROBABILITY) {
		header = new Node({}, {}, MAX_LVL);

		byte_counter = sizeof(SkipList) + getSize(header);
	}

	SkipList::~SkipList() {
		Node* curr = header;
		while (curr) {
			Node* next = curr->next[0];
			delete curr;
			curr = next;
		}
		byte_counter = 0;
	}


	Node* SkipList::getHeader() const {
		return header;
	}
 
	std::vector<Node*> SkipList::getPredecessors(const std::string& key) const {
		Node* curr = header;
		int curr_level = MAX_LVL;

		std::vector<Node*> res(MAX_LVL + 1);

		while (curr_level >= 0) {
			if (curr->next[curr_level] && curr->next[curr_level]->key < key) {
				curr = curr->next[curr_level];
			}
			else {
				res[curr_level] = curr;
				--curr_level;
			}
		}

		return res;
	}


	size_t SkipList::calcNodeLevel() {
		int res = dist(gen);
		if (res < MAX_LVL) {return res;}
		return MAX_LVL;
	}


	iterator SkipList::begin() const {
		return iterator(header->next[0]);
	}

	iterator SkipList::end() const {
		return iterator(nullptr);
	}

	iterator SkipList::search(const std::string& key) const {
		Node* req = getPredecessors(key)[0]->next[0];
		if (req && key == req->key) {
			return iterator(req);
		}

		return end();
	}

	/*bool SkipList::remove(const std::string& key) {
		auto predecessors = getPredecessors(key);
		Node* curr = predecessors[0]->next[0];

		if (!curr || curr->key != key) {
			return false;
		}

		int levels = curr->next.size() - 1;

		for (int i=levels; i>=0; --i) {
			predecessors[i]->next[i] = curr->next[i];
		}

		byte_counter -= getEntrySize(curr);
		delete curr;

		return true;
	}*/

	void SkipList::insert(bool insert_tombstone, const std::string& key, const std::string& val) {
		auto predecessors = getPredecessors(key);
		Node* next = predecessors[0]->next[0];

		if (next && key == next->key) {
			byte_counter -= getEntrySize(next);
			insertHelper(insert_tombstone, val, next);
			byte_counter += getEntrySize(next);
		}
		else {
			size_t max_level = calcNodeLevel();
			Node* to_add = new Node(key, val, max_level);

			insertHelper(insert_tombstone, val, to_add);

			byte_counter += getEntrySize(to_add);

			for (size_t i=0; i<=max_level; ++i) {
				to_add->next[i] = predecessors[i]->next[i];
				predecessors[i]->next[i] = to_add;
			}

		}
	}

	void SkipList::insertHelper(bool insert_tombstone, const std::string& val, Node* curr) {
		if (insert_tombstone) {
			curr->val = {};
			curr->is_tombstone = true;
		}
		else {
			curr->val = val;
			curr->is_tombstone = false;
		}
	}

	size_t SkipList::getEntrySize(const Node* curr) const {
		return curr->getSizeBytes();
	}

	size_t SkipList::getSizeBytes() const {
		return byte_counter;
	}

	//End SkipList functions.

}