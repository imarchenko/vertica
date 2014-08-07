-------------------------------
INTRODUCTION
-------------------------------

This package provides a way to check if a string is a valid date in HP Vertica.
This function is useful to check before casting a VARCHAR to DATE type to avoid getting an error from the Vertica SQL parser.

Written by Igor Marchenko - imarchen@gmail.com

-------------------------------
COMPILING
-------------------------------

On any Vertica node, you will need to compile the source code using this syntax:
$ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o lib/isdate.so lib/isdate.cpp /opt/vertica/sdk/include/Vertica.cpp

If you are missing g++, then you will need to install it (gcc-c++ package for yum-based systems)

-------------------------------
INSTALLING / UNINSTALLING
-------------------------------

Then, on any Vertica node, the package can be installed as a UDx for the whole Vertica cluster.
$ vsql -f ddl/install.sql

To uninstall,
$ vsql -f ddl/uninstall.sql

-------------------------------
USAGE
-------------------------------

SELECT isdate(VARCHAR);

This function will return a BOOLEAN value, determing if the the string is a valid date


For example,
SELECT isdate('test');
 isdate 
--------
 f
   
SELECT isdate('August 6, 2014');
 isdate 
--------
 t

SELECT isdate('8-6-2014');
 isdate 
--------
 t
