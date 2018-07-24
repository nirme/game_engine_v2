#pragma once

#ifndef _REFERENCE_COUNTING_OBJECT
#define _REFERENCE_COUNTING_OBJECT


class ReferenceCountingObject
{
private:
	int i_refCount;

public:

	ReferenceCountingObject() : i_refCount(1) {};
	~ReferenceCountingObject(){};

	inline int refCount() const { return i_refCount; };
	inline void addRef() { ++i_refCount; };
	inline void release() { if ( !( --i_refCount ) ) delete this; };
};

#endif _REFERENCE_COUNTING_OBJECT
