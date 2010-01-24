add_executable(plfit main.c error.c getopt.c kolmogorov.c plfit.c test.c zeta.c)
target_link_libraries(plfit m)
