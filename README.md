# DESIGNING, SIMULATING AND EVALUATING A NEW CONGESTION AVOIDANCE PROTOCOL FOR VANETS
## Abstract
The Vehicular Ad-hoc Network (VANET) is a peer-to-peer network of vehicles and
roadside infrastructure over wireless channels. On a VANET, the vehicles co-operate
to create co-operative awareness of the local and global traffic conditions based on
the information received from other vehicles to enhance safety and navigation. Thus,
by enhancing the accuracy of information, dynamically setting the priority of the
information based on vehicular conditions, a VANET protocol can be created to suite
various traffic safety requirements such as congestion and collision avoidance.

In this thesis, a novel approach is designed and implemented to perform real-time
congestion avoidance using a VANET. In this approach, to achieve their goal, the
vehicles exchange congestion information messages (CIM) along with the VANET’s
standard IEEE 802.11p based beacon messages. The CIMs help the vehicles to dynamically 
choose the next best possible road based on the average speed of vehicles
on all roads the vehicle can turn into next. The beacon messages help the vehicles
to develop a collective awareness such that the vehicles in the same network can take
preemptive actions to avoid congestion, collisions as well as increase pedestrian safety.

The proposed approach is evaluated using Veins, an open-source framework for performing 
vehicular simulations that combines SUMO and OMNET++ simulators for
vehicular and network traffic respectively. The effectiveness of the proposed congestion 
avoidance protocol has been compared against (i) a similar congestion avoidance
protocol proposed by M. Turker et al., (ii) a built-in congestion avoidance protocol
bundled with the Veins framework, and (iii) without any protocol, where the vehicles
follow a predefined shortest path algorithm such as Dijkstra or A*. The results show
the promising performance of the proposed approach.
