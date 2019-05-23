#include "test.h"

#include "dgraph.h"

void test() {
	dgraph_t graph = dgraph_create();
	int am[3] = { 1, 0, 1 };
	int bm[3] = { 0, 0, 0 };
	int cm[3] = { 1, 1, 0 };
	const int *m[3] = { am, bm, cm };
	dgraph_init(graph, 3, m);
	assert_equals_str("{#0 [0, 2], #1 [ ], #2 [0, 1]}", dgraph_to_string(graph, NULL));
	dgraph_rm_node(graph, 1);
	assert_equals_str("{#0 [0, 2], #2 [0]}", dgraph_to_string(graph, NULL));
	dgraph_clear(graph);
	assert_equals_str("{ }", dgraph_to_string(graph, NULL));
	cm[1] = 0;
	dgraph_init(graph, 3, m);
	assert_equals_str("{#0 [0, 2], #2 [0]}", dgraph_to_string(graph, NULL));
}
