#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_ARRAY_CAP 4

typedef struct {
    size_t length;
     // end of struct definition in DynArray.h
    _Bool freed;
    void *data;
    size_t cap_bytes;
    size_t elem_size;
} DynArray;

static void DynArray_resize(DynArray *arr, size_t new_byte_cap) {
    if (arr->freed) {
        perror("cannot resize a freed dynamic array");
        exit(EXIT_FAILURE);
    }

    void *tmp = realloc(arr->data, new_byte_cap);

    if (tmp == NULL) {
        perror("failed to shrink dynamic array");
        exit(EXIT_FAILURE);
    }

    arr->data = tmp;
    arr->cap_bytes = new_byte_cap;
}

DynArray DynArray_new(size_t element_size) {
    DynArray arr;

    arr.data = malloc(element_size * INIT_ARRAY_CAP);
    arr.length = 0;
    arr.cap_bytes = element_size * INIT_ARRAY_CAP;
    arr.elem_size = element_size;
    arr.freed = 0;

    return arr;
}

void DynArray_push(DynArray *arr, void *restrict element) {
    if (arr->freed) {
        perror("cannot push to a freed dynamic array");
        exit(EXIT_FAILURE);
    }

    // resize if necessary
    if (arr->length >= (arr->cap_bytes / arr->elem_size)) {
        size_t new_cap = arr->cap_bytes * 2;
        DynArray_resize(arr, new_cap);
    }

    memcpy(&((char*)arr->data)[arr->length * arr->elem_size], element, arr->elem_size);

    arr->length++;
}

void DynArray_free(DynArray *arr) {
    if (arr->freed) {
        perror("attempted double-freeing of a dynamic array");
        exit(EXIT_FAILURE);
    }

    free(arr->data);
    arr->freed = 1;
}

void DynArray_clear(DynArray *arr) {
    if (arr->freed) {
        perror("cannot clear a freed dynamic array");
        exit(EXIT_FAILURE);
    }

    DynArray_resize(arr, INIT_ARRAY_CAP * arr->elem_size);

    arr->length = 0;
}

void *DynArray_get(DynArray *arr, size_t n) {
    if (arr->freed) {
        perror("attempted accessing of freed dynamic array");
        exit(EXIT_FAILURE);
    }

    if (n >= arr->length) {
        perror("index out of bounds for dynamic array");
        exit(EXIT_FAILURE);
    }

    return &((char*)arr->data)[n * arr->elem_size];
}

void DynArray_remove(DynArray *arr, size_t n) {
    if (arr->freed) {
        perror("attempted removing element from freed dynamic array");
        exit(EXIT_FAILURE);
    }

    if (n >= arr->length) {
        perror("index out of bounds for dynamic array");
        exit(EXIT_FAILURE);
    }

    size_t moving_elems = (arr->length - 1) - n;
    size_t moving_bytes = moving_elems * arr->elem_size;
    size_t moving_dst = n * arr->elem_size;
    size_t moving_src = moving_dst + arr->elem_size;

    memmove((char*)arr->data + moving_dst, (char*)arr->data + moving_src, moving_bytes);

    arr->length--;

    // resize if more than half of allocated is unused
    size_t elem_cap = arr->cap_bytes / arr->elem_size;
    size_t free_space = elem_cap - arr->length;
    if (free_space >= elem_cap / 2 && elem_cap > INIT_ARRAY_CAP) {
        size_t new_cap = arr->cap_bytes / 2;
        DynArray_resize(arr, new_cap);
    }
}
