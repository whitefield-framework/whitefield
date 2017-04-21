all:
	make -f src/commline/Makefile.commline all
	make -f src/utils/Makefile.utils all
	make -f src/airline/Makefile.airline all

clean:
	@make -f src/airline/Makefile.airline clean
	@rm -rf log pcap
