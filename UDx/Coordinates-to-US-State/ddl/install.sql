/*****************************
 * Vertica Analytic Database
 *
 * Longitude and Latitude to US State Conversion UDSF
 *
 * Written by Igor Marchenko (HP Vertica) - igor.a.marchenko@hp.com
 *****************************/
-- Installation script: defined the shared library and the appropriate entry points
\set libfile '\''`pwd`'/lib/GeoState.R\'';

CREATE LIBRARY GeoStateLib AS :libfile LANGUAGE 'R';

CREATE FUNCTION geostate AS LANGUAGE 'R' NAME 'GeoStateFactory' LIBRARY GeoStateLib;
