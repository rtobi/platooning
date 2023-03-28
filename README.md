# Platooning

This repo presents a **starting point for further development** in the field of advanced platooning functions.
It follows decentralized, dynamic, cooperative and feudalistic platooning concepts presented and described in [1], [2], and [3].


The code in this repository is **not executable** out of the box: dependencies from other software packages are needed.
These are artery [5], vanetza [6], simulte [7], and SUMO (Simulation of Urban Mobility) [8]. Follow instructions in [4] and [5] to install required packages.


# File structure

- artery: services are implemented according to [4].
    - application
        - platooning
            - PlatooningService: decentralized platooning service including *form* and join*. Further maneuvers may be added dissolve, merge, split, change lane, leave.
            - FeudalisticPlatooningService: feudalistic platooning service running on top of PlatooningService.
            - Message definitions (*.msg) of PlatoonMsg, ReadyToPlatoonMsg, and FeudalMsg.  
        - video_recorder: generates screenshots of the SUMO-GUI. On this basis a video can be generated. Follow instructions in the specific folder.
    - symbiosis 
        - LTED2DInterface: send/receives ITS messages, but also custom messages, that are usually transceived using 802.11p (e.g., ITS services), via LTE (both D2D and legacy communication are possible). Extending signals allows to use further messages and services.
        - CentralDataStorage: data structure to store and access platoon information.
        - FeudalisticPlatooningCentral: server implementation for a feudalistic platoon.
        - Car: example implementation of a vehicle supporting both LTE and 802.11p.
        - World_singleCell: example implementation of an environment consisting of one single cell (one eNodeB). 
- misc
    - colorset: color set for painting vehicles according to their role (affects only SUMO-GUI).
    - spline: spline library for maneuver planning [9].
- opp_messages_prebuild
    - generated header and source files for direct usage of custom messages.
- sceanrios
    - roads: examplary roads using simple setup (egg-shaped road, one eNodeB) and more advanced setup (Brennerpass between Austria/Italy, 3 eNodeBs).
    - Examplary configuration files for networks, services and sensors.
- vanetza
    - ITS message definitions (asn files) and prebuild header and source files. Cooperative message is extended by PathFuture [2].


[1] T. Renzler, M. Stolz and D. Watzenig, "Decentralized Dynamic Platooning Architecture with V2V Communication Tested in Omnet++," 2019 IEEE International Conference on Connected Vehicles and Expo (ICCVE), Graz, Austria, 2019, pp. 1-6, doi: 10.1109/ICCVE45908.2019.8965224. <br>
    PDF available at https://tobiasrenzler.com/documents/decentralized_dynamic_platooning.pdf <br>
[2] T. Renzler, M. Stolz and D. Watzenig, "Looking into the Path Future: Extending CAMs for Cooperative Event Handling," 2020 IEEE 92nd Vehicular Technology Conference (VTC2020-Fall), Victoria, BC, Canada, 2020, pp. 1-5, doi: 10.1109/VTC2020-Fall49728.2020.9348776. <br>
    PDF available at https://tobiasrenzler.com/documents/path_future.pdf <br>
[3] T. Renzler, M. Stolz and D. Watzenig, "Feudalistic Platooning: Subdivide Platoons, Unite Networks, and Conquer Efficiency and Reliability," Sensors 2022, 22, 4484. https://doi.org/10.3390/s22124484 <br>
    PDF available at https://tobiasrenzler.com/documents/feudalistic_platooning.pdf <br>
[4] http://artery.v2x-research.eu/architecture/ <br>
[5] https://github.com/riebl/artery <br>
[6] https://github.com/riebl/vanetza <br>
[7] https://github.com/inet-framework/simulte <br>
[8] https://www.eclipse.org/sumo <br>
[9] https://kluge.in-chemnitz.de/opensource/spline/spline.h <br>
