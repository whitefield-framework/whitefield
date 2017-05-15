## Changes to Contiki
1. Added a new 'platform' called 'whitefield' (code in contiki/platform/whitefield)
2. whitefield platform overrides mac/radio layer to send the packets to Airline using Commline interfaces
3. Zero changes to anything else in Contiki!!
4. Any Contiki application can be compiled with whitefield platform using `make TARGET=whitefield` and
the generated app bin path can be specified in the whitefield config (wf.cfg)

#### Managing OAM with Contiki
Contiki is designed to be single threaded i.e. all the modules in the Contiki access global variables and other shared resources without any locking/unlocking worries. Whitefield introduces OAM in Contiki in a separate thread which is Whitefield platform specific. Thus now there is a problem of accessing Contiki module's state in a separate thread. But fortunately whitefield controls the initial access/poll to the contiki thread. Thus a single mutex is used to control access to the Contiki state i.e. either OAM or the contiki native thread will be accessing the shared resources.

## Changes to NS3
1. NS3 supports 802.15.4-2006 MAC layer with MAC layer ACK. 
For ETX metric calculation in routing protocol Contiki requires the number of retries done in CSMA along with ACK_OK status. NS3 didn't support this number of retries in McpsDataConfirmIndication callback. Added it and impacted files (src/lr-wpan/model/lr-wpan-mac.[cc & h]).
