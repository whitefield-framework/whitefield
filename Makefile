all:
	make -f src/commline/Makefile.commline all
	make -f src/tools/Makefile.tools all
	make -f src/airline/Makefile.airline all

clean:
	@make -f src/airline/Makefile.airline clean
