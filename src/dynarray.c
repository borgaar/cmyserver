#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_ARRAY_CAP 4

typedef struct {
    size_t length;
     // end of struct definition in DynArray.h
    void *data; // is set to NULL pointer when freed to detect use after free etc.
    size_t cap_bytes;
    size_t elem_size;
} DynArray;

static void check_use_after_free(void* data, char* err_msg) {
    if (data  == NULL) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
};

static void DynArray_resize(DynArray *arr, size_t new_byte_cap) {
    check_use_after_free(arr->data, "cannot resize a freed dynamic array");

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

    return arr;
}

void DynArray_push(DynArray *arr, void *restrict element) {
    check_use_after_free(arr->data, "cannot push to a freed dynamic array");

    // resize if necessary
    if (arr->length >= (arr->cap_bytes / arr->elem_size)) {
        size_t new_cap = arr->cap_bytes * 2;
        DynArray_resize(arr, new_cap);
    }

    memcpy(&((char*)arr->data)[arr->length * arr->elem_size], element, arr->elem_size);

    arr->length++;
}

void DynArray_free(DynArray *arr) {
    check_use_after_free(arr->data, "attempted double-freeing of a dynamic array");

    free(arr->data);
    arr->data = NULL;
}

void DynArray_clear(DynArray *arr) {
    check_use_after_free(arr->data, "cannot clear a freed dynamic array");

    DynArray_resize(arr, INIT_ARRAY_CAP * arr->elem_size);

    arr->length = 0;
}

void *DynArray_get(DynArray *arr, size_t n) {
    check_use_after_free(arr->data, "attempted accessing of freed dynamic array");

    if (n >= arr->length) {
        perror("index out of bounds for dynamic array");
        exit(EXIT_FAILURE);
    }

    return &((char*)arr->data)[n * arr->elem_size];
}

void DynArray_remove(DynArray *arr, size_t n) {
    check_use_after_free(arr->data, "attempted removing element from freed dynamic array");

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
