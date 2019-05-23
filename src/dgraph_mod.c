#include "internal_dgraph.h"

#include <errno.h>
#include <stdlib.h>

void **dgraph_retrive_node(dgraph_t graph, int n, int *is_new) {
	size_t old_size = graph->nodes;
	dgraph_node_t node = dgraph_internal_place_node(graph, n, NULL, 1);
	if (is_new)
		*is_new = graph->nodes - old_size;
	if (node)
		return &node->content;
	else
		return NULL;
}

int dgraph_set(dgraph_t graph, int a, int b) {
	return dgraph_internal_insert_to_node(graph, a, 1, &b);
}

int dgraph_set_list(dgraph_t graph, int n, size_t size, const int list[]) {
	return dgraph_internal_insert_to_node(graph, n, size, list);
}

int dgraph_remove(dgraph_t graph, int a, int b) {
	return dgraph_internal_remove_from_node(graph, a, 1, &b);
}

int dgraph_rm_list(dgraph_t graph, int n, size_t size, const int list[]) {
	return dgraph_internal_remove_from_node(graph, n, size, list);
}

int dgraph_rm_node(dgraph_t graph, int n) {
	return dgraph_internal_destroy_node(graph, n);
}

int dgraph_init(dgraph_t graph, size_t size, const int *const *data) {
	size_t i;
	int result = 0;

	int *buff = malloc(size*sizeof(int));

	for (i = 0; i < size; i++) {
		size_t j;
		size_t amount = 0;
		const int *list = data[i];
		for (j = 0; j < size; j++) {
			if (list[j])
				buff[amount++] = j;
		}
		if (amount) {
			if (dgraph_internal_insert_to_node(graph, i, amount, buff))
				result = 1;
			else if (errno == ENOMEM) {
				free(buff);
				errno = ENOMEM;
				return 0;
			}
		}
	}
	free(buff);
	return result;
}

void dgraph_clear(dgraph_t graph) {
	size_t i;
	dgraph_allocator alloc = graph->alloc;
	void *ud_alloc = graph->ud_alloc;
	size_t nodes = graph->nodes;
	dgraph_node_t *data = graph->data;
	for (i = 0; i < nodes; i++)
		alloc(ud_alloc, data[i], 0);
	alloc(ud_alloc, data, 0);
	graph->data = NULL;
	graph->nodes = 0;
}
