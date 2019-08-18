include config.inc

MAKE=make -s

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
STACKLINE_DEPCLEAN+=openthread_clean
endif

ifneq ($(AIRLINE_NS3),)
AIRLINE_DEP+=ns3
AIRLINE_DEPCLEAN+=ns3_clean
endif


all: $(AIRLINE_DEP) whitefield $(STACKLINE_DEP)

whitefield:
	$(MAKE) -f src/commline/Makefile.commline all
	$(MAKE) -f src/utils/Makefile.utils all
	$(MAKE) -f src/airline/Makefile.airline all

riot:
	$(MAKE) -C $(STACKLINE_RIOT)/tests/whitefield

riot_clean:
	$(MAKE) -C $(STACKLINE_RIOT)/tests/whitefield clean

contiki:
	$(MAKE) -C $(STACKLINE_CONTIKI)/examples/ipv6/rpl-udp TARGET=whitefield

contiki_clean:
	$(MAKE) -C $(STACKLINE_CONTIKI)/examples/ipv6/rpl-udp TARGET=whitefield clean

openthread:
	@if [ -d $(STACKLINE_OPENTHREAD) ]; then \
		cd $(STACKLINE_OPENTHREAD); \
		if [ ! -d output ]; then \
			echo "Bootstrapping OpenThread...";\
			./bootstrap; \
		fi; \
		$(MAKE) -f examples/Makefile-whitefield ALL_LOGS=1; \
	fi
	#if [ -d $(STACKLINE_OPENTHREAD) ]; then $(MAKE) -f src/stackline/wf_openthread/Makefile.openthread; fi

openthread_clean: 
	cd $(STACKLINE_OPENTHREAD); $(MAKE) -f examples/Makefile-whitefield clean

ns3:
	$(MAKE) -C $(AIRLINE_NS3)

ns3_clean:
	$(MAKE) -C $(AIRLINE_NS3) clean

clean:
	@rm -rf bin log pcap

allclean: $(STACKLINE_DEPCLEAN)
	$(MAKE) clean

tests:
	regression/regress.sh regression/full.set

