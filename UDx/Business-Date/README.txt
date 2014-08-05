-------------------------------
INTRODUCTION
-------------------------------

This package determines if a date is a United States business date (based on New York Stock Exchange holidays)

Written by Igor Marchenko - imarchen@gmail.com

-------------------------------
INSTALLING / UNINSTALLING
-------------------------------

There is an R dependency for this package. You can install it using the standard R package installation methods.

** You need to install these packages on all Vertica nodes **

$ sudo /opt/vertica/R/bin/R
R> install.packages("timeDate");
R> quit("no");

Then, on a single Vertica node, the package can be installed as a UDx for the whole Vertica cluster.
$ vsql -f ddl/install.sql

To uninstall,
$ vsql -f ddl/uninstall.sql

-------------------------------
USAGE
-------------------------------

SELECT is_business_date(<date>);

INPUT: DATE value to check if it is a business day
RETURN: BOOLEAN value (true if it is a business day, false otherwise)

For example,
SELECT is_business_date(NOW()::DATE);
 is_business_date 
------------------
 t
(1 row)

SELECT is_business_date('8-1-2014'::DATE);
 is_business_date 
------------------
 t
(1 row)

SELECT is_business_date('8-2-2014'::DATE);
 is_business_date 
------------------
 f
(1 row)


SELECT count_business_dates(<start date>, <end date>);
INPUT: Two DATE values to count the number of business days between the range of dates
RETURN: INTEGER value for the count

For example,
SELECT count_business_dates('5-1-2013'::DATE, '5-31-2013'::DATE);
 count_business_dates 
----------------------
                   23
(1 row)

SELECT count_business_dates('1-1-2013'::DATE, '12-31-2013'::DATE);
 count_business_dates 
----------------------
                  261
(1 row)
