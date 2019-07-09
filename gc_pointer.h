#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
	Pointer implements a pointer type that uses
	garbage collection to release unused memory.
	A Pointer must only be used to point to memory
	that was dynamically allocated using new.
	When used to refer to an allocated array,
	specify the array size.
*/
template <class T, int size = 0>
class Pointer {
private:
	// refContainer maintains the garbage collection list.
	static std::list<PtrDetails<T> > refContainer;

	// addr points to the allocated memory to which
	// this Pointer pointer currently points.
	T *addr;

	/*  isArray is true if this Pointer points
		to an allocated array. It is false
		otherwise.
	*/
	bool isArray;

	// true if pointing to array
	// If this Pointer is pointing to an allocated
	// array, then arraySize contains its size.
	unsigned arraySize; // size of the array

	static bool first; // true when first Pointer is created

	// Return an iterator to pointer details in refContainer.
	typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);

public:
	// Define an iterator type for Pointer<T>.
	typedef Iter<T> GCiterator;

	// Empty constructor
	// NOTE: templates aren't able to have prototypes with default arguments
	// this is why constructor is designed like this:
	Pointer() {
		Pointer(NULL);
	}
	Pointer(T*);

	// Copy constructor.
	Pointer(const Pointer &);

	// Destructor for Pointer.
	~Pointer();

	// Collect garbage. Returns true if at least
	// one object was freed.
	static bool collect();

	// Overload assignment of pointer to Pointer.
	T *operator=(T *t);

	// Overload assignment of Pointer to Pointer.
	Pointer &operator=(Pointer &rv);

	// Return a reference to the object pointed
	// to by this Pointer.
	T &operator*() {
		return *addr;
	}

	// Return the address being pointed to.
	T *operator->() { return addr; }

	// Return a reference to the object at the
	// index specified by i.
	T &operator[](int i) { return addr[i]; }

	// Conversion function to T *.
	operator T *() { return addr; }

	// Return an Iter to the start of the allocated memory.
	Iter<T> begin() {
		int _size;
		if (isArray)
			_size = arraySize;
		else
			_size = 1;
		return Iter<T>(addr, addr, addr + _size);
	}

	// Return an Iter to one past the end of an allocated array.
	Iter<T> end() {
		int _size;
		if (isArray)
			_size = arraySize;
		else
			_size = 1;
		return Iter<T>(addr + _size, addr, addr + _size);
	}

	// Return the size of refContainer for this type of Pointer.
	static int refContainerSize() { return refContainer.size(); }

	// A utility function that displays refContainer.
	static void showlist();

	// Clear refContainer when program exits.
	static void shutdown();

};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T, int size>
Pointer<T, size>::Pointer(T *t) {
	// Register shutdown() as an exit function.
	if (first)
		atexit(shutdown);
	first = false;

	//  DONE

	////set the iterator
	//typename std::list<PtrDetails<T>>::iterator p;

	////call function to display refContainer
	//showlist();

	////Find a pointer in refContainer
	//p = findPtrInfo(t);

	////make sure there are existing data 
	//if (!p->refcount) {

	//	//set details on pointer
	//	PtrDetails<T> newP(t);

	//	//push the pointer to the back of the list
	//	refContainer.push_back(newP);
	//}
	//else {
	//	//increment refcount if there was none found
	//	p->refcount++;
	//}

	////call function to display refContainer
	//showlist();

	////set address of pointer
	//addr = t;

	//// set size to 0 as no pointer
	//arraySize = 0;

	////set as not an array
	//isArray = false;

	//grab the address
	addr = t;
	
	//grab the size
	arraySize = size;
	
	//test to see if there is any data in the array
	//set either case
	if (arraySize > 0)
		isArray = true;
	else
		isArray = false;

	//if there is an address existing
	//add it to the back of the container
	if (t)
		refContainer.push_back(PtrDetails<T>(t, arraySize));

}

// Copy constructor.
template< class T, int size>
Pointer<T, size>::Pointer(const Pointer &ob) {
	//DONE
	typename std::list<PtrDetails<T>>::iterator p;
	p = findPtrInfo(ob.addr);

	// DONE
	addr = ob.addr;
	
	// increment ref count
	p->refcount++;
	
	// decide whether it is an array
	arraySize = ob.arraySize;
	
	//access size
	p->arraySize = ob.arraySize;
	
	//if larger than 0 (we have data)
	if (ob.arraySize > 0) {
		//flag array as true
		isArray = true;
		//set reference
		p->isArray = isArray;
	}
	else {
		//flag as not array
		isArray = false;
		//set reference
		p->isArray = isArray;
	}

}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer() {

	//DONE
	
	//setup iterator obj
	typename std::list<PtrDetails<T>>::iterator p;
	
	//gra the details
	p = findPtrInfo(addr);
	
	//if we have an existing reference in iterator obj
	if (p->refcount)
		// decrement ref count
		p->refcount--;
		
	// Collect garbage when a pointer goes out of scope.
	collect();
	
	// For real use, you might want to collect unused memory less frequently,
	// such as after refContainer has reached a certain size, after a certain number of Pointers have gone out of scope,
	// or when memory is low.
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect() {
	//DONE
	
	//var to track if the memory has been freed
	bool hasMemBeenFreed = false;

	//set up an iterator to grab pointer details
	typename std::list<PtrDetails<T> >::iterator p;

	//interate throught the ... um ... iterator
	do {
		// Scan refContainer looking for unreferenced pointers.
		for (p = refContainer.begin(); p != refContainer.end(); p++) {
			
			// If in-use, skip.
			int test = p->refcount;
			if (test > 0)
				continue;
				
			//flag memory has been freed
			hasMemBeenFreed = true;

			// Free memory unless the Pointer is null
			if (p->memPtr) {
				//check to see if this is a pointer array
				if (p->isArray) {
					delete[] p->memPtr;
				}
				else {
					//this is a normal pointer
					delete p->memPtr; 
				}
			}

			// Remove unused entry from refContainer
			refContainer.remove(*p);
			
			// Restart the search.
			break;
		}
	} while (p != refContainer.end());

	return hasMemBeenFreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t) {

	//DONE
	
	delete this;
	//Pointer<T> P = new T(*t);
	
	//setup the iterator object
	typename std::list<PtrDetails<T>>::iterator p;
	
	//grab the details
	p = findPtrInfo(t);
	
	// Increment the reference count of
	// the new address.
	showlist();
	if (!p->refcount) {
		PtrDetails<T> newP(t);
		refContainer.push_back(newP);
	}
	else {
		p->refcount++;
	}
	
	// store the address.
	showlist();
	
	//set address ref
	this->addr = t;
	
	//flag as not array
	this->isArray = false;
	
	// return obj
	return *this;

}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv) {

//DONE
	//setup the iterator object
	typename std::list<PtrDetails<T>>::iterator p;
	
	//grab the details
	p = findPtrInfo(addr);
	
	//set pointer space
	Pointer ptr;
	
	// First, decrement the reference count
	// for the memory currently being pointed to.
	p->refcount--;
	
	// Then, increment the reference count of
	// the new address.
	// increment ref count
	rv->refcount++;
	
	// store the address.
	ptr.addr = rv.addr;
	
	// return
	return ptr;

}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist() {

	//setup the iterator object
	typename std::list<PtrDetails<T> >::iterator p;
	
	//write details to the console
	std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
	std::cout << "memPtr refcount value\n ";
	if (refContainer.begin() == refContainer.end())
	{
		std::cout << " Container is empty!\n\n ";
	}
	for (p = refContainer.begin(); p != refContainer.end(); p++)
	{
		std::cout << "[" << (void *)p->memPtr << "]"
			<< " " << p->refcount << " ";
		if (p->memPtr)
			std::cout << " " << *p->memPtr;
		else
			std::cout << "---";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr) {

	//setup the iterator object
	typename std::list<PtrDetails<T> >::iterator p;
	
	// Find ptr in refContainer.
	for (p = refContainer.begin(); p != refContainer.end(); p++)
		if (p->memPtr == ptr)
			return p;
	
	//return
	return p;
}

// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown() {
	if (refContainerSize() == 0)
		return; // list is empty
		
	//setup the iterator object
	typename std::list<PtrDetails<T> >::iterator p;
	
	loop through container from beginning to end (all)
	for (p = refContainer.begin(); p != refContainer.end(); p++)
	{
		// Set all reference counts to zero
		p->refcount = 0;
	}
	
	//GC
	collect();
}
