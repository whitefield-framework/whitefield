## Whitefield Configuration User Manual

Whitefield configuration provides common configuration options across different airlines and stacklines used 
i.e. the same configuration works whether you use NS3 or Castalia-Omnet++ as Airline or use Contiki/RIOT as 
stackline.

|Key|Value Range|Remarks|
|---|-----------|-------|
|numOfNodes|[2-5000]|Number of nodes in the network|
|fieldX|Uint range|Field length in X direction ... Currently only 2D model is supp|
|fieldY|Uint range|Field length in Y direction|
|topologyType|randrect|Randomly place nodes in area denoted by fieldX * fieldY|
| |grid | Grid topology where nodes are separated by distance specified by fieldX * fieldY and the width of the grid is specified by gridWidth|
|panID|Ushort range| PAN identifier to be used in LOWPAN |
|macPktQlen|<100|Maximum number of packets that can be buffered/queued at MAC layer|
|macMaxRetry|<20|Maximum number of times the mac packet will be retried|
|nodeExec|/path/to/stackline/binary.exec|Usually native compiled executable for Contiki/RIOT nodes path will be specified here|

The configuration can be applied to only a set of nodes by specifying the node index range (note, the first node has an index of zero). 
For e.g.
```
numOfNode=20
nodeExec=/path/to/contiki
nodeExec[10-19]=/path/to/riot
```
In the above configuration the first nodeExec=/path/to/contiki will result in the execuatable getting set for all nodes. 
In the subsequent config statement, nodes[10-19] (inclusive) will override the nodeExec path. 
Note that the sequence of configuration option is important in this particular case.
