#include "internal_dgraph.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int dgraph_compar_integers(const void *p1, const void *p2) {
	return *(const int *)p1 - *(const int *)p2;
}

int dgraph_compar_lists(const void *p1, const void *p2) {
	return ((const dgraph_node_t)p1)->n - ((const dgraph_node_t)p2)->n;
}

int dgraph_internal_initialize_node(dgraph_t graph, size_t n, size_t size, int *values, void *ud, dgraph_initializer init) {
	assert(graph->nodes > n);
	dgraph_node_t node = graph->data[n] = (dgraph_node_t)graph->alloc(graph->ud_alloc, NULL,
		sizeof(struct dgraph_handle) - sizeof(int) * (size - 1));
	if (!node)
		return 0;
	node->content = init ? init(ud, NULL, n) : NULL;
	node->size = size;
	node->n = n;
	memcpy(node->list, values, size);
	qsort(node->list, size, sizeof(int), &dgraph_compar_integers);
	return 1;
}

int dgraph_internal_destroy_node(dgraph_t graph, size_t n, void *ud, dgraph_initializer init) {
	size_t m;
	dgraph_node_t node;
	if (init) {
		node = graph->data[m = n];
		init(ud, node->content, n);
		goto found;
	}
	for (m = 0; m <= n; m++) {
		if (n == (node = graph->data[m])->n)
			goto found;
	}
	return 0;
found:
	graph->alloc(graph->ud_alloc, node, 0);
	memmove(graph->data + m, graph->data + m + 1, --graph->nodes * sizeof(dgraph_node_t));
	graph->alloc(graph->ud_alloc, graph->data, graph->nodes * sizeof(dgraph_node_t));
	return 1;
}

int dgraph_internal_insert_to_node(dgraph_t graph, int from, int to, void *value) {
	size_t m;
	dgraph_node_t node;
	for (m = 0; m <= from; m++) {
		node = graph->data[m];
		if (node->n == from) {
			goto found;
		}
		if (node->n > from)
			break;
	}
	
found:
	size_t i;
	for (i = 0; i < node->size; i++) {
		if (node->list[i] == to) {
			errno = 0;
			return 0;
		}
	}
	node = graph->alloc(graph->ud_alloc, node, node->size + 1);
	if (!node) {
		errno = ENOMEM;
		return 0;
	}
	graph->data[from] = node;
	node->list[node->size++] = to;
	return 1;
}
