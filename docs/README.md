## Advanced Whitefield Customisations

### How to add a new RF simulator using Airline/Commline interfaces
The Airline module contains an RF simulator (such as NS3) which provides PHY and MAC layer. The MAC layer is tightly coupled with the physical layer and hence it is expected that the Airline module (be it NS3/Omnet/Opnet) provides both PHY/MAC layer. One example reason for such tight coupling is the case where the MAC layer depends upon "carrier sensing" interfaces provided by physical layer. Loose coupling these calls may cause severe performance impact since these interfaces are called much too often. Thus the general expectation of the Airline module is:
1. Provide a PHY layer with the desired properties
2. Provide a MAC/LLC layer including MAC framing capability.
3. Receive messages from commline, the message contains the packet buffer (IP layer and above) along with metadata in the form of src node_id, dst node_id. The Airline module is responsible for handing out the packet to the appropriate node's network interface after doing the MAC layer framing. Usually there are interfaces provided by NS3/Omnet/... for handling such tasks and one can look at airline/NS3 implementation to understand better.

Current Limitations:
1. The implementation uses uint16_t as node id. Thus maximum of 65K nodes are allowed in one network. Please note that node_id is used to map to the MAC interface identifier (IID) but the length of MAC IID may be different than uint16_t. For example, the NS3 lr-wpan (RF simulator based on 802.15.4) based Airline module can support short address(2B) or extended address (8B) addressing mode as per design. But the actual implementation of NS3 currently supports only 2B addressing mode so the MAC framing is done using short address mode only.

### How to add a new IoT protocol stack using Stackline/Commline interfaces
  1. Explain changes to Contiki
  2. Explain changes to RiOT
