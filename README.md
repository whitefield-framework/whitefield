![Alt text](docs/res/Logo.png "Whitefield Logo")

[![GitPitch](https://gitpitch.com/assets/badge.svg)](https://gitpitch.com/nyrahul/whitefield) [![Build Status](https://travis-ci.org/whitefield-framework/whitefield.svg?branch=master)](https://travis-ci.org/whitefield-framework/whitefield)
[![Join the chat at https://gitter.im/whitefield-fw](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/whitefield-fw?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Whitefield provides a simulation environment for wireless sensor network by combining realistic RF simulation with the native mode use of popular IoT stacks/OSes such as [Contiki](http://www.contiki-os.org/)/[RIOT](https://riot-os.org/)/[OpenThread](https://openthread.io/). Thus one can use existing stack implementation as is and test it on top of realistic RF simulation.

Whitefield uses PHY/MAC layer provided by popular simulators (such as NS3) and loosely couples it with the constrained network stacks supported by Contiki/RIOT, thus providing the best of both worlds by simply glueing things. It provides common visualization and OAM tools (logs, pcap, shell) which simplifies monitoring and troubleshooting.

Typical use of Whitefield:
1. Large scale testing of sensor networks
2. Large scale interop testing between multitudes of stacks in same wireless environment. It can scale to thousands of nodes on general purpose server (even laptops).
3. Validate RF phy/mac simulation against each other. For e.g. compare results in NS3 vs Castalia
4. Regression tests using IdealAir(future).

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")

**Terminology**:

[Airline](src/airline): Airline refers to PHY/MAC/RDC implementation provided by thirdparty simulators such as NS3/Castalia-Omnet++. Provides configuration management and OAM tools which works across any thirdpaty simulators. It interfaces with Commline to send/recv messages to/from Stackline.

[Commline](src/commline): Message queue and corresponding interfaces which decouples Airline and Stackline. All the messages be it the network payloads or OAM messages are handshaked using Commline.

[Stackline](src/stackline): 6lowpan/Network layer and above protocol stack provided by thirdparty IoT OSes such as Contiki/RIOT. A new platform is added in such OSes to interface with Whitefield. The protocol stack with application will be used as-is from these OSes.

### Setup Instructions
Tested on: Ubuntu 16.04 (xenial), Debian GNU/Linux 8.7 (jessie), Ubuntu 18.04 (bionic beaver)

**System Requirements**:

Disk Space: ~1.5GB
**Build Instructions**:

Assumes Airline=NS3, Stackline=Contiki (example app=contiki/examples/ipv6/rpl-udp)
```
git clone https://github.com/whitefield-framework/whitefield
cd whitefield
git submodule update --init
git submodule foreach git pull origin master
./scripts/setup.sh
```
**Execute Instructions**:
```
#Start Whitefield
$./invoke_whitefield.sh config/wf.cfg

#Monitor whitefield status
$./scripts/monitor.sh

#View live network toplogy
$./canvas/canvas.py
#open http://localhost:8080/ in a browser

#Whitefield shell
$./scripts/wfshell help

#Stop whitefield
$./scripts/wfshell stop_whitefield
```

* ### [Configuration manual](docs/wf_config_help.md "Whitefield Configuration")
* ### [OAM/Monitor command manual](docs/wf_oam_help.md "Whitefield OAM commands")
* ### [Using RIOT/Contiki with Whitefield](docs/wf_using_riot_contiki.md "Whitefield with Contiki/RIOT")
* ### [Using OpenThread with Whitefield](docs/wf_using_openthread.md "Whitefield with OpenThread")
* ### [Whitefield compared to Cooja/NS3](docs/wf-vs-cooja.md "Whitefield compared to Cooja/NS3")
* ### [Module support table](docs/README.md "Module support table")
* ### [Forum](https://groups.google.com/forum/?pli=1#!forum/whitefield-fw "Mailing List")

### Work done in Whitefield

* ### [RPL Efficient Route Invalidation](https://github.com/nyrahul/ietf-data/blob/master/DCO_performance_report.md)
* ### [6Lo Fragment Forwarding](https://github.com/nyrahul/ietf-data/blob/master/6lo-fragfwd-perf-report.rst)

