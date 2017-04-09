test_programs = \
	test-gtd-simple-test \
	$(NULL)

TESTS = test-gtd-simple-test
test_gtd_simple_test_SOURCES = test-gtd-simple-test.c
test_gtd_simple_test_CFLAGS = $(GNOME_TODO_CFLAGS)
test_gtd_simple_test_LDADD = $(GNOME_TODO_LIBS)

if ENABLE_TESTS
noinst_PROGRAMS = $(TESTS)
endif

check_PROGRAMS = $(TESTS)

