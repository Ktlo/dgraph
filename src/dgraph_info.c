#include "internal_dgraph.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

size_t dgraph_nodes(const dgraph_t graph) {
	return graph->nodes;
}

size_t dgraph_edges(const dgraph_t graph) {
	size_t i;
	size_t result = 0;
	size_t nodes = graph->nodes;
	dgraph_node_t *data = graph->data;
	for (i = 0; i < nodes; i++) {
		result += data[i]->size;
	}
	return result;
}

#define stralloc(addjust) \
		char *new_str = alloc(ud_alloc, str, str_size += addjust); \
		if (!new_str) \
			goto fail; \
		str = new_str;

char *dgraph_to_string(const dgraph_t graph, dgraph_element_to_string tostr) {
	dgraph_allocator alloc = graph->alloc;
	void *ud_alloc = graph->ud_alloc;
	char *str = alloc(ud_alloc, NULL, sizeof(void*));
	if (!str)
		goto fail;
	str[0] = '{';
	str[1] = '\0';
	size_t str_size = sizeof(void*);
	size_t str_ptr = 1;
	dgraph_node_t *data = graph->data;
	size_t nodes = graph->nodes;
	if (!nodes) {
		str[1] = ' ';
		str[2] = '}';
		str[3] = '\0';
		return str;
	}
	size_t i;
	for (i = 0; i < nodes; i++) {
		dgraph_node_t node = data[i];
		size_t addjust;
		if (tostr && node->content) {
			char *obj_str = tostr(node->content);
			if (!obj_str)
				goto fail;
			addjust = strlen(obj_str) + dgraph_util_measure_int(node->n) + 6;
			stralloc(addjust);
			sprintf(str + str_ptr, "#%d (%s) [", node->n, obj_str);
			alloc(ud_alloc, obj_str, 0);
		} else {
			addjust = dgraph_util_measure_int(node->n) + 3;
			str = alloc(ud_alloc, str, str_size += addjust);
			if (!str)
				goto fail;
			sprintf(str + str_ptr, "#%d [", node->n);
		}
		str_ptr += addjust;
		size_t j;
		size_t last = node->size - 1;
		if (last == (size_t)-1) {
			addjust = 2;
			stralloc(addjust);
			str[str_ptr] = ' ';
			str[str_ptr + 1] = ']';
			str_ptr += addjust;
		} else {
			for (j = 0; j < last; j++) {
				int goal = node->list[j];
				addjust = dgraph_util_measure_int(goal) + 2;
				stralloc(addjust);
				sprintf(str + str_ptr, "%d, ", goal);
				str_ptr += addjust;
			}
			int goal = node->list[last];
			addjust = dgraph_util_measure_int(goal) + 1;
			stralloc(addjust);
			sprintf(str + str_ptr, "%d]", goal);
			str_ptr += addjust;
		}
		if (i + 1 != nodes) {
			size_t addjust = 2;
			stralloc(addjust);
			str[str_ptr] = ',';
			str[str_ptr + 1] = ' ';
			str_ptr += addjust;
		} else {
			str[str_ptr] = '}';
			str[str_ptr + 1] = '\0';
		}
	}
	return str;
fail:
	if (str)
		alloc(ud_alloc, str, 0);
	errno = ENOMEM;
	return NULL;
}

int dgraph_equals(const dgraph_t first, const dgraph_t second, dgraph_element_equals eq) {
	if (first == second)
		return 1;
	size_t nodes = first->nodes;
	if (nodes != second->nodes)
		return 0;
	size_t i;
	for (i = 0; i < nodes; i++) {
		const dgraph_node_t a = first->data[i];
		const dgraph_node_t b = second->data[i];
		size_t size = a->size;
		if (eq)
			if (!eq(a->content, b->content))
				return 0;
		if (a->n != b->n || size != b->size)
			return 0;
		int *list_a = a->list;
		int *list_b = b->list;
		size_t j;
		for (j = 0; j < size; j++)
			if (list_a[j] != list_b[j])
				return 0;
	}
	return 1;
}

int dgraph_hcr(int n, int bits) {
	return (n << bits) | ((unsigned)n >> (sizeof(int)*8 - bits));
}

int dgraph_hash_code(const dgraph_t graph, dgraph_element_hash_code hc) {
	size_t nodes = graph->nodes;
	int result = dgraph_hcr((int)nodes, 8);
	result ^= dgraph_hcr((int)(nodes >> 32), 8);
	size_t i;
	for (i = 0; i < nodes; i++) {
		const dgraph_node_t node = graph->data[i];
		if (hc)
			result ^= dgraph_hcr(hc(node->content), 16);
		result ^= node->n;
		size_t size = node->size;
		result ^= dgraph_hcr((int)(size >> 32), 8);
		result ^= dgraph_hcr((int)(size), 8);
		int *list = node->list;
		size_t j;
		for (j = 0; j < size; j++)
			result += list[j];
	}
	return result;
}

int dgraph_has_node(const dgraph_t graph, int n) {
	if (dgraph_internal_place_node((dgraph_t)graph, n, NULL, 0))
		return 1;
	else
		return 0;
}

int dgraph_has_edge(const dgraph_t graph, int a, int b) {
	dgraph_node_t node = dgraph_internal_place_node((dgraph_t)graph, a, NULL, 0);
	if (!node)
		return 0;
	return dgraph_internal_node_contains(node, b);
}

size_t dgraph_get(const dgraph_t graph, int n, const int *list[]) {
	dgraph_node_t node = dgraph_internal_place_node((dgraph_t)graph, n, NULL, 0);
	if (!node) {
		if (list)
			*list = NULL;
		return 0;
	} else {
		if (list)
			*list = node->list;
		return node->size;
	}
}

void dgraph_visit(const dgraph_t graph, dgraph_visitor visitor, void *ud) {
	size_t nodes = graph->nodes;
	size_t i;
	for (i = 0; i < nodes; i++) {
		dgraph_node_t node = graph->data[i];
		if (visitor(graph, ud, node->n, &node->content, node->size, node->list))
			return;
	}
}
