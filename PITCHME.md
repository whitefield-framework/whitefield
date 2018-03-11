# Whitefield

Simulation Framework for Wireless Sensor Networks

---
## Yet another framework!

- Does real-world protocol stacks (Contiki/RIOT/OpenThread)
- With realistic RF simulation (NS3/..)
- At scale!
- Currently works for 802.15.4 but can be extended for
    - BLE
    - 802.11-OCB, 802.11s
    - 802.15.4 TSCH
- Protocol Design/performance/efficiency
    - Dependency on link layer tech
    - Asymmetric links
    - Transient links
- An example... protocol design with assumption of bidirectional links ...

---

## Our Experience
- Lets talk practical IoT
- We implemented and tested,
    - IETF RFCs, and drafts
    - IEEE papers
- Unrealistic data with Cooja
- Few use NS3, Castalia-Omnet++
    - Requires more effort

---

## experience(contd)
- Performance@scale
    - example, routing/adhoc issues @ scale
- Design@scale
    - example, neighbor mgmt issues
- Most of the plugfests, prototypes are limited to few nodes
- Rarely you see pratical prototypes with 100s/1000s of nodes

---

## experience (contd)
- Security is especially difficult
- Impact of 6lo fragmentation
- Network convergence time@scale

---

## Design
1.
2.
3. Integrated OAM
4. Process design

---
## What does Whitefield have today (Nov 2017)?

---
## How have we used it for?

---
## Screenshots
1. configuration
2. start whitefield
3. monitor network
4. monitor network with canvas
5. OAM - wfshell 
6. OAM - monitor routing table agnostic of the OS stack used
7. pcap
8. logging
9. 
