###########
Terminology
###########

.. image:: res/module_design.png

* Airline
    Airline refers to PHY/MAC/RDC implementation provided by thirdparty simulators such as NS3/Castalia-Omnet++
* Stackline
    6lowpan/Network layer and above protocol stack provided by thirdparty IoT OSes such as Contiki/RIOT. A new platform is added in such OSes to interface with Whitefield. The protocol stack with application will be used as-is from these OSes.
* Commline
    Message queue and corresponding interfaces which decouples Airline and Stackline. All the messages be it the network payloads or OAM messages are handshaked using Commline.

