/*****************************
 * Vertica Analytic Database
 *
 * Convert any iconv supported charset to UTF-8 for HP Vertica
 * This is useful when loading data from data sources from non-UNIX environments, such as Microsoft IIS log files.
 *
 * Written by Igor Marchenko - imarchen@gmail.com
 *****************************/

-- Uninstallation script
DROP LIBRARY IConv CASCADE;
