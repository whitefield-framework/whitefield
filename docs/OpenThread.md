## OpenThread-Whitefield Integration Aim

Validate OpenThread claims in realistic 802.15.4 environment possibly in
standalone mode i.e., without having to worry about third-party OS quirks.

Using Whitefield it should be possible to test following:
1. OpenThread scalability 
    * max routers per BR
    * max child nodes per routers
    * max BRs
    * max hops
2. Openthread performance under realistic conditions
    * Network convergence time
    * Node join time
    * packet delivery rate
    * Control Overhead incurred under different scenarios
    * Repair time (involving router failure, leader failure, BR failure)
    * Reboot handling of Router/Leader/BR
3. Possible to simulate following scenarios
    * High node density scenario
    * Variable data-rate per node
    * Impact of node mobility
    * With and without memory constraints
    * Battery performance

## OpenThread Integration Option

### Integrate by adding new platform in core OpenThread

Adding a new platform of whitefield in OpenThread just like cc2538 or posix
(already present in OpenThread). This option allows to directly use core
OpenThread in Whitefield.

Advantage: Can test OpenThread in standalone mode without any dependency on
another OS.

Disadvantage: In real world, the OpenThread core would be used along with some
another OS/platform (such as [Zephyr]/[RIOT]/[FreeRTOS]). Only core-openthread
could be tested with such integration mode.

This mode of integration best satisfies the aim mentioned above and this is the
mode targeted first.

### Integrate using existing OpenThread supported OS ([RIOT]/[Zephyr]/[FreeRTOS])

Advantage: Get existing OpenThread integration support in existing popular IoT
OS.

Disadvantage: Have to follow the quirks of the IoT OS used. If the aim is to
validate OpenThread then this is unnecessary albeit counter-productive.

This should have been the simplest mode of integration but it means adding
Whitefield as a platform/driver/target in [RIOT]/[Zephyr]/[FreeRTOS]. Currently
Whitefield integrates with [RIOT] as a separate package and not as a
platform/driver (just like OpenThread), and thus cannot use OpenThread (because
two packages cannot be used together). [Zephyr] is not supported in Whitefield
at the moment.

Whitefield works as a separate target currently only in [Contiki].
Unfortunately, Contiki does not support OpenThread integration at the moment.

Another option is to try [OT-RTOS] which is based on [FreeRTOS], but it also
requires to add platform support in [FreeRTOS].

### Integrate by externally attaching to Posix platform

This is the least intrusive mode of integration in OpenThread i.e., use
existing posix platform and override radio transmit/receive (send/recv) with
Whitefield transmit/receive using preloading techniques such as LD\_PRELOAD, or
GCC -wrap option.

Advantage: Can readily use any OpenThread binary directly in Whitefield.

Disadvantage: The preloaded lib needs to be in sync with OpenThread APIs. This
is difficult to enforce with this mode of integration.

## OpenThread TODOs
1. Prepare call flows for documentation. This may help future drivers writing
   new porting interfaces.
2. How is OpenThread-Whitefield different from OpenThread-Posix?
    a. ACKs are handled in Whitefield. (This is yet to be handled)

## Current Stage for OpenThread Integration:
Have created a separate platform for whitefield in OpenThread. Commline
init/deinit is handled but the actual commline send/recv is not handled as of
now. In radioTransmit/Receive the posix-UDP send/recv is done to tx/rx
messages. Using this I have verified that the Leader and Router node work fine.
Steps to use the OpenThread integration:
1. ./invoke\_whitefield.sh config/wf\_ot.cfg ... This step just turns on all
   the nodes but does not start the network.
2. ./scripts/init\_openthread\_nodes.sh ... This step starts the openthread
   network subsystem. You can change this script if you want to use different
   config (for e.g., diff panID).

## FAQs:

[FreeRTOS]: https://freertos.org/
[RIOT]: https://www.riot-os.org/
[Zephyr]: https://www.zephyrproject.org/
[Contiki]: http://www.contiki-os.org/
[OT-RTOS]: https://openthread.io/platforms/ot-rtos

