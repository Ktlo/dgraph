#include "dgraph_algs.h"

#include "internal_dgraph.h"

#include <assert.h>
#include <stdlib.h>

int is_in_stack(int *stack, size_t size, int goal) {
	size_t i;
	for (i = 0; i < size; i++) {
		if (stack[i] == goal)
			return 1;
	}
	return 0;
}

int fill_stack(const dgraph_t graph, size_t offset, int n, int *stack, size_t *index) {
	assert(index);
	size_t m = offset + 1;
	stack[offset] = n;
	while (1) {
		dgraph_node_t node = dgraph_internal_place_node(graph, stack[m - 1], NULL, 0);
		if (node->size) {
			int goal = node->list[0];
			if (is_in_stack(stack, m, goal))
				return 0;
			stack[m++] = goal;
		} else
			break;
	}
	*index = m - 1;
	return 1;
}

int dgraph_find_cycle(const dgraph_t graph) {
	int *stack = graph->alloc(graph->ud_alloc, NULL, graph->nodes*sizeof(int));
	size_t m = 0, nodes = graph->nodes, i;
	for (i = 0; i < nodes; i++) {
		if (!fill_stack(graph, 0, graph->data[i]->n, stack, &m))
			goto cycle;
		while (m > 0) {
			int old_goal = stack[m];
			m--;
			int n = stack[m];
			dgraph_node_t node = dgraph_internal_place_node(graph, n, NULL, 0);
			size_t j;
			for (j = 0; j < node->size; j++) {
				if (node->list[j] == old_goal)
					break;
			}
			if (j != node->size - 1) {
				if (!fill_stack(graph, m + 1, node->list[j + 1], stack, &m))
					goto cycle;
			}
		}
	}
	free(stack);
	return -1;
cycle:
{
	int result = stack[0];
	free(stack);
	return result;
}
}
