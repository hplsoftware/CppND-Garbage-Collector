// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
public:
	unsigned refcount; // current reference count
	T *memPtr;         // pointer to allocated memory
	/* isArray is true if memPtr points
to an allocated array. It is false
otherwise. */
	bool isArray; // true if pointing to array
	/* If memPtr is pointing to an allocated
array, then arraySize contains its size */
	unsigned arraySize; // size of array
	// Here, mPtr points to the allocated memory.
	// If this is an array, then size specifies
	// the size of the array.

	PtrDetails(T* _pnt, int _size = 0) :memPtr(_pnt), arraySize(_size)
	{
		//DONE

		//initially set to false
		isArray = false;

		//set mem pointer to passed in val
		memPtr = _pnt;

		//set size
		arraySize = _size;

		//initialize the refcount var
		refcount = 1;

		//if the passed in size is valid, set the array existence
		//as being true
		if (_size > 0)
			isArray = true;
	}
};
// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template <class T>
bool operator==(const PtrDetails<T> &ob1,
	const PtrDetails<T> &ob2)
{
	// DONE
	return (ob1.memPtr == ob2.memPtr);
}
