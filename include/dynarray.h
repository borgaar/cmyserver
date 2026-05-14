#include <stddef.h>
typedef struct {
    size_t length;
} DynArray;

DynArray DynArray_new(size_t element_size);
void DynArray_push(DynArray *arr, void *restrict element);
void DynArray_free(DynArray *arr);
void DynArray_clear(DynArray *arr);
void *DynArray_get(DynArray *arr, size_t n);
void DynArray_remove(DynArray *arr, size_t n);
