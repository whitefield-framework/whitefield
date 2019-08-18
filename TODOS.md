## OpenThread TODOs:
1. Prepare call flows for documentation. This may help future drivers writing new porting interfaces.
2. How is OpenThread-Whitefield different from OpenThread-Posix
    a. ACKs are handled in Whitefield. (This is yet to be handled)


## Current Stage for OpenThread Integration:
Have created a separate platform for whitefield in OpenThread. Commline
init/deinit is handled but the actual commline send/recv is not handled as of
now. In radioTransmit/Receive the posix-UDP send/recv is done to tx/rx
messages. Using this I have verified that the Leader and Router node work fine.
Steps to use the OpenThread integration:
1. ./invoke\_whitefield.sh config/wf\_ot.cfg ... This step just turns on all the
nodes but does not start the network.
2. ./scripts/init\_openthread\_nodes.sh ... This step starts the openthread
network subsystem. You can change this script if you want to use different
config (for e.g., diff panID).

Next Steps for OpenThread:

FAQs:
1. Why does the wireshark not show ipv6 packets?
2. Why do i receive ACK in the stackline for multicast packets?
