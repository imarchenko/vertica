/*****************************
 * Vertica Analytic Database
 *
 * Perform fuzzy string matching with HP Vertica.
 *
 * Written by Igor Marchenko - imarchen@gmail.com
 *****************************/

-- Installation script: defined the shared library and the appropriate entry points
\set libfile '\''`pwd`'/lib/fuzzymatch.so\'';
DROP LIBRARY FuzzyMatch CASCADE;
CREATE LIBRARY FuzzyMatch AS :libfile;
CREATE FUNCTION fuzzy_match AS LANGUAGE 'C++' NAME 'FuzzyMatchFactory' LIBRARY FuzzyMatch FENCED;
