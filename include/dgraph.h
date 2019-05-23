#ifndef _DGRAPH_H
#define _DGRAPH_H

#include <stddef.h>

struct dgraph_handle;
typedef struct dgraph_handle *dgraph_t;

typedef void *(*dgraph_allocator)(void *ud, void *ptr, size_t size);
typedef char *(*dgraph_element_to_string)(const void *);
typedef int (*dgraph_element_equals)(const void *, const char *);
typedef int (*dgraph_element_hash_code)(const void *);
typedef int (*dgraph_visitor)(const dgraph_t graph, void *ud, int n, void **content, size_t size, const int list[]);

dgraph_t dgraph_new(void *alloc_ud, dgraph_allocator alloc);
dgraph_t dgraph_create();
void **dgraph_retrive_node(dgraph_t graph, int n, int *is_new);
int dgraph_set(dgraph_t graph, int a, int b);
int dgraph_set_list(dgraph_t graph, int n, size_t size, const int list[]);
int dgraph_remove(dgraph_t graph, int a, int b);
int dgraph_rm_list(dgraph_t graph, int n, size_t size, const int list[]);
int dgraph_rm_node(dgraph_t graph, int n);
int dgraph_init(dgraph_t graph, size_t size, const int *const *data);
size_t dgraph_nodes(const dgraph_t graph);
size_t dgraph_edges(const dgraph_t graph);
char *dgraph_to_string(const dgraph_t graph, dgraph_element_to_string tostr);
int dgraph_equals(const dgraph_t first, const dgraph_t second, dgraph_element_equals eq);
int dgraph_hash_code(const dgraph_t graph, dgraph_element_hash_code hc);
int dgraph_has_node(const dgraph_t graph, int n);
int dgraph_has_edge(const dgraph_t graph, int a, int b);
size_t dgraph_get(const dgraph_t graph, int n, const int *list[]);
void dgraph_visit(const dgraph_t graph, dgraph_visitor visitor, void *ud);
void dgraph_clear(dgraph_t graph);
void dgraph_delete(dgraph_t graph);

#endif // _DGRAPH_H
