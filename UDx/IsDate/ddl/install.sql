/*****************************
 * Vertica Analytic Database
 *
 * Check if a string is a valid date with HP Vertica.
 *
 * Written by Igor Marchenko - imarchen@gmail.com
 *****************************/

-- Installation script: defined the shared library and the appropriate entry points
\set libfile '\''`pwd`'/lib/isdate.so\'';
DROP LIBRARY IsDateLib CASCADE;
CREATE LIBRARY IsDateLib AS :libfile;
CREATE FUNCTION ISDATE AS LANGUAGE 'C++' NAME 'IsDateFactory' LIBRARY IsDateLib FENCED;
