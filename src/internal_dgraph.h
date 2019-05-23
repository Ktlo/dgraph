#ifndef _INTERNAL_DGRAPH_H
#define _INTERNAL_DGRAPH_H

#include "dgraph.h"

#define FAKE_DGRAPH_NODE_LIST_SZ 1

struct dgraph_node_list {
	int n;
	void *content;
	size_t size;
	int list[FAKE_DGRAPH_NODE_LIST_SZ]; // Should be 0, but unfotunatelly ISO C forbids that
};

typedef struct dgraph_node_list *dgraph_node_t;

dgraph_node_t dgraph_internal_place_node(dgraph_t graph, int n, size_t *index, int place);
int dgraph_internal_destroy_node(dgraph_t graph, int n);
int dgraph_internal_insert_to_node(dgraph_t graph, int from, size_t amount, const int to[]);
int dgraph_internal_remove_from_node(dgraph_t graph, int from, size_t amount, const int to[]);
int dgraph_internal_node_contains(const dgraph_node_t node, int n);

size_t dgraph_util_measure_int(int n);

struct dgraph_handle {
	void *ud_alloc;
	dgraph_allocator alloc;
	size_t nodes;
	dgraph_node_t *data;
};

#endif // _INTERNAL_DGRAPH_H
