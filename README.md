# Whitefield

Whitefield aims to provide a simulation environment for wireless sensor network by combining RF simulation provided by NS3, Castalia/Omnet++ while allowing to use native mode configuration of popular IoT OSes such as Contiki and RiOT.
Whitefield uses PHY/MAC layer provided by NS3/Omnet and loosely couples it with the 6Lowpan stacks supported by Contiki and RiOT(future). Thus providing with the best of both worlds. On top of this it provides visualization and OAM tools which helps in monitoring and troubleshooting the protocols with less efforts.

Typical use of Whitefield:
1. Interoperability testing of various IOT protocol stack implementations without having to adapt to NS3 or Castalia based environment while still retaining more-realistic RF simulations provided by NS3/Omnet.
2. Large scale interop testing. It can scale to thousands of nodes.
3. Validate RF phy/mac simulation against each other. For e.g. compare results in NS3 vs Castalia
4. Regression tests using IdealAir(future)

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")
### Whitefield as compared to Cooja
| Feature | Cooja | Whitefield |
| --- | --- | --- |
| Scale | <100 nodes | ~1K nodes |
| RF model | No | Yes^ |
| Loss model | No | Yes^ |
| Asymmetric Links | Statically Configured | Dynamic^ |
| Visualization | Good | Cmdline tools^^ |

^ Managed by Airline module which depends on NS3 like implementation to provide these features.

^^ In the first phase, Whitefield will provide command line tools to,
  - dump network topology based on tree-like structure or using position based network topology
  - monitor runtime tables (routing table for e.g.) and stats (MAC, IPv6, RPL, 6Lo, UDP) on any of the nodes.
  
In the future, Whitefield aims to have advanced visualization using sigmajs like SDKs. It is our intent (manifested in design) to loosely couple visualization from actual simulation for e.g. by having simulation modules send events/info on sockets to remote visualization tools.
