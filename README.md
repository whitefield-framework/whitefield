# Whitefield

Whitefield aims to provide a simulation environment for wireless sensor network by combining RF simulation provided by NS3, Castalia/Omnet++ while allowing to use native mode configuration of popular IoT OSes such as Contiki and RiOT.

Whitefield uses PHY/MAC layer provided by NS3/Omnet and loosely couples it with the 6Lowpan stacks supported by Contiki and RiOT(future), thus providing the best of both worlds by simply glueing things. On top of this it will provide kickass visualization and OAM tools which should really simplify monitoring and troubleshooting.

Some background: There are lot of papers/IETF drafts making tall claims about protocol performance and providing the observations from Cooja simulation. We worked on many such papers (primarily related to RPL/6lo's perf optimizations) and implemented the algorithms, later to find out that the claims are unrealistic because of practical issues such as RF inteference/path loss model/asymmetric transient links. Whitefield aims to provide a realistic RF simulation using well-known implementations such as NS3/Omnet and hope that the experiments conducted would be more closer to reality.

Typical use of Whitefield:
1. Interoperability testing of various IOT protocol stack implementations without having to adapt to NS3 or Castalia based environment while still retaining more-realistic RF simulations provided by NS3/Omnet.
2. Large scale interop testing. It can scale to thousands of nodes.
3. Validate RF phy/mac simulation against each other. For e.g. compare results in NS3 vs Castalia
4. Regression tests using IdealAir(future)

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")
### Whitefield as compared to Cooja
| Feature | Cooja | Whitefield |
| --- | --- | --- |
| Interop testing | No | Yes |
| Scale | <150 nodes | >1K nodes |
| RF model | No | Yes^ |
| Loss model | No | Yes^ |
| Asymmetric Links | Statically Configured | Dynamic^ |
| Visualization | Good | Cmdline tools^^ |
| Emulated Hardware | Yes | No |
| PHY/MAC/RDC | Provided by Contiki | Provided by NS3/Castalia |

^ Managed by Airline module which depends on NS3 like implementation to provide these features.

^^ In the first phase, Whitefield will provide command line tools to,
  - dump network topology based on tree-like structure or using position based network topology using GraphViz
  - monitor runtime tables (routing table for e.g.) and stats (MAC, IPv6, RPL, 6Lo, UDP) on any of the nodes.
  
In the future, Whitefield aims to have advanced visualization using sigmajs like SDKs. It is our intent (manifested in design) to loosely couple visualization from actual simulation for e.g. by having simulation modules send events/info on sockets to remote visualization tools.

Regarding use of emulated hardware: Contiki uses cross-compilers and MSPSim tools elegantly coupled with Cooja to provide this feature. While Whitefield can also support such a feature (since it is technically feasible), we currently do not have plans to integrate such tools as part of Whitefield.

Scalability: Cooja uses JVM coupled with MSPSim and the combination takes a real dig at computational and other resources. While it is purely our obervation that Cooja stutters badly beyond 150 nodes, other users may have different opinion depending upon the hardware they use. We tested Whitefield and Cooja on the same machine for longer duration to make this observation. We ll put the detailed configs very soon.

One of the biggest advantage of using Cooja/Contiki's MAC layer is that contiki supports some advanced RDC mechanism with optimizations such PLO (Phase loop optimizations). In case of Whitefield, it depends upon Airline module's support for such optimizations. While Castalia can support some advanced RDC mechanisms, the current phase of Whitefield does not support Castalia. But since Cooja's air interface is not as realistic as NS3/Castalia thus using advanced Contiki RDC mechanism on Cooja's air interface may not result in realistic behaviour.

