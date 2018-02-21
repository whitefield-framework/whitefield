# Whitefield compared to Cooja
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
|Energy Consumption model| PowerTracker | In the future |

^ Managed by Airline module which depends on NS3 like implementation to provide these features.

^^ In the first phase, Whitefield will provide command line tools to,
  - dump network topology based on tree-like structure or using position based network topology using GraphViz
  - monitor runtime tables (routing table for e.g.) and stats (MAC, IPv6, RPL, 6Lo, UDP) on any of the nodes.
  
In the future, Whitefield aims to have advanced visualization using sigmajs like SDKs. It is our intent (manifested in design) to loosely couple visualization from actual simulation for e.g. by having simulation modules send events/info on sockets to remote visualization tools.

Regarding use of emulated hardware: Contiki uses cross-compilers and MSPSim tools elegantly coupled with Cooja to provide this feature. While Whitefield can also support such a feature (since it is technically feasible), we currently do not have plans to integrate such tools as part of Whitefield.

Scalability: Cooja uses JVM coupled with MSPSim and the combination takes a real dig at computational and other resources. While it is purely our obervation that Cooja stutters badly beyond 150 nodes, other users may have different opinion depending upon the hardware they use. We tested Whitefield and Cooja on the same machine for longer duration to make this observation. We ll put the detailed configs very soon.

One of the biggest advantage of using Cooja/Contiki's MAC layer is that contiki supports some advanced RDC mechanism with optimizations such PLO (Phase loop optimizations). In case of Whitefield, it depends upon Airline module's support for such optimizations. While Castalia can support some advanced RDC mechanisms, the current phase of Whitefield does not support Castalia. But since Cooja's air interface is not as realistic as NS3/Castalia thus using advanced Contiki RDC mechanism on Cooja's air interface may not result in realistic behaviour.

# Whitefield compared to NS3/other simulation framework
Whitefield uses NS3 internally for actual phy/mac simulation, so naturally question arises as to what is the difference of using NS3 in whitefield rather than using NS3 directly:
1. To use NS3 simulation framework directly you need to port the network stack to NS3 model ... This is often time-consuming and error-prone.
2. NS3/DCE compared to Whitefield?
3. Is there any disadvantage with using NS3 in whitefield? The primary problem is that all the experiments now run in real-time and not in simulation-time. This has major implications on the amount of time take to complete the experiments.
