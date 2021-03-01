#pragma once

void* operator new(size_t n);
void* operator new(size_t n, void* p);
void* operator new[](size_t s);
void  operator delete(void *p);
void  operator delete[](void *p);
