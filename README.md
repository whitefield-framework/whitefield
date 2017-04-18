# Whitefield

Whitefield aims to provide a simulation environment for wireless sensor network by combining RF simulation provided by NS3, Castalia/Omnet++ while allowing to use native mode configuration of popular IoT OSes such as Contiki and RiOT.
Whitefield uses PHY/MAC layer provided by NS3/Omnet and loosely couples it with the 6Lowpan stacks supported by Contiki and RiOT(future). Thus providing with the best of both worlds. On top of this it provides visualization and OAM tools which helps in monitoring and troubleshooting the protocols with less efforts.

Typical use of Whitefield:
1. Interoperability testing of various IOT protocol stack implementations without having to adapt to NS3 or Castalia based environment while still retaining more-realistic RF simulations provided by NS3/Omnet.
2. Large scale interop testing
3. Provide realistic RF environment by utilizing phy/mac of the proven RF simulators such as NS3/Castalia.
4. Validate RF phy/mac simulation against each other. For e.g. compare results in NS3 vs Castalia
5. Regression tests using IdealAir

![Alt text](docs/res/Whitefield%20-%20HLD.png "Whitefield-High Level design")
