# TrialExerciseGT
This repository is for my trial excersise for Guardtime research engineer position.

## Compilation

Before compiling one should refer to the Makefile and check that the compiler used (currently GCC) is compatible with their system. Also, the code is dependent on OpenSSL library (https://www.openssl.org/). If its installation directory is not in the path, then one should change line 27 from 

OpenSSL = -lssl -lcrypto 

to 

OpenSSL = -lssl -lcrypto -L<path\_to\_OpenSSL>

For compilation just call type \ct{make} on the command line. The resulting executable will be place into directory \ct{built}. At the moment the code has only been tested on Linux. It should work without issues on a Mac. I'm not sure about Windows machines.  
