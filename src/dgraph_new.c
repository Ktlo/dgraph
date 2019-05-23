#include "internal_dgraph.h"

#include <stdlib.h>
#include <errno.h>
#include <assert.h>

void *dgraph_default_allocator(void *ud, void *old, size_t size) {
	(void)ud;
	return realloc(old, size);
}

dgraph_t dgraph_new(void *alloc_ud, dgraph_allocator alloc) {
	if (!alloc)
		alloc = &dgraph_default_allocator;
	dgraph_t graph = (dgraph_t)alloc(alloc_ud, NULL, sizeof(struct dgraph_handle));
	if (!graph) {
		alloc(alloc_ud, graph, 0);
		errno = ENOMEM;
		return NULL;
	}
	graph->ud_alloc = alloc_ud;
	graph->alloc = alloc;
	graph->nodes = 0;
	graph->data = NULL;
	return graph;
}

dgraph_t dgraph_create() {
	return dgraph_new(NULL, NULL);
}

void dgraph_delete(dgraph_t graph) {
	dgraph_clear(graph);
	graph->alloc(graph->ud_alloc, graph, 0);
}
