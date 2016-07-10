# TrialExerciseGT
This repository is for my trial excersise for Guardtime research engineer position.

## Compilation

Before compiling one should refer to the Makefile and check that the compiler used (currently GCC) is compatible with their system. Also, the code is dependent on OpenSSL library (https://www.openssl.org/). If its installation directory is not in the path, then one should change line 27 from 

OpenSSL = -lssl -lcrypto 

to 

OpenSSL = -lssl -lcrypto -L<path\_to\_OpenSSL>

For compilation just call 

make 

The resulting executables will be placed into directory built/. 

At the moment the code has only been tested on Linux. It should work without issues on a Mac. I'm not sure about Windows machines.  

## Running 

For both of executables build/hasher and build/test_hasher the only compulsory argument is the log file name, which can be passed as 

-i <log\_file\_name> 

The executables produce help messages in the expected way (-h or --help). If they are called with just the log file argument, then only root signing is conducted. For hash chain extraction add --chain <file_with_requested_lines> and for storing the leaves --leaves.  

For ease of testing the code output, script run\_test.sh has been added. It calls the test_hasher with numbers.log, storing the signature, leaves and the hash chains for all the lines. 