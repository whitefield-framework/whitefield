all:
	make -f src/airline/Makefile.airline all
	make -f src/commline/Makefile.commline all

clean:
	@make -f src/airline/Makefile.airline clean
