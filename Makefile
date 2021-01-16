all:
	@mkdir -p obj
	@$(CC) blk_malloc.c -c -o obj/blk_malloc.o
	@mkdir -p rls
	@ar -rcs rls/libblk_malloc.a obj/blk_malloc.o
	@cp blk_malloc.h rls	

.PHONY: clean test

test: clean
	@$(CC) blk_malloc.c tests.c -DTEST -g3 -gdwarf -Wall -pedantic -std=c11 \
		                          -fprofile-arcs -ftest-coverage \
		-o test
	@./test
	@gcov blk_malloc.c

clean:
	@rm -rf test obj rls *.gcov *.gcda *.gcno
