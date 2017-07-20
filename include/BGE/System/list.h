#ifndef BGE_SYSTEM_LIST_H
#define BGE_SYSTEM_LIST_H
#include <BGE/Config.h>

namespace bge
{

struct BGE_EXPORT_API ListData 
{
    struct Data 
	{
        int   alloc, begin, end;
        int   sharable : 1;
        void* array[1];
    };
    enum { DataHeaderSize = sizeof(Data) - sizeof(void *) };

    Data* detach(int alloc);
    Data* detach_grow(int *i, int n);
    void realloc(int alloc);
    static Data shared_null;
    Data *d;
    void **erase(void **xi);
    void **append(int n);
    void **append();
    void **append(const ListData &l);
    void **prepend();
    void **insert(int i);
    void remove(int i);
    void remove(int i, int n);
    void move(int from, int to);
    inline int size() const { return d->end - d->begin; }
    inline bool isEmpty() const { return d->end  == d->begin; }
    inline void **at(int i) const { return d->array + d->begin + i; }
    inline void **begin() const { return d->array + d->begin; }
    inline void **end() const { return d->array + d->end; }
};

}

#endif
