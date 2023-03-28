once CAM structure is modified, files need to be rebuild.
To do so, navigate to the out directory of the CAM folder, and execute
$ asn1c -fcompound-names -fincludes-quoted -no-gen-example  ../v1.3.2/CAM.asn ../../CDD_TS102894-2/v1.2.1/ITS-Container.asn 

the new generated files are located in /out. Copy the new files to the vanetza directory of artery.

If any new source files were generated, they need to be added in the asn1c_its_sources.txt file. It can be found in artery/extern/vanetza/vanetza/asn1
