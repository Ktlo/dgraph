#include "test.h"

#include "dgraph_algs.h"

void test() {
	dgraph_t graph = dgraph_create();
	dgraph_set_list(graph, 0, 0, NULL);
	int l1[] = { 2, 3 };
	dgraph_set_list(graph, 1, 2, l1);
	int l2[] = { 3, 4 };
	dgraph_set_list(graph, 2, 2, l2);
	dgraph_set(graph, 3, 0);
	int l4[] = { 1, 0 };
	dgraph_set_list(graph, 4, 2, l4);
	assert_equals_str("{#0 [ ], #1 [2, 3], #2 [3, 4], #3 [0], #4 [0, 1]}", dgraph_to_string(graph, NULL));
	assert_equals_int(1, dgraph_find_cycle(graph));
	dgraph_remove(graph, 4, 1);
	assert_equals_str("{#0 [ ], #1 [2, 3], #2 [3, 4], #3 [0], #4 [0]}", dgraph_to_string(graph, NULL));
	assert_equals_int(-1, dgraph_find_cycle(graph));
}
