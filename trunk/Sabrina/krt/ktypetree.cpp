#include "ktypetree.hpp"

#include "kmeta.hpp"
#include "kconfig.hpp"

//public
KTypeTree::KTypeTree(void)
	: root(NULL), inserted(NULL), size(0)
{
}

//public
KTypeTree::~KTypeTree(void)
{
	this->freeNode(this->root);
}

//protected static
int KTypeTree::compare(const TypeOrd &a, const TypeOrd &b)
{
	uint64_t oa = a.ord, ob = b.ord;
#ifdef IS64BIT
	if (oa > ob)
		return 1;
	else if (oa < ob)
		return -1;
	else
		return (a.type.cls == b.type.cls) ? 0 : ((a.type.cls > b.type.cls) ? 1 : -1);
#else
	return (oa == ob) ? 0 : ((oa > ob) ? 1 : -1);
#endif
}

//protected static
void KTypeTree::rotateLeft(Node *&node)
{
	Node *r = node->right;
	node->right = r->left;
	r->left = node;
	node = r;
}

//protected static
void KTypeTree::rotateRight(Node *&node)
{
	Node *l = node->left;
	node->left = l->right;
	l->right = node;
	node = l;
}

//protected static
void KTypeTree::balanceLeft(Node *&node)
{
	if (node->left->balance == NB_LEFT)
	{
		rotateRight(node);
		node->balance = NB_BALANCE;
		node->right->balance = NB_BALANCE;
	}
	else if (node->left->balance == NB_RIGHT)
	{
		rotateLeft(node->left);
		rotateRight(node);

		if (node->balance == NB_LEFT)
		{
			node->left->balance = NB_LEFT;
			node->right->balance = NB_RIGHT;
		}
		if (node->balance == NB_RIGHT)
		{
			node->left->balance = NB_LEFT;
			node->right->balance = NB_BALANCE;
		}
		else
		{
			node->left->balance = NB_BALANCE;
			node->right->balance = NB_BALANCE;
		}

		node->balance = NB_BALANCE;
	}
}

//protected static
void KTypeTree::balanceRight(Node *&node)
{
	if (node->right->balance == NB_RIGHT)
	{
		rotateLeft(node);
		node->balance = NB_BALANCE;
		node->left->balance = NB_BALANCE;
	}
	else if (node->right->balance == NB_LEFT)
	{
		rotateRight(node->right);
		rotateLeft(node);

		if (node->balance == NB_LEFT)
		{
			node->left->balance = NB_BALANCE;
			node->right->balance = NB_RIGHT;
		}
		else if (node->balance == NB_RIGHT)
		{
			node->left->balance = NB_LEFT;
			node->right->balance = NB_RIGHT;
		}
		else
		{
			node->left->balance = NB_BALANCE;
			node->right->balance = NB_BALANCE;
		}

		node->balance = NB_BALANCE;
	}
}

//protected
bool KTypeTree::addNode(const TypeOrd &ord, Node *&parentNode, bool &incrHeight)
{
	if (parentNode == NULL)
	{
		Node *_node = new Node;
		_node->data = ord;
		_node->left = NULL;
		_node->right = NULL;
		_node->balance = NB_BALANCE;
		parentNode = _node;

		incrHeight = true;
		this->inserted = _node;

		return true;
	}
	else
	{
		if (KTypeTree::compare(ord, parentNode->data) < 0)
		{
			if (!this->addNode(ord, parentNode->left, incrHeight))
				return false;

			if (incrHeight)
			{
				if (parentNode->balance == NB_LEFT)
				{
					balanceLeft(parentNode);
					incrHeight = false;
				}
				else if (parentNode->balance == NB_RIGHT)
				{
					parentNode->balance = NB_BALANCE;
					incrHeight = false;
				}
				else
				{
					parentNode->balance = NB_LEFT;
					incrHeight = true;
				}
			}

			return true;
		}
		else if (KTypeTree::compare(ord, parentNode->data) > 0)
		{
			if (!this->addNode(ord, parentNode->right, incrHeight))
				return false;

			if (incrHeight)
			{
				if (parentNode->balance == NB_LEFT)
				{
					parentNode->balance = NB_BALANCE;
					incrHeight = false;
				}
				else if (parentNode->balance == NB_RIGHT)
				{
					balanceRight(parentNode);
					incrHeight = false;
				}
				else
				{
					parentNode->balance = NB_RIGHT;
					incrHeight = true;
				}
			}

			return true;
		}
		else
		{
			incrHeight = false;
			this->inserted = parentNode;
			return false;
		}
	}
}

//protected
void KTypeTree::freeNode(Node *&node)
{
	if (node != NULL)
	{
		this->freeNode(node->left);
		this->freeNode(node->right);

		delete node;
		node = NULL;
	}
}

//public
const TypeDef * KTypeTree::add(ktypetag_t tag, kushort_t dim, const void *udt)
{
	TypeDef type = { tag, dim };
	type.cls = (ClassDef *)udt;
	type.size = sizeof(TypeDef);

	TypeOrd ord = { };
	ord.type = type;

	bool incrHeight = false;
	if (this->addNode(ord, this->root, incrHeight))
		++this->size;

	return &this->inserted->data.type;
}

//public
knuint_t KTypeTree::getSize(void) const
{
	return size;
}
