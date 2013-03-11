#include "kgc.hpp"

#include "kenv.hpp"
#include "kobject.hpp"

#include <cstdlib>

#define assert(v) ((void)0)

#define IS_WHITE(hdr) ((hdr->mark & 0x01) == 0u)
#define IS_BLACK(hdr) ((hdr->mark & 0x01) == 1u)
#define IS_ARRAY(hdr) (((hdr->mark & 0x10) >> 4) == 0u)
#define IS_STACK(hdr) (((hdr->mark & 0x10) >> 4) == 1u)

#define SET_WHITE(hdr) hdr->mark = (uint8_t)(hdr->mark & ~1u)
#define SET_BLACK(hdr) hdr->mark = (uint8_t)(hdr->mark |  1u)
#define SET_ARRAY(hdr) hdr->mark = (uint8_t)(hdr->mark & ~(1u << 4))
#define SET_STACK(hdr) hdr->mark = (uint8_t)(hdr->mark |  (1u << 4))

#define ALLOC(len)       ((char *)malloc(sizeof(Header) + sizeof(KObject) * len))

#define HEADER_FROM_ARRAY(arr) ((Header *)((char *)arr - sizeof(Header)))
#define ARRAY_FROM_HEADER(hdr) ((KObject *)((char *)hdr + sizeof(Header)))

#define MAX_MARK	5
#define MAX_SWEEP	5

#define IS_REF(type)    (type->dim || ((type->tag & KT_SCALAR_MASK) == KT_CLASS || (type->tag & KT_SCALAR_MASK) == KT_DELEGATE))

KGC::KGC(void)
	: paused(false), phase(KGC_IDLE),
	allocAmount(0), allocCount(0),
	gchead(NULL), gcsweeping(NULL), gcswept(NULL)
{
	this->rootset = new rootset_t;
	this->greystack = new greystack_t;

	KObject::gc = const_cast<KGC *>(this);
}


KGC::~KGC(void)
{
	if (this->rootset)
	{
		delete this->rootset;
		this->rootset = NULL;
	}

	if (this->greystack)
	{
		delete this->greystack;
		this->greystack = NULL;
	}

	Header *h = this->gchead;
	while (h)
	{
		// perform clean-up on left-over objects
		if (IS_ARRAY(h))
		{
			KObject *arr = ARRAY_FROM_HEADER(h);
			size_t len = h->len;
			for (size_t i = 0; i < len; ++i)
				arr[i].clean();
		}

		char *p = (char *)h;
		h = h->next;
		free(p);
	}
	this->gchead = NULL;
}

//public
size_t KGC::getAllocAmount() const
{
	return this->allocAmount;
}

//public
size_t KGC::getAllocCount() const
{
	return this->allocCount;
}

//public
KObject * KGC::alloc(size_t len)
{
	char *mem = ALLOC(len);
	if (!mem && this->phase == KGC_SWEEP)
	{
		this->doSweep();
		mem = ALLOC(len);
	}

	if (!mem)
		return NULL;

	this->allocAmount += len;
	++this->allocCount;

	Header *hdr = (Header *)mem;
	hdr->next = this->gchead;
	hdr->len = len;
	hdr->mark = 0;

	this->gchead = hdr;

	KObject *arr = ARRAY_FROM_HEADER(hdr);
	memset(arr, 0, sizeof(KObject) * len);

	return arr;
}

//public
KObject * KGC::allocForStack(size_t len)
{
	char *mem = ALLOC(len);
	if (!mem && this->phase == KGC_SWEEP)
	{
		this->doSweep();
		mem = ALLOC(len);
	}

	if (!mem)
		return NULL;

	this->allocAmount += len;
	++this->allocCount;

	Header *hdr = (Header *)mem;
	hdr->next = this->gchead;
	hdr->len = len;
	hdr->mark = 0;
	SET_STACK(hdr);

	this->gchead = hdr;

	KObject *arr = ARRAY_FROM_HEADER(hdr);
	memset(arr, 0, sizeof(KObject) * len);

	return arr;
}

//public
void KGC::addRoot(KObject *p)
{
	assert(p != NULL);
	this->rootset->insert(HEADER_FROM_ARRAY(p));
	this->mark(p);
}

//public
void KGC::removeRoot(KObject *p)
{
	assert(p != NULL);
	this->rootset->erase(HEADER_FROM_ARRAY(p));
}

//public
void KGC::mark(KObject *p)
{
	assert(p != NULL);

	Header *hdr = HEADER_FROM_ARRAY(p);
	if (IS_WHITE(hdr))
		return;

	if (this->phase == KGC_MARK)
	{
		this->greystack->push(hdr);
	}
	else if (this->phase == KGC_SWEEP)
	{
		this->greystack->push(hdr);
		while(!this->greystack->empty())
			this->propagate();
	}
}

//public
void KGC::step(void)
{
	if (this->paused)
		return;

	switch (this->phase)
	{
	case KGC_IDLE:
		doInit();
		break;
	case KGC_MARK:
		doMark();
		break;
	case KGC_SWEEP:
		doSweep();
		break;
	}
}
	
//public
void KGC::collect(void)
{
	if (this->phase == KGC_IDLE)
		this->doInit();
	
	while (this->phase == KGC_MARK)
		this->doMark();

	while (this->phase == KGC_SWEEP)
		this->doSweep();
}

//protected
void KGC::doInit(void)
{
	assert(!this->greystack->empty());

	if (this->gchead == NULL)
		return;

	this->gcswept = this->gchead;
	this->gcsweeping = this->gcswept->next;

	for (rootset_t::iterator it = this->rootset->begin(); it != this->rootset->end(); ++it)
		this->greystack->push(*it);

	this->phase = KGC_MARK;
}

//protected
void KGC::doMark(void)
{
	for (size_t i = 0; i < MAX_MARK; ++i)
	{
		if (this->greystack->empty())
		{
			phase = KGC_SWEEP;
			return;
		}
		this->propagate();
	}
}

//protected
void KGC::doSweep(void)
{
	for (size_t i = 0; i < MAX_SWEEP; ++i)
	{
		if (!this->gcsweeping)
		{
			this->gcswept = NULL;
			phase = KGC_IDLE;
			return;
		}

		Header *hdr = this->gcsweeping;
		if (IS_BLACK(hdr))
		{
			SET_WHITE(hdr);
			this->gcswept = hdr;
			this->gcsweeping = hdr->next;
		}
		else
		{
			this->gcswept->next = hdr->next;
			this->gcsweeping = hdr->next;

			KObject *arr = ARRAY_FROM_HEADER(hdr);
			size_t len = hdr->len;

			// we do not perform clean-up on stack elements
			if (IS_ARRAY(hdr))
				for (size_t i = 0; i < len; ++i)
					arr[i].clean();

			this->allocAmount -= len;
			--this->allocCount;

			free((char *)hdr);
		}
	}
}

//protected
void KGC::propagate(void)
{
	assert(!this->greystack->empty());

	Header *hdr = this->greystack->top(); this->greystack->pop();
	if (IS_BLACK(hdr))
		return;

	SET_BLACK(hdr);

	KObject *arr = ARRAY_FROM_HEADER(hdr);
	size_t len;
	if (IS_STACK(hdr))
		len = this->env->stackPointer;
	else
		len = hdr->len;

	for (size_t i = 0; i < len; ++i)
	{
		KObject *elem = &arr[i];
		if (IS_REF(elem->type) && elem->vObj)
			this->greystack->push(HEADER_FROM_ARRAY(elem->vObj));
	}
}


#ifdef _DEBUG

#include <cstdio>
//public
void KGC::inspect(void)
{
	printf("\nMARK\tLEN\n----------------\n");
	Header *hdr = this->gchead;
	while (hdr)
	{
		printf("%02x\t%d\n", hdr->mark, hdr->len);
		hdr = hdr->next;
	}
	putchar('\n');
}

#endif //_DEBUG
