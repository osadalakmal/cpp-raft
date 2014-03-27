TEST_DIR = ./tests

GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
SHELL  = /bin/bash
CC     = g++
CCFLAGS = -g -O0 $(GCOV_CCFLAGS) -I./ -w -fpermissive -fno-inline-functions #-fno-omit-frame-pointer -fno-common -fsigned-char
SRCS = state_mach.cpp raft_server.cpp raft_logger.cpp raft_node.cpp $(TEST_DIR)/main_test.cpp $(TEST_DIR)/test_server.cpp $(TEST_DIR)/test_node.cpp $(TEST_DIR)/test_log.cpp $(TEST_DIR)/test_scenario.cpp $(TEST_DIR)/mock_send_functions.cpp $(TEST_DIR)/CuTest.c

all: tests_main

$(TEST_DIR)/main_test.c:
	cd $(TEST_DIR) && sh make-tests.sh "test_*.c" > main_test.c && cd ..

tests_main: $(SRCS)
	$(CC) $(CCFLAGS) -o $@ $^
	./tests_main

clean:
	rm -f $(TEST_DIR)/main_test.c *.o $(GCOV_OUTPUT)
