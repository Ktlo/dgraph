#include "internal_dgraph.h"

#include <stdlib.h>
#include <errno.h>

void *dgraph_default_allocator(void *ud, void *old, size_t size) {
	(void)ud;
	return realloc(old, size);
}

dgraph_t dgraph_new_alloc(void *alloc_ud, dgraph_allocator alloc, void *init_ud, dgraph_initializer init, int **initial) {
	if (!alloc)
		alloc = &dgraph_default_allocator;
	dgraph_t graph = (dgraph_t)alloc(alloc_ud, NULL, sizeof(dgraph_t));
	if (!graph)
		goto fail;
	graph->ud_alloc = alloc_ud;
	graph->alloc = alloc;
	if (!initial) {
		graph->nodes = 0;
		graph->data = NULL;
		return graph;;
	}
	size_t max_node_n = -1;
	int **next_node;
	size_t i;
	for (i = 0, next_node = initial; *next_node; next_node++, i++);
	max_node_n = i;
	graph->data = (dgraph_node_t *)alloc(alloc_ud, NULL, max_node_n * sizeof(dgraph_node_t));
	if (!graph->data)
		goto fail;
	for (i = 0; i < max_node_n; i++) {
		int *list = initial[i];
		size_t list_length;
		for (list_length = 0; list[list_length] != -1; list_length++);
		if (!dgraph_internal_initialize_node(graph, i, list_length, list, init_ud, init))
			goto fail;
	}
	return graph;
fail:
	if (graph) {
		if (graph->data) {
			for (i = 0; i < graph->nodes; i++) {
				struct dgraph_node_list *node = graph->data[i];
				if (node)
					alloc(alloc_ud, node, 0);
				else
					break;
			}
			alloc(alloc_ud, graph->data, 0);
		}
		alloc(alloc_ud, graph, 0);
	}
	errno = ENOMEM;
	return NULL;
}

dgraph_t dgraph_new(void *init_ud, dgraph_initializer init, int **initial) {
	return dgraph_new_alloc(NULL, &dgraph_default_allocator, init_ud, init, initial);
}

dgraph_t dgraph_create() {
	return dgraph_new_alloc(NULL, &dgraph_default_allocator, NULL, NULL, NULL);
}
