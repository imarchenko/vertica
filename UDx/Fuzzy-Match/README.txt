-------------------------------
INTRODUCTION
-------------------------------

This package allows fuzzy string matching with HP Vertica.
It uses the Levenshtein distance algorithm to calculate how many character edits two strings are away from each other.

Written by Igor Marchenko - imarchen@gmail.com

-------------------------------
COMPILING
-------------------------------

On any Vertica node, you will need to compile the source code using this syntax:
$ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o lib/fuzzymatch.so lib/fuzzymatch.cpp /opt/vertica/sdk/include/Vertica.cpp

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

SELECT fuzzy_match(<varchar column>, <varchar column> [USING PARAMETERS [ignore_case = true|false], [ignore_space = true|false], [ignore_special = true|false]], [ignore_digits = true|false]) FROM <table>;
   The optional parameters:
    ignore_case [true|false] - this will perform a case-insensitive comparison of the two input strings
    ignore_space [true|false] - this will ignore all whitespace characters ( \t\n\v\f\r) when comparing the two input strings
    ignore_special [true|false] - this will ignore all non-alphanumeric or whitespace characters ( \t\n\v\f\r) when comapring the two input strings
    ignore_digits [true|false] - this will ignore all digits [0-9] when comparing the two input strings

   This function will return a percentage of how close the two strings are


For example, these two strings are 82% similar:
SELECT fuzzy_match('The brown dog jumped over the green fence!', 'the green dog jumped over the 1 brown fence' USING PARAMETERS ignore_case = true, ignore_space = true, ignore_special = true, ignore_digits = true);
   fuzzy_match    
------------------
 82.3529434204102
(1 row)
