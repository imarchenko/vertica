/*****************************
 * Vertica Analytic Database
 *
 * Business Date UDSF
 *
 * Written by Igor Marchenko - imarchen@gmail.com
 *****************************/

-- Installation script: defined the shared library and the appropriate entry points
\set libfile '\''`pwd`'/lib/BusinessDate.R\''
DROP LIBRARY BusinessDateLib CASCADE;
CREATE LIBRARY BusinessDateLib AS :libfile LANGUAGE 'R';
CREATE FUNCTION is_business_date AS LANGUAGE 'R' NAME 'is_business_date_factory' LIBRARY BusinessDateLib;
CREATE FUNCTION count_business_dates AS LANGUAGE 'R' NAME 'count_business_dates_factory' LIBRARY BusinessDateLib;
