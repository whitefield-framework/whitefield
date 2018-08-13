include config.inc

ifneq ($(STACKLINE_RIOT),)
STACKLINE_DEP+=riot
STACKLINE_DEPCLEAN+=riot_clean
endif

ifneq ($(STACKLINE_CONTIKI),)
STACKLINE_DEP+=contiki
STACKLINE_DEPCLEAN+=contiki_clean
endif

ifneq ($(STACKLINE_OPENTHREAD),)
STACKLINE_DEP+=openthread
endif

ifneq ($(AIRLINE_NS3),)
AIRLINE_DEP+=ns3
AIRLINE_DEPCLEAN+=ns3_clean
endif


all: $(AIRLINE_DEP) whitefield $(STACKLINE_DEP)

whitefield:
	make -f src/commline/Makefile.commline all
	make -f src/utils/Makefile.utils all
	make -f src/airline/Makefile.airline all

riot:
	make -C $(STACKLINE_RIOT)/tests/whitefield

riot_clean:
	make -C $(STACKLINE_RIOT)/tests/whitefield clean

contiki:
	make -C $(STACKLINE_CONTIKI)/examples/ipv6/rpl-udp TARGET=whitefield

contiki_clean:
	make -C $(STACKLINE_CONTIKI)/examples/ipv6/rpl-udp TARGET=whitefield clean

openthread:
	if [ -d $(STACKLINE_OPENTHREAD) ]; then make -f src/stackline/wf_openthread/Makefile.openthread; fi

ns3:
	make -C $(AIRLINE_NS3)

ns3_clean:
	make -C $(AIRLINE_NS3) clean

clean:
	@rm -rf bin log pcap

allclean: $(STACKLINE_DEPCLEAN)
	make clean

tests:
	regression/regress.sh regression/full.set
