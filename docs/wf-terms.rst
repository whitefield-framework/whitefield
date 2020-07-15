###########
Terminology
###########

.. image:: res/module_design.png

Whitefield decouples the physical layer from above layers.

Airline
-------
Airline refers to the physical(+mac) layer.
Airline implementation is handled by thirdparty simulators such as NS3.

Stackline
---------
These are the real-world protocol stacks. 6lowpan/Network layer and above protocol stack provided by thirdparty IoT OSes such as Contiki/RIOT. A new platform is added in such OSes to interface with Whitefield. The protocol stack with application will be used as-is from these OSes.

Commline
--------
Message queue and corresponding interfaces which decouples Airline and Stackline. All the messages be it the network payloads or OAM messages are transported using Commline.

NodeID
------
Whitefield assigns an internal nodeid to all the nodes. Note that this node id may not necessarily translate to the mac-address. The nodeid starts with 0 and reaches numOfNodes-1, where numOfNodes is part of the configuration. The wfshell commands refers to this NodeID when invoking a cmd_* command.