//
// Red-Black Binary Search Tree (Balanced Tree)
//
// Copyright (C) 2017 by Luiz Lima Jr.
// Based on Cormen, chap. 13
//

#ifndef RBTREE_H
#define RBTREE_H

#include <stdexcept>

namespace colibry {

	template<typename T>
	class RBTree {
	public:
		// Data types
		enum class Color : char { RED, BLACK };
		class Node {
		private:
			RBTree* tree;	// owner tree (to access nil())
		public:
			T key;
			Node* left;
			Node* right;
			Node* parent;
			Color color;
		public:
			Node(RBTree* t) : tree{t} { left = right = parent = tree->nil(); }
			bool IsRoot() const { return parent == tree->nil(); }
			bool IsLeft() const { return (IsRoot() ? false : (this == parent->left)); }
			bool IsRight() const { return (IsRoot() ? false : (this == parent->right)); }
			RBTree* Tree() { return tree; }
		};
	protected:
		Node* m_sentinel;	// color is BLACK (other fields are immaterial)
		Node* m_root;
	protected:
		void RotateLeft(Node* n);
		void RotateRight(Node* n);
		void FixInsert(Node* nn);
		void Erase(Node* r);
	public:
		RBTree();
		virtual ~RBTree();

		virtual Node* Search(const T& k);
		virtual Node* Minimum(Node* n);
		virtual Node* Maximum(Node* n);
		virtual Node* Minimum() { return Minimum(m_root); }
		virtual Node* Maximum() { return Maximum(m_root); }
		virtual Node* Successor(Node* n);
		virtual Node* Predecessor(Node* n);
		virtual unsigned int Level(Node* n);
		virtual void Insert(const T& k);
		virtual void Remove();	// to do
		template<typename F>
		void Traverse(Node* r, F visit);
		// void Traverse(Node* r, F visit) {
		// 	if (r != nil()) {
		// 		Traverse(r->left,visit);
		// 		visit(r->key);
		// 		Traverse(r->right,visit);
		// 	}
		// }
		template<typename F>
		void Traverse(F visit) { Traverse(m_root,visit); }
		virtual bool Empty() const { return m_root == nil(); }
		virtual void Clear() { Erase(m_root); m_root = nil(); }
	public:
		inline Node* nil() const { return m_sentinel; }
	};

	// ------------------------------------------------------------

	template<typename T>
	RBTree<T>::RBTree()
	{
		m_sentinel = new Node{this};
		m_sentinel->color = Color::BLACK;
		m_sentinel->parent = m_sentinel->left
			= m_sentinel->right
			= m_sentinel;	// could be anything
		m_root = nil();
	}

	template<typename T>
	RBTree<T>::~RBTree()
	{
		delete m_sentinel;
		Erase(m_root);
	}

	template<typename T>
	void RBTree<T>::RotateLeft(Node* n)
	{
		Node* m = n->right;
		n->right = m->left;
		if (m->left != nil())
			m->left->parent = n;
		m->parent = n->parent;
		if (n->IsRoot())
			m_root = m;
		else if (n->IsLeft())
			n->parent->left = m;
		else
			n->parent->right = m;
		m->left = n;
		n->parent = m;
	}

	template<typename T>
	void RBTree<T>::RotateRight(Node* n)
	{
		Node* m = n->left;
		n->left = m->right;
		if (m->right != nil())
			m->right->parent = n;
		m->parent = n->parent;
		if (n->IsRoot())
			m_root = m;
		else if (n->IsLeft())
			n->parent->left = m;
		else
			n->parent->right = m;
		m->right = n;
		n->parent = m;
	}

	template<typename T>
	void RBTree<T>::FixInsert(Node* nn)
	{
		while (nn->parent->color == Color::RED) {
			if (nn->parent->IsLeft()) {
				Node* m = nn->parent->parent->right;
				if (m->color == Color::RED) {
					nn->parent->color = Color::BLACK;
					m->color = Color::BLACK;
					nn->parent->parent->color = Color::RED;
					nn = nn->parent->parent;
				} else {
					if (nn->IsRight()) {
						nn = nn->parent;
						RotateLeft(nn);
					}
					nn->parent->color = Color::BLACK;
					nn->parent->parent->color = Color::RED;
					RotateRight(nn->parent->parent);
				}
			} else {
				Node* m = nn->parent->parent->left;
				if (m->color == Color::RED) {
					nn->parent->color = Color::BLACK;
					m->color = Color::BLACK;
					nn->parent->parent->color = Color::RED;
					nn = nn->parent->parent;
				} else {
					if (nn->IsLeft()) {
						nn = nn->parent;
						RotateRight(nn);	// Left??
					}
					nn->parent->color = Color::BLACK;
					nn->parent->parent->color = Color::RED;
					RotateLeft(nn->parent->parent);	// Right??
				}
			}
		}
		m_root->color = Color::BLACK;
	}

	template<typename T>
	void RBTree<T>::Insert(const T& k)
	{
		Node* nn = new Node{this};
		nn->key = k;
		nn->left = nil();
		nn->right = nil();
		nn->color = Color::RED;

		Node* m = nil();
		Node* n = m_root;
		while (n != nil()) {
			m = n;
			if (nn->key < n->key)
				n = n->left;
			else
				n = n->right;
		}
		nn->parent = m;
		if (m == nil())
			m_root = nn;
		else if (m->key > nn->key)
			m->left = nn;
		else
			m->right = nn;
		FixInsert(nn);
	}

	template<typename T>
	typename RBTree<T>::Node* RBTree<T>::Search(const T& k)
	{
		Node* nd = m_root;
		while (nd != nil()) {
			if (nd->key == k)
				break;
			else if (nd->key < k)
				nd = nd->right;
			else
				nd = nd->left;
		}
		return nd;
	}

	template<typename T>
	typename RBTree<T>::Node* RBTree<T>::Minimum(Node* n)
	{
		if (n == nil())
			throw std::runtime_error{"Minimum: empty tree"};
		Node* nd = n;
		while (nd->left != nil())
			nd = nd->left;
		return nd;
	}

	template<typename T>
	typename RBTree<T>::Node* RBTree<T>::Maximum(Node* n)
	{
		if (n == nil())
			throw std::runtime_error{"Minimum: empty tree"};
		Node* nd = n;
		while (nd->right != nil())
			nd = nd->right;
		return nd;
	}

	template<typename T>
	typename RBTree<T>::Node* RBTree<T>::Successor(Node* n)
	{
		if (n == nil())
			throw std::runtime_error{"Successor(): n is nil"};
		if (n->right != nil())
			return Minimum(n->right);
		Node* m = n->parent;
		while (m != nil() && n == m->right) {
			n = m;
			m = m->parent;
		}
		return m;
	}

	template<typename T>
	typename RBTree<T>::Node* RBTree<T>::Predecessor(Node* n)
	{
		if (n == nil())
			throw std::runtime_error{"Predecessor(): n is nil"};
		if (n->left != nil())
			return Maximum(n->left);
		Node* m = n->parent;
		while (m != nil() && n == m->left) {
			n = m;
			m = m->parent;
		}
		return m;
	}

	template<typename T>
	unsigned int RBTree<T>::Level(Node* n)
	{
		if (n->IsRoot())
			return 0;
		else
			return 1 + Level(n->parent);
	}

	template<typename T>
	template<typename F>
	void RBTree<T>::Traverse(Node* r, F visit)
	{
		if (r != nil()) {
			Traverse(r->left,visit);
			visit(r->key);
			Traverse(r->right,visit);
		}
	}


	template<typename T>
	void RBTree<T>::Remove()
	{
	}

	template<typename T>
	void RBTree<T>::Erase(Node* r)
	{
		if (r != nil()) {
			Erase(r->left);
			Erase(r->right);
			delete r;
		}
	}

}; // end namespace colibry

#endif
