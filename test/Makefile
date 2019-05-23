src = ${wildcard *.c}
dep = ${src:.c=.d}
CFLAGS:=$(CFLAGS) -DTEST
DEFAULT_TEST:= ${src:.c=.test}
test:=${or ${TESTS},${TESTS},${DEFAULT_TEST}}

test: $(test)

%.test: %.c ../src/lib${NAME}.a
	$(CC) $(CFLAGS) -I../src $< -L../src -l$(NAME) -o $@
	$(GDB) ./$@

.PHONY: clean
clean:
	$(RM) $(DEFAULT_TEST)
