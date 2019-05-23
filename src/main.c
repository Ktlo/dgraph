#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <dgraph.h>
#include <dgraph_algs.h>

struct arguments_struct {
	const char *target;
	FILE *input;
	FILE *output;
} arguments;

void init_args(int argc, char *argv[]);

char *get_main_name(const char *line, int sz);

struct find_closure {
	const char *name;
	int n;
};

int find_node_with_name(const dgraph_t graph, void *ud, int n, void **content, size_t size, const int list[]) {
	(void)graph; (void)size; (void)list;
	struct find_closure *closure = ud;
	if (*content && !strcmp(closure->name, *content)) {
		closure->n = n;
		return 1;
	}
	return 0;
}

char *str_to_str(const void *obj) {
	size_t sz = strlen(obj);
	char *result = malloc((sz + 1)*sizeof(char));
	strcpy(result, obj);
	return result;
}

typedef struct easy_list {
	int *list;
	size_t size;
} *easy_list_t;

void dependency_walker(dgraph_t graph, easy_list_t list, int target);
void filter_list(easy_list_t list);

#define BUFF_INPUT_SIZE 4096

int main(int argc, char *argv[]) {
	init_args(argc, argv);
	char line[BUFF_INPUT_SIZE];
	int read;
	dgraph_t graph = dgraph_create();
	while (fgets(line, BUFF_INPUT_SIZE, arguments.input)) {
		read = strlen(line);
		char *node_name;
        if (isalpha(line[0])) {
			node_name = get_main_name(line, read);
			if (!node_name)
				continue;
			struct find_closure closure = { node_name, -1 };
			if (!node_name) {
				fputs("format error", stderr);
				return -1;
			}
			dgraph_visit(graph, &find_node_with_name, &closure);
			int n;
			if (closure.n == -1) {
				int is_new;
				n = dgraph_nodes(graph);
				*dgraph_retrive_node(graph, n, &is_new) = node_name;
				assert(is_new);
			} else {
				free(node_name);
				n = closure.n;
			}
			int i;
			for (i = 0; line[i] != ':'; i++);
			while (isspace(line[++i]));
			char *word = strtok(line + i, " \t\n\r");
			while (word) {
				closure.name = word;
				closure.n = -1;
				dgraph_visit(graph, &find_node_with_name, &closure);
				int goal;
				if (closure.n == -1) {
					size_t size = strlen(word);
					char *copy_word = malloc((size + 1)*sizeof(char));
					memcpy(copy_word, word, size);
					copy_word[size] = '\0';
					goal = dgraph_nodes(graph);
					int is_new;
					*dgraph_retrive_node(graph, goal, &is_new) = copy_word;
					assert(is_new);
				} else {
					goal = closure.n;
				}
				dgraph_set(graph, n, goal);
				word = strtok(NULL, " \t\n\r");
			}
		}
    }
#	ifdef DEBUG
		char *out_str;
		fputs(out_str = dgraph_to_string(graph, &str_to_str), stderr);
		putc('\n', stderr);
		free(out_str);
#	endif
	int cycle = dgraph_find_cycle(graph);
	if (cycle != -1) {
		char *dep_name = *dgraph_retrive_node(graph, cycle, NULL);
		fprintf(stderr, "ERROR: target '%s' has cycles in dependency tree\n", dep_name);
		return 5;
	}
	struct easy_list list = { malloc(dgraph_nodes(graph)*sizeof(int)), 0 };
	struct find_closure closure = { arguments.target, -1 };
	if (closure.name)
		dgraph_visit(graph, &find_node_with_name, &closure);
	else
		closure.n = 0;
	if (closure.n == -1) {
		fprintf(stderr, "ERROR: specified target '%s' do not exists\n", arguments.target);
		return 6;
	}
	dependency_walker(graph, &list, closure.n);
	filter_list(&list);
	size_t i;
	for (i = 0; i < list.size; i++) {
		char *target_name = *dgraph_retrive_node(graph, list.list[i], NULL);
		fprintf(arguments.output, "%s ", target_name);
	}
	putc('\n', arguments.output);
	return 0;
}

char *get_main_name(const char *line, int sz) {
	int i;
	for (i = 0; i < sz; i++) {
		if (line[i] == ':' || isspace(line[i])) {
			char *result = malloc((i+1)*sizeof(char));
			memcpy(result, line, i*sizeof(char));
			result[i] = '\0';
			return result;
		}
	}
	return NULL;
}

void filter_list(easy_list_t list) {
	size_t i, j;
	for (i = 0; i < list->size; i++) {
		int target = list->list[i];
		for (j = i + 1; j < list->size; j++) {
			if (target == list->list[j]) {
				memmove(list->list + j, list->list + j + 1, (--list->size - j)*sizeof(int));
				j--;
			}
		}
	}
}

void dependency_walker(dgraph_t graph, easy_list_t list, int target) {
	const int *ingredients;
	size_t count = dgraph_get(graph, target, &ingredients);
	size_t i;
	for (i = 0; i < count; i++) {
		dependency_walker(graph, list, ingredients[i]);
	}
	list->list[list->size++] = target;
}

void init_args(int argc, char *argv[]) {
	int i;
	arguments.target = NULL;
	arguments.input = stdin;
	arguments.output = stdout;

	char *word;

	for (i = 1; i < argc; i++) {
		word = argv[i];
		if (!strcmp(word, "--input") || !strcmp(word, "-i")) {
			if (++i == argc)
				goto fail_no_arg;
			arguments.input = fopen(argv[i], "r");
			if (!arguments.input)
				goto fail_no_file;
		} else if (!strcmp(word, "--output") || !strcmp(word, "-o")) {
			if (++i == argc)
				goto fail_no_arg;
			arguments.output = fopen(argv[i], "w");
			if (!arguments.output)
				goto fail_no_file;
		} else if (!strcmp(word, "--target") || !strcmp(word, "-t")) {
			if (++i == argc)
				goto fail_no_arg;
			arguments.target = argv[i];
		}
	}
	return;
fail_no_arg:
	fprintf(stderr, "ERROR: invalid argument, no value specifiend for %s parameter\n", word);
	exit(1);
fail_no_file:
	fprintf(stderr, "ERROR: invalid argument '%s' for %s parameter: %s", argv[i], argv[i - 1], strerror(errno));
	exit(2);
}
