#ifndef _DGRAPH_H
#define _DGRAPH_H

#include <stddef.h>

struct dgraph_handle;
typedef struct dgraph_handle *dgraph_t;

typedef void *(*dgraph_allocator)(void *ud, void *ptr, size_t size);
typedef void *(*dgraph_initializer)(void *ud, void *, int);

dgraph_t dgraph_new_alloc(void *alloc_ud, dgraph_allocator alloc, void *init_ud, dgraph_initializer init, int **initial);
dgraph_t dgraph_new(void *init_ud, dgraph_initializer init, int **initial);
dgraph_t dgraph_create();
int dgraph_set(dgraph_t graph, int a, int b, void *value);
int dgraph_remove(dgraph_t graph, int a, int b, void **value);
int dgraph_get(dgraph_t graph, int a, int b, void **value);
int dgraph_nodes(dgraph_t graph);
int dgraph_edges(dgraph_t graph);
size_t dgraph_to_string(dgraph_t graph, char *str, size_t length);

#endif // _DGRAPH_H
