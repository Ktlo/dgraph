#include "test.h"

#include "dgraph.h"

void test() {
    dgraph_t graph = dgraph_create();
    assert_equals_int(0, dgraph_nodes(graph));
    assert_equals_int(0, dgraph_edges(graph));

    dgraph_set(graph, 3, 100);

    assert_equals_int(2, dgraph_nodes(graph));
    assert_equals_int(1, dgraph_edges(graph));

    dgraph_set(graph, 6, 10);
    dgraph_set(graph, 10, 6);
    dgraph_set(graph, 10, 10);
    dgraph_set(graph, 500, -10);

    assert_equals_int(6, dgraph_nodes(graph));
    assert_equals_int(5, dgraph_edges(graph));

    dgraph_t other = dgraph_create();
    dgraph_set(other, 3, 100);
    dgraph_set(other, 6, 10);
    dgraph_set(other, 10, 6);
    dgraph_set(other, 10, 10);
    dgraph_set(other, 500, -10);

    assert_equals_str("{#-10 [ ], #3 [100], #6 [10], #10 [6, 10], #100 [ ], #500 [-10]}", dgraph_to_string(graph, NULL));
    assert_equals_str("{#-10 [ ], #3 [100], #6 [10], #10 [6, 10], #100 [ ], #500 [-10]}", dgraph_to_string(other, NULL));

    assert_equals_int(dgraph_hash_code(graph, NULL), dgraph_hash_code(other, NULL));
    assert_equals_int(1, dgraph_equals(graph, other, NULL));

    dgraph_rm_node(graph, 10);
    assert_equals_str("{#-10 [ ], #3 [100], #6 [ ], #100 [ ], #500 [-10]}", dgraph_to_string(graph, NULL));
    assert_equals_int(0, dgraph_equals(graph, other, NULL));

    assert_equals_int(1, dgraph_has_edge(graph, 500, -10));
    assert_equals_int(0, dgraph_has_edge(graph, 10, 10));
    assert_equals_int(1, dgraph_has_edge(other, 10, 10));
    assert_equals_int(1, dgraph_has_edge(other, 10, 6));
    assert_equals_int(1, dgraph_has_node(other, 100));
    assert_equals_int(0, dgraph_has_node(other, -100));
    assert_equals_int(1, dgraph_has_node(other, -10));
}
