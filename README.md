RemoteCache
===========

Engine for Memcached - C Shared Library. Uses Dinamic Partitions and Buddy system as algorithms.

The Config.txt must be copied inside memcached-1.6 folder where the executable memcached is.
This engine works with memcached 1.6 or following versions.


In order to use the custom engine, memcached console parameters must be set as follows: 
-E "Path to the engine .so file" 
-n "Chunk size, minimum partition size"
-I "Full cache size"
