# Platooning

This repo presents ways of platoon management (form, dissolve, join, merge, split, change lane, leave).

Note that the repository represents a **starting point for further development**. The code in this repository is **not executable** out of the box: dependencies from other software packages are needed.

# Files

In folder *decentralized* you find the implementation of the decentralized, dynamic platooning algorithm presented in [1] and available at [2].
- Service [3] implementation: platooning service includes *form* and *join*.
- Message definitions (*.msg) of PlatoonMsg and ReadyToPlatoonMsg including generated header and source files for direct usage.
- Additional dependencies can be found here [4].

[1] Renzler, Tobias, Michael Stolz, and Daniel Watzenig. "Decentralized dynamic platooning architecture with v2v communication tested in omnet++." _2019 IEEE International Conference on Connected Vehicles and Expo (ICCVE)_. IEEE, 2019. <br>
[2] https://www.tobiasrenzler.com/documents/decentralized_dynamic_platooning.pdf <br>
[3] http://artery.v2x-research.eu/architecture/ <br>
[4] https://github.com/riebl/artery <br>