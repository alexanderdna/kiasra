#pragma once

#include <stack>
#include <set>
#include <cstdint>
#include <cstddef>

#include "kni.h"

class KEnvironment;
class KObject;

class KGC
{
	friend class KEnvironment;

protected:
	enum phase_t : uint8_t
	{
		KGC_IDLE  = 0,
		KGC_MARK  = 1,
		KGC_SWEEP = 2,
	};

	struct Header
	{
		Header *next;
		size_t  len;
		uint8_t mark; //low nibble: 0 for white, 1 for black; high nibble: 0 for class/delegate/array, 1 for evalstack.
	};

	typedef std::  set<Header *> rootset_t;
	typedef std::stack<Header *> greystack_t;

protected:
	const KEnvironment *env;

	phase_t phase;
	size_t allocAmount;
	size_t allocCount;

	rootset_t   *rootset;
	greystack_t *greystack;

	Header *gchead;
	Header *gcsweeping;
	Header *gcswept;

protected:
	void doInit(void);
	void doMark(void);
	void doSweep(void);
	void propagate(void);

public:
	bool paused;

public:
	KGC(void);
	~KGC(void);

	// Gets the amount of objects allocated.
	size_t getAllocAmount() const;

	// Gets the number of arrays allocated.
	size_t getAllocCount() const;
	
	// Allocates an array of objects.
	KObject * alloc(knuint_t len);

	// Allocates an array of objects for use in a stack.
	KObject * allocForStack(knuint_t len);

	// Adds an array to the root set.
	void addRoot(KObject *p);

	// Removes an array from the root set.
	void removeRoot(KObject *p);

	// Write-barrier
	void mark(KObject *p);

	// Performs a GC step.
	void step(void);
	
	// Forces garbage collection.
	void collect(void);

#ifdef _DEBUG
	void inspect(void);
#endif
};
