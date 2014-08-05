/*****************************
 * Vertica Analytic Database
 *
 * Convert any iconv supported charset to UTF-8 for HP Vertica
 * This is useful when loading data from data sources from non-UNIX environments, such as Microsoft IIS log files.
 *
 * Written by Igor Marchenko - imarchen@gmail.com
 *****************************/
-- Installation script: defined the shared library and the appropriate entry points
\set libfile '\''`pwd`'/lib/iconv.so\'';
DROP LIBRARY IConv CASCADE;
CREATE LIBRARY IConv AS :libfile;
CREATE FUNCTION iconv AS LANGUAGE 'C++' NAME 'IConvFactory' LIBRARY IConv FENCED;
