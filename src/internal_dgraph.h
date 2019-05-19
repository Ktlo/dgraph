#ifndef _INTERNAL_DGRAPH_H
#define _INTERNAL_DGRAPH_H

#include "dgraph.h"

struct dgraph_node_list {
	int n;
	void *content;
	size_t size;
	int list[1]; // Should be 0, but unfotunatelly ISO C forbids that
};

typedef struct dgraph_node_list *dgraph_node_t;

int dgraph_internal_initialize_node(dgraph_t graph, size_t n, size_t size, int *values, void *ud, dgraph_initializer init);
int dgraph_internal_destroy_node(dgraph_t graph, size_t n, void *ud, dgraph_initializer init);
int dgraph_internal_insert_to_node(dgraph_t graph, int from, int to, void *value);

struct dgraph_handle {
	void *ud_alloc;
	dgraph_allocator alloc;
	size_t nodes;
	dgraph_node_t *data;
};

#endif // _INTERNAL_DGRAPH_H
