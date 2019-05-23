#include "test.h"

#include "dgraph.h"

#include <stdlib.h>
#include <string.h>

char *test_to_str(const void * obj) {
	const char *str = (const char *)obj;
	char *result = malloc((strlen(str) + 1)*sizeof(char));
	strcpy(result, str);
	return result;
}

void test() {
	// Test to_string
	dgraph_t graph = dgraph_create();
	assert_equals_int(1, graph != NULL);
	char *str = dgraph_to_string(graph, NULL);
	assert_equals_str("{ }", str);
	free(str);

	dgraph_set(graph, 3, 4);
	assert_equals_int(2, dgraph_nodes(graph));
	str = dgraph_to_string(graph, NULL);
	assert_equals_str("{#3 [4], #4 [ ]}", str);
	free(str);

	dgraph_set(graph, 3, 2);
	assert_equals_int(3, dgraph_nodes(graph));
	str = dgraph_to_string(graph, NULL);
	assert_equals_str("{#2 [ ], #3 [2, 4], #4 [ ]}", str);
	free(str);

	dgraph_set(graph, -1, -1);
	assert_equals_int(4, dgraph_nodes(graph));
	str = dgraph_to_string(graph, NULL);
	assert_equals_str("{#-1 [-1], #2 [ ], #3 [2, 4], #4 [ ]}", str);
	free(str);

	dgraph_remove(graph, 3, 2);
	assert_equals_int(4, dgraph_nodes(graph));
	str = dgraph_to_string(graph, NULL);
	assert_equals_str("{#-1 [-1], #2 [ ], #3 [4], #4 [ ]}", str);
	free(str);

	int is_new;

	*dgraph_retrive_node(graph, -1, &is_new) = "Hello World!!!";
	assert_equals_int(0, is_new);
	str = dgraph_to_string(graph, &test_to_str);
	assert_equals_str("{#-1 (Hello World!!!) [-1], #2 [ ], #3 [4], #4 [ ]}", str);
	free(str);
}
