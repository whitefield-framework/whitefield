* [Using Contiki in Whitefield](#using-contiki-in-whitefield)
* [Using RIOT in Whitefield](#using-riot-in-whitefield)

# Using Contiki in Whitefield
## Whitefield's mode of integration for contiki
Whitefield is added as a TARGET platform in Contiki ([forked version](https://github.com/whitefield-framework/contiki)) of the code. The whitefield integration code can be found in the forked version at [this](https://github.com/whitefield-framework/contiki/tree/master/platform/whitefield) location.

Any contiki application can be compiled for whitefield execution by just specifying 'make TARGET=whitefield'.

The default setup makes use of examples/ipv6/rpl-udp application.

Please note Whitefield has not been tested with the Contiki-NG fork. 

## Using a different contiki application

# Using RIOT in Whitefield

Whitefield is integrated in RIOT as a third party package. Check the code [here](https://github.com/whitefield-framework/RIOT/tree/master/pkg/whitefield).
