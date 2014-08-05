-------------------------------
INTRODUCTION
-------------------------------

This package converts geolocation coordinates (longitude, latitude) into a United States state.

Written by Igor Marchenko - imarchen@gmail.com

-------------------------------
INSTALLING / UNINSTALLING
-------------------------------

There are a few dependencies for this package. You can install them using the standard R package installation methods.

** You need to install these packages on all Vertica nodes **

$ sudo /opt/vertica/R/bin/R
> install.packages(c("sp", "maps", "maptools"))

Then, on a single Vertica node, the package can be installed as a UDx for the whole Vertica cluster.
$ vsql -f ddl/install.sql

To uninstall,
$ vsql -f ddl/uninstall.sql

-------------------------------
USAGE
-------------------------------

SELECT geostate(longitude, latitude);

INPUT: NUMERIC values for longitude, latitude
RETURN: VARCHAR representing the US state or NULL if coordinates are outside USA border

For example,
vsql=> SELECT geostate(-71.145948, 42.394590);
 geostate 
----------
 MA
(1 row)
 
