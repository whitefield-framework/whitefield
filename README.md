![Alt text](docs/res/Logo.png "Whitefield Logo")

[![GitPitch](https://img.shields.io/badge/What%20is%20Whitefield%3F-Slides-Violet)](https://docs.google.com/presentation/d/e/2PACX-1vRE-8g-tyh7MwECHnWwYGC9mJWKVOCQnWWwUjD-5wnvu_fW1w75h_K91UM1FV9QOMcNWYW2mdFxds9J/pub?start=false&loop=false&delayms=3000)
[![Setup & Regress](https://github.com/whitefield-framework/whitefield/actions/workflows/setup_regress.yml/badge.svg)
[![Doc status](https://readthedocs.org/projects/whitefield/badge/?version=latest)](https://whitefield.readthedocs.io/en/latest/)
[![Join the chat at https://gitter.im/whitefield-fw](https://img.shields.io/badge/Got%20Questions%3F-Chat-Violet)](https://github.com/whitefield-framework/whitefield/discussions)

Whitefield provides a simulation environment for sensor networks by
combining realistic PHY/MAC layer simulation with the native mode use of popular IoT
stacks/OSes such as [Contiki]/[RIOT]/[OpenThread]/[Zephyr]/[FreeRTOS]/[OT-RTOS]. 
Thus one can use existing stack implementation as is and test it on top of
realistic RF simulation.

Whitefield uses PHY/MAC layer provided by popular simulators (such as
[NS3-lrwpan]/PLC) and loosely couples it with the constrained network stacks
supported by popular IoT OSes, thus providing the best of both worlds.
It provides common visualization and OAM tools (logs, pcap, shell) which
simplifies monitoring and troubleshooting.

Typical use-cases of Whitefield:
1. Large scale realistic testing with real-world embedded stacks. Test
   thousands of nodes with realistic 802.15.4 and Power Line Communication
   interfaces.
2. Large scale interop testing between different network stacks in same environment.
3. Create reproducible work/experiments. Most of the experiments mentioned in the
   papers are difficult to be reproduced. Whitefield helps alleviate this
   issue.

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")

**Terminology**:

[Airline](src/airline): Airline refers to PHY/MAC/RDC implementation provided
by third-party simulators such as NS3/Castalia-Omnet++. Provides configuration
management and OAM tools which works across any third-party simulators. It
interfaces with Commline to send/recv messages to/from Stackline.

[Commline](src/commline): Message queue and corresponding interfaces which
decouples Airline and Stackline. All the messages, be it the network payloads or
OAM messages are transported using Commline.

[Stackline](src/stackline): 6lowpan/Network layer and above protocol stack
provided by third-party IoT OSes such as Contiki/RIOT. A new platform is added
in such OSes to interface with Whitefield. The protocol stack with application
will be used as-is from these OSes.

### Setup Instructions

<details>
  <summary>Build Instructions (Native Install)</summary>

```
git clone https://github.com/whitefield-framework/whitefield
cd whitefield
./scripts/setup.sh #This may take a while...
make
```
</details>

<details>
  <summary>Build Instructions (Vagrant Install)</summary>

```
git clone https://github.com/whitefield-framework/whitefield
cd whitefield
vagrant up	# <- step takes time
vagrant ssh
```
</details>

**Execute Instructions**:
```
$ cd whitefield

#Start Whitefield
$ ./invoke_whitefield.sh config/wf.cfg

#Monitor whitefield status
$ ./scripts/monitor.sh

#View live network toplogy open http://localhost:8080/ in a browser
$ ./canvas/canvas.py

#Whitefield shell
$ ./scripts/wfshell help

#Stop whitefield
$ ./scripts/wfshell stop_whitefield
```

* ### [Configuration manual](docs/wf-config-help.rst "Whitefield Configuration")
* ### [OAM/Monitor command manual](docs/wf_oam_help.md "Whitefield OAM commands")
* ### [Using RIOT/Contiki with Whitefield](docs/wf_using_riot_contiki.md "Whitefield with Contiki/RIOT")
* ### [Using OpenThread with Whitefield](docs/OpenThread.md "Whitefield with OpenThread")
* ### [Whitefield compared to Cooja/NS3](docs/wf-vs-cooja.md "Whitefield compared to Cooja/NS3")
* ### [Module support table](docs/wf-module-support.rst "Module support table")
* ### [Forum](https://groups.google.com/forum/?pli=1#!forum/whitefield-fw "Mailing List")

### Work done in Whitefield

* ### [RPL Efficient Route Invalidation](https://github.com/nyrahul/ietf-data/blob/master/DCO_performance_report.md)
* ### [6Lo Fragment Forwarding](https://github.com/nyrahul/ietf-data/blob/master/6lo-fragfwd-perf-report.rst)

[NS3-lrwpan]: https://www.nsnam.org/docs/models/html/lr-wpan.html
[Contiki]: http://www.contiki-os.org/
[RIOT]: https://riot-os.org/
[OpenThread]: https://openthread.io/
[FreeRTOS]: https://freertos.org/
[Zephyr]: https://www.zephyrproject.org/
[OT-RTOS]: https://openthread.io/platforms/ot-rtos

