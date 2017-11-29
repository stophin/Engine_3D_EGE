//Ray.cpp
//

#include "../flatform/ELibrary.h"
#include "Ray.h"

VertsPool pool;

void Verts::operator delete(void * _ptr) {
	if (_ptr == NULL)
	{
		return;
	}
	for (INT i = 0; i < MAX_VERTS_LINK; i++)
	{
		if (((Verts*)_ptr)->prev[i] != NULL || ((Verts*)_ptr)->next[i] != NULL)
		{
			return;
		}
	}
	//delete(_ptr);
	pool.back((Verts*)_ptr);
}

void * Verts::operator new(size_t size){
	return pool.get();
}