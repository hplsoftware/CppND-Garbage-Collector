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

	//initially set as not an array
	isArray = false;
    
	//save passed in pointer object
	addr = t;

	//if size exists, flag as an arrray
	if (size > 0) {
    	isArray = true;
    }
	
	//initialize the size of the array
	arraySize = size;
    
	//set the iterator
	typename std::list<PtrDetails<T>>::iterator p;
    
    //set details on pointer
	p = findPtrInfo(t);

	//make sure there are existing data 
	if (p != refContainer.end()) {
    	//increment refcount if there was none found
		p->refcount++;
	}
	else {
    	//set details on pointer
		PtrDetails<T> ptr(t, size);
        
        //push the pointer to the back of the list
		refContainer.push_back(ptr);		
	}

}
// Copy constructor.
template< class T, int size>
Pointer<T, size>::Pointer(const Pointer &ob) {
	//DONE
    
	//set the iterator
	typename std::list<PtrDetails<T>>::iterator p;
    
    //set details on pointer
	p = findPtrInfo(ob.addr);
    
    //increment the reference counter
	p->refcount++;

	//set the address to that of the current obh=ject
	addr = ob.addr;
    
    //set the array size to current obj
	arraySize = ob.arraySize;
    
    //set flag to show if this is an array or not
	isArray = ob.isArray;
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer() {

	//DONE
    
    //set the iterator
	typename std::list<PtrDetails<T>>::iterator p;
    
    //set details on pointer
	p = findPtrInfo(addr);
    
    //decrement the ref counter if exists
	if (p->refcount)
		p->refcount--;
        
    // Collect garbage when a pointer goes out of scope.  
	collect();

}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect() {

	//DONE
    
    //var to track if the memory has been freed
	bool memfreed = false;
    
    //set up an iterator to grab pointer details
	typename std::list<PtrDetails<T>>::iterator p;
    
    //interate throught the ... um ... iterator
	do {
    
		showlist();
        
        // Scan refContainer looking for unreferenced pointers.
		for (p = refContainer.begin(); p != refContainer.end(); p++) {
        
        	// If in-use, skip.
			if (p->refcount > 0) {
				continue;
			}
            
            //flag memory as freed
			memfreed = true;
            
            // Free memory unless the Pointer is null
			if (p->memPtr) {
            //check to see if this is a pointer array
				if (p->isArray) {
                	//this is an array pointer, delete
					delete[]((void*)p->memPtr);
				}
				else {
                	//this is a normal pointer, delete
					delete ((void*)p->memPtr);
				}
			}
            
            // Remove unused entry from refContainer
			refContainer.remove(*p);
            
            // Restart the search.
			break;
		}
	} while (p != refContainer.end());
    
    //return flag
	return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t) {

	//DONE
    
    //set up an iterator to grab pointer details
	typename std::list<PtrDetails<T>>::iterator p;
    
    //if we have a valid address
    //grab detail and decrement refcount
	if (addr) {
		p = findPtrInfo(addr);
		p->refcount--;
	}

//grab pointer info
	p = findPtrInfo(t);
    
    //test if at end of search
	if (p != refContainer.end()) {
    	//increment ref count
		p->refcount++;
	}
	else {
    //grab info on pointer
		PtrDetails<T> ptr(t, size);
        
        //add to the container
		refContainer.push_back(ptr);		
	}

	//set address
	addr = t;

	//set flag if there is data available
	if (size > 0) isArray = true;
	else isArray = false;

	// set size
	arraySize = size;

	//return
	return *this;	

}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv) {

	//DONE
    //set up an iterator to grab pointer details
	typename std::list<PtrDetails<T>>::iterator p;
    
     //grab info on pointer
	p = findPtrInfo(rv.addr);
    
    //increment reference count
	p->refcount++;

	//grab pointer info
	p = findPtrInfo(addr);
    
    //decrement previous reference.
	p->refcount--;

	//set detials based on current object
	addr = rv.addr;
	arraySize = rv.size;
	isArray = rv.isArray;

	//return result
	return this;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist() {

  //set up an iterator to grab pointer details
	typename std::list<PtrDetails<T> >::iterator p;
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
	typename std::list<PtrDetails<T> >::iterator p;
	// Find ptr in refContainer.
	for (p = refContainer.begin(); p != refContainer.end(); p++)
		if (p->memPtr == ptr)
			return p;
	return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown() {
	if (refContainerSize() == 0)
		return; // list is empty
	typename std::list<PtrDetails<T> >::iterator p;
	for (p = refContainer.begin(); p != refContainer.end(); p++)
	{
		// Set all reference counts to zero
		p->refcount = 0;
	}
	collect();
}
