#include "test.h"

#include "internal_dgraph.h"

void test() {
	// Test calls in to_string
	assert_equals_int(1, dgraph_util_measure_int(0));
	assert_equals_int(5, dgraph_util_measure_int(-3452));
	assert_equals_int(4, dgraph_util_measure_int(3428));
	assert_equals_int(1, dgraph_util_measure_int(1));
	assert_equals_int(2, dgraph_util_measure_int(-1));
}
