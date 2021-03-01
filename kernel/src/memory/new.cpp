#include "heap.h"
#include "new.h"

void* operator new(size_t n)
{
    return malloc(n);
}

void* operator new(size_t n, void* p)
{
    return p;
}

void* operator new[](size_t s)
{
    return malloc(s);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}
