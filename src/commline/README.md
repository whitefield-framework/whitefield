# CommLine

Commline provides interfaces for Airline and Stackline to send/recv messages. Messages could be actual IPv6/IPv4/6lo packets or they could be control packets used for OAM. Commline currently uses sysv msgqs for sending/recving messages. This can be changed to support anything else in the future.

The decoupling of Commline allows Whitefield to be more flexible. For e.g. in the future the Airline could be running on one machine while the stacklines could be executed on set of other machines!! Secondly, supporting TUN/TAP interfaces with Stacklines in the future will be made easy because of this single point of entry/exit for message.

### Choice of Sys V msgq
One particular feature of sysv msgqs which is utilized by whitefield is that one can send a packet with a given 'mtype' and there could be multiple processes waiting on the same msgq but only the process requesting the given 'mtype' will receive the msg. In case of Whitefield, there are multiple Stackline processes who are waiting on the same message queue. The Airline sends a message with mtype=nodeid and the stackline responsible for that nodeid receives the messages.

