Module support
==============

The best place to check whats up in the future is `Whitefield Project on Trello <https://trello.com/b/9bdugZxX>`__

Support Table
-------------

+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Stackline Modules       | Supported   | Remarks                                                                                |
+=========================+=============+========================================================================================+
| Contiki                 | Yes         |                                                                                        |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Contiki-NG              | Yes         |                                                                                        |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| RIOT                    | Yes         |                                                                                        |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| OpenThread              | Future      | Part of the support is added but not complete                                          |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Zephyr                  | Future      |                                                                                        |
+-------------------------+-------------+----------------------------------------------------------------------------------------+

+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Airline Modules         | Supported   | Remarks                                                                                |
+=========================+=============+========================================================================================+
| 802.15.4                | Yes         | supports `NS3 lrwpan <https://www.nsnam.org/docs/models/html/lr-wpan.html>`__ module   |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| PLC                     | Yes         | supports `NS3 PLC <https://github.com/ns3-plc-module/plc>`__ module                    |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Static Visualization    | Yes         | Using Graphviz to plot network diagram                                                 |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Dynamic Visualization   | Yes         | Canvas tool which plots nodes and their edges dynamically using Cytoscape.js           |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Castalia-Omnet          | Future      | Not sure if Omnet license allows integrating with Whitefield                           |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| 802.15.4e               | Future      | Not planned yet. Requires submitting changes to NS3                                    |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| LoRAWAN                 | Future      | Possibly based on `NS3-lorawan <https://github.com/signetlabdei/lorawan>`__            |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| WAVE/DSRC               | Future      |                                                                                        |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Runtime Mobility        | Future      | Currently it is possible to change the node's location using `cmd_set_node_position`   |
+-------------------------+-------------+----------------------------------------------------------------------------------------+

+-------------------------+-------------+----------------------------------------------------------------------------------------+
| General Modules         | Supported   | Remarks                                                                                |
+=========================+=============+========================================================================================+
| Static Visualization    | Yes         | Using Graphviz to plot network diagram                                                 |
+-------------------------+-------------+----------------------------------------------------------------------------------------+
| Dynamic Visualization   | Yes         | Canvas tool which plots nodes and their edges dynamically using Cytoscape.js           |
+-------------------------+-------------+----------------------------------------------------------------------------------------+

Advanced Customisations
-----------------------

How to add a new RF simulator using Airline/Commline interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Airline module contains an RF simulator (such as NS3) which provides
PHY and MAC layer. The MAC layer is tightly coupled with the physical
layer and hence it is expected that the Airline module (be it
NS3/Omnet/Opnet) provides both PHY/MAC layer. One example reason for
such tight coupling is the case where the MAC layer depends upon
"carrier sensing" interfaces provided by physical layer. Loose coupling
these calls may cause severe performance impact since these interfaces
are called much too often. Thus the general expectation of the Airline
module is: 1. Provide a PHY layer with the desired properties 2. Provide
a MAC/LLC layer including MAC framing capability. 3. Receive messages
from commline, the message contains the packet buffer (IP layer and
above) along with metadata in the form of src node\_id, dst node\_id.
The Airline module is responsible for handing out the packet to the
appropriate node's network interface after doing the MAC layer framing.
Usually there are interfaces provided by NS3/Omnet/... for handling such
tasks and one can look at airline/NS3 implementation to understand
better.

Current Limitations: 1. The implementation uses uint16\_t as node id.
Thus maximum of 65K nodes are allowed in one network. Please note that
node\_id is used to map to the MAC interface identifier (IID) but the
length of MAC IID may be different than uint16\_t. For example, the NS3
lr-wpan (RF simulator based on 802.15.4) based Airline module can
support short address(2B) or extended address (8B) addressing mode as
per design. But the actual implementation of NS3 currently supports only
2B addressing mode so the MAC framing is done using short address mode
only.

How to add a new IoT protocol stack using Stackline/Commline interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Explain changes to Contiki
2. Explain changes to RIOT

