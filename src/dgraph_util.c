#include "internal_dgraph.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define realloc_node(node, size) \
		graph->alloc(graph->ud_alloc, node, sizeof(struct dgraph_node_list) + (size - FAKE_DGRAPH_NODE_LIST_SZ)*sizeof(int));

int dgraph_compar_integers(const void *p1, const void *p2) {
	return *(const int *)p1 - *(const int *)p2;
}

int dgraph_compar_lists(const void *p1, const void *p2) {
	return ((const dgraph_node_t)p1)->n - ((const dgraph_node_t)p2)->n;
}

dgraph_node_t dgraph_internal_place_node(dgraph_t graph, int n, size_t *index, int place) {
	size_t m;
	dgraph_node_t node;
	for (m = 0; m < graph->nodes; m++) {
		node = graph->data[m];
		if (n == node->n) {
			if (index)
				*index = m;
			return node;
		}
		if (node->n > n) {
			if (!place)
				return NULL;
			if (index)
				*index = m;
			dgraph_node_t *data = graph->alloc(graph->ud_alloc, graph->data, (graph->nodes + 1)*sizeof(dgraph_node_t));
			if (!data)
				goto fail;
			graph->data = data;
			memmove(graph->data + m + 1, graph->data + m, (graph->nodes++ - m)*sizeof(dgraph_node_t));
			node = realloc_node(NULL, 0);
			if (!node) {
				memmove(graph->data + m, graph->data + m + 1, (--graph->nodes - m)*sizeof(dgraph_node_t));
				graph->data = graph->alloc(graph->ud_alloc, graph->data, graph->nodes*sizeof(dgraph_node_t));
				goto fail;
			}
			data[m] = node;
			node->content = NULL;
			node->n = n;
			node->size = 0;
			return node;
		}
	}
	if (!place)
		return NULL;
	if (index)
		*index = m;
	dgraph_node_t *data = graph->alloc(graph->ud_alloc, graph->data, (graph->nodes + 1)*sizeof(dgraph_node_t));
	if (!data)
		goto fail;
	graph->nodes++;
	graph->data = data;
	node = realloc_node(NULL, 0);
	if (!node) {
		graph->data = graph->alloc(graph->ud_alloc, graph->data, --graph->nodes*sizeof(dgraph_node_t));
		goto fail;
	}
	data[m] = node;
	node->content = NULL;
	node->n = n;
	node->size = 0;
	return node;
fail:
	errno = ENOMEM;
	return NULL;
}

int dgraph_internal_destroy_node(dgraph_t graph, int n) {
	size_t m;
	dgraph_node_t node;
	int result = 0;
	for (m = 0; m < graph->nodes; m++) {
		node = graph->data[m];
		if (n == node->n) {
			result = 1;
			graph->alloc(graph->ud_alloc, node, 0);
			memmove(graph->data + m, graph->data + m + 1, (--graph->nodes - m)*sizeof(dgraph_node_t));
			graph->alloc(graph->ud_alloc, graph->data, graph->nodes*sizeof(dgraph_node_t));
			m--;
		} else {
			size_t i;
			for (i = 0; i < node->size; i++) {
				int goal = node->list[i];
				if (goal == n) {
					memmove(node->list + i, node->list + i + 1, (--node->size - i)*sizeof(int));
					node = realloc_node(node, node->size);
					graph->data[m] = node;
					break;
				}
				if (goal > n)
					break;
			}
		}
	}
	return result;
}

int dgraph_internal_insert_to_node(dgraph_t graph, int from, size_t amount, const int to[]) {
	dgraph_node_t node = dgraph_internal_place_node(graph, from, NULL, 1);
	if (!node)
		goto fail;
	size_t i;
	size_t allocate;
	int result = 0;
	for (i = 0, allocate = 0; i < amount; i++) {
		if (!dgraph_internal_node_contains(node, to[i])) {
			allocate++;
			result = 1;
			if(!dgraph_internal_place_node(graph, to[i], NULL, 1))
				goto fail;
		}
	}
	size_t ind_a;
	dgraph_internal_place_node(graph, from, &ind_a, 1);
	node = realloc_node(node, node->size + allocate);
	if (!node)
		goto fail;
	graph->data[ind_a] = node;
	for (i = 0; i < amount; i++) {
		int goal = to[i];
		size_t j;
		int inserted = 0;
		for (j = 0; j < node->size; j++) {
			if (node->list[j] == goal) {
				inserted = 1;
				break;
			}
			if (node->list[j] > goal) {
				memmove(node->list + j + 1, node->list + j, (node->size - j)*sizeof(int));
				node->list[j] = goal;
				node->size++;
				inserted = 1;
				break;
			}
		}
		if (!inserted) {
			node->list[j] = goal;
			node->size++;
		}
	}
	errno = 0;
	return result;
fail:
	errno = ENOMEM;
	return 0;
}


int dgraph_internal_remove_from_node(dgraph_t graph, int from, size_t amount, const int to[]) {
	size_t index;
	dgraph_node_t node = dgraph_internal_place_node(graph, from, &index, 1);
	if (!node) {
		errno = ENOMEM;
		return 0;
	}
	size_t i;
	int result = 0;
	for (i = 0; i < amount; i++) {
		int goal = to[i];
		size_t j = 0;
		for (j = 0; j < node->size; j++) {
			if (node->list[j] == goal)
				memmove(node->list + j, node->list + j + 1, (--node->size - j)*sizeof(int));
			if (node->list[j] > goal)
				break;
		}
	}
	node = realloc_node(node, node->size);
	graph->data[index] = node;
	errno = 0;
	return result;
}

int dgraph_internal_node_contains(const dgraph_node_t node, int n) {
	size_t i;
	int *list = node->list;
	for (i = 0; i < node->size; i++) {
		if (list[i] == n)
			return 1;
		if (list[i] > n)
			break;
	}
	return 0;
}

size_t dgraph_util_measure_int(int n) {
	if (!n)
		return 1;
	size_t result = n < 0;
	if (result)
		n = -n;
	while (n) {
		result++;
		n /= 10;
	}
	return result;
}
