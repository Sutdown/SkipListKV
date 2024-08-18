﻿// SkipListKV.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<string>
#include<set>
#include<time.h>

using namespace std;

template<typename T>
struct Less {
	bool operator () (const T& a, const T& b) const {
		return a < b;
	}
};

template<typename K, typename V, typename Comp = Less<K>>
class skip_list {
private:
	struct skip_list_node {
		int level; // 节点层数
		const K key;
		V value;
		skip_list_node** forward; // 指针数组，指向不同层次上的下一个节点

		skip_list_node() : key{ 0 }, value{ 0 }, level{ 0 }, forward{ 0 } {}
		skip_list_node(K k, V v, int l, skip_list_node* nxt = nullptr) : key(k), value(v), level(l) {
			forward = new skip_list_node * [level + 1];
			for (int i = 0; i <= level; ++i)forward[i] = nxt;
		}
		~skip_list_node() { delete[] forward; }
	};

	using node = skip_list_node;

	void init() {
		srand((uint32_t)time(NULL));
		level = length = 0;
		head->forward = new node * [MAXL + 1]; // 最大长度32+1
		for (int i = 0; i <= MAXL; i++)head->forward[i] = tail;
	}

	// 生成随机跳表层数，平衡跳表高度，提高查找插入效率。
	int randomLevel() {
		int lv = 1;
		while ((rand() & S) < PS)++lv;
		return MAXL > lv ? lv : MAXL;
	}

	int level;
	int length;
	static const int MAXL = 32;
	static const int P = 4;
	static const int S = 0xFFFF;
	static const int PS = S / P;
	static const int INVALID = INT_MAX;
	node* head, * tail;
	Comp less;

	node* find(const K& key, node** update) {
		node* p = head;
		for (int i = level; i >= 0; i--) {
			/*某一层，逐步转向下一个节点*/
			while (p->forward[i] != tail && less(p->forward[i]->key, key)) {
				p = p->forward[i];
			}
			update[i] = p;
		}
		p = p->forward[0];
		return p;
	}

public:
	struct Iter {
		node* p;

		Iter() : p(nullptr) {};
		Iter(node* rhs) : p(rhs) {};

		node* operator ->() const { return (p); }
		node& operator *() const { return *p; }

		bool operator ==(const Iter& rhs) { return rhs.p == p; }
		bool operator !=(const Iter& rhs) { return !(rhs.p == p); }
		void operator ++() { p = p->forward[0]; }
		void operator ++(int) { p = p->forward[0]; }
	};

	skip_list() : head(new node()), tail(new node()), less{ Comp() } {
		init();
	}
	skip_list(Comp _less) : head(new node()), tail(new node()), less{ _less } {
		init();
	}

	void insert(const K& key, const V& value) {
		node* update[MAXL + 1];
		node* p = find(key, update);
		if (p->key == key) {
			p->value = value;
			return;
		}

		int lv = randomLevel();
		if (lv > level) {
			lv = ++level;
			update[lv] = head;
		}

		node* newNode = new node(key, value, lv);
		for (int i = lv; i >= 0; i--) {
			p = update[i];
			newNode->forward[i] = p->forward[i];
			p->forward[i] = newNode;
		}
		++length;
	}

	bool erase(const K& key) {
		node* update[MAXL + 1];
		node* p = find(key, update);
		if (p->key != key)return false;

		for (int i = 0; i <= p->level; ++i) {
			update[i]->forward[i] = p->forward[i];
		}

		delete p;
		while (level > 0 && head->forward[level] == tail)--level;
		--length;
		return true;
	}

	Iter find(const K& key) {
		node* update[MAXL + 1];
		node* p = find(key, update);
		if (p == tail)return tail;
		if (p->key != key)return tail;
		return Iter(p);
	}

	bool count(const K& key) {
		node* update[MAXL + 1];
		node* p = find(key, update);
		if (p == tail)return false;
		return key == p->key;
	}

	Iter end() {
		return Iter(tail);
	}

	Iter begin() {
		return Iter(head->forward[0]);
	}
};

int main() {
	{
		//使用lambda
		auto cmp = [](const string& a, const string& b) {return a.length() < b.length(); };
		skip_list < string, int, decltype(cmp)> list(cmp);
		list.insert("aab", 1321);
		list.insert("hello", 54342);
		list.insert("world", 544);
		for (auto it = list.begin(); it != list.end(); it++) {
			cout << it->key << " " << it->value << endl;
		}
	}

	cout << "==================================" << endl;

	{
		//默认小于号
		skip_list<int, int>list;
		list.insert(1, 3); list.insert(1, 3); list.insert(4, 3);
		list.insert(5, 3); list.insert(1, 3); list.insert(4, 3);
		for (auto it = list.begin(); it != list.end(); it++) {
			cout << it->key << " " << it->value << endl;
		}
	}
	return 0;
}