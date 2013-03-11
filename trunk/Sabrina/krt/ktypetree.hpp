#pragma once

#include "kmeta.hpp"

class KTypeTree
{
protected:
	enum nodebalance_t
	{
		NB_LEFT    = -1,
		NB_BALANCE =  0,
		NB_RIGHT   =  1,
	};

	union TypeOrd
	{
		TypeDef  type;
		uint64_t ord;
	};

	struct Node
	{
		TypeOrd       data;
		Node         *left;
		Node         *right;
		nodebalance_t balance;
	};

protected:
	Node *root;
	Node *inserted;
	knuint_t size;

protected:
	static int compare(const TypeOrd &a, const TypeOrd &b);

	static void rotateLeft(Node *&node);
	static void rotateRight(Node *&node);
	static void balanceLeft(Node *&node);
	static void balanceRight(Node *&node);

protected:
	bool addNode(const TypeOrd &ord, Node *&parentNode, bool &incrHeight);
	void freeNode(Node *&node);

public:
	KTypeTree(void);
	~KTypeTree(void);

	const TypeDef * add(ktypetag_t tag, kushort_t dim, const void *udt);
	knuint_t getSize(void) const;
};
