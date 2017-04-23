# Whitefield

Whitefield aims to provide a simulation environment for wireless sensor network by combining RF simulation provided by NS3, Castalia/Omnet++ while allowing to use native mode configuration of popular IoT OSes such as Contiki/RIOT or any other in the future.

Whitefield uses PHY/MAC layer provided by NS3/Omnet and loosely couples it with the 6Lowpan stacks supported by Contiki and RiOT(future), thus providing the best of both worlds by simply glueing things. On top of this it will provide kickass visualization and OAM tools which should really simplify monitoring and troubleshooting.

**Some background**: There are lot of papers/IETF drafts making tall claims about protocol performance and providing the observations from Cooja/Matlab simulation. We worked on few such papers (primarily related to RPL/6lo's perf optimizations) and implemented the algorithms, later to find out that the claims were unrealistic because of practical issues such as RF inteference/path losses/asymmetric transient links. Whitefield aims to provide a realistic RF simulation using well-known implementations such as NS3/Castalia-Omnet and hope that the experiments conducted thus would be more closer to reality.

Typical use of Whitefield:
1. Interoperability testing of various IOT protocol stack implementations without having to adapt to NS3 or Castalia based environment while still retaining more-realistic RF simulations provided by NS3/Omnet.
2. Large scale interop testing. It can scale to thousands of nodes.
3. Validate RF phy/mac simulation against each other. For e.g. compare results in NS3 vs Castalia
4. Regression tests using IdealAir(future)

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")

### Setup Instructions
Tested on: Ubuntu >= 16.04 (xenial), Debian GNU/Linux 8.7 (jessie)

On ubuntu 14.04, ns3 build has issues.

**System Requirements**:

Disk Space: ~1.5GB
```
sudo apt-get update
sudo apt-get install gcc g++ make git graphviz
```
**Build Instructions**:

Assumes Airline=NS3, Stackline=Contiki (example app=contiki/examples/ipv6/rpl-udp)
```
git clone https://github.com/whitefield-framework/whitefield
cd whitefield
git submodule update --init --recursive
git submodule foreach git pull origin master
./setup.sh
```

### Whitefield as compared to Cooja (docs/wf-vs-cooja.md "Whitefield-High Level design")

