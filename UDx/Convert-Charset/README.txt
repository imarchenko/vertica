-------------------------------
INTRODUCTION
-------------------------------

This package converts any iconv supported character set to UTF-8 for HP Vertica.
This type of conversion is required when loading strings from non-UNIX systems, in some cases.
An example of this is when loading Microsoft IIS web logs and special characters do not convert to UTF-8 properly.

All supported encoding character sets are available by running this iconv shell program
$ iconv --list

Written by Igor Marchenko - imarchen@gmail.com

-------------------------------
COMPILING
-------------------------------

On any Vertica node, you will need to compile the source code using this syntax:
$ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o lib/iconv.so lib/iconv.cpp /opt/vertica/sdk/include/Vertica.cpp

If you are missing g++, then you will need to install it (gcc-c++ package for yum-based systems)

-------------------------------
INSTALLING / UNINSTALLING
-------------------------------

Make sure you have iconv.h installed on *all* of your Vertica nodes. It is part of the glibc-common package on yum-based systems.

Then, on any Vertica node, the package can be installed as a UDx for the whole Vertica cluster.
$ vsql -f ddl/install.sql

To uninstall,
$ vsql -f ddl/uninstall.sql

-------------------------------
USAGE
-------------------------------

SELECT iconv(<varchar column> USING PARAMETERS charset='<encoding charset to convert from>') FROM <table>;

INPUT: A VARCHAR to convert
PARAMETERS: charset can be obtained from the shell program 'iconv --list'
RETURN: A VARCHAR converted to the UTF-8 charset

For example,
If we take the URL string "http%3A%2F%2Fwww.example.com%2Fsearch%3Fq%3DFather%92s-Day%26uuid%3D012346789ABCDEF", there is a special left-apostrophe
character between Father's in the URL (identified by %92). If we run this through uri_percent_decode without iconv, we get a question-mark for that
character.

SELECT uri_percent_decode('http%3A%2F%2Fwww.example.com%2Fsearch%3Fq%3DFather%92s-Day%26uuid%3D012346789ABCDEF');
                        uri_percent_decode                         
-------------------------------------------------------------------
 http://www.example.com/search?q=Father?s-Day&uuid=012346789ABCDEF
(1 row)

By using iconv, the character is properly converted.
SELECT iconv(uri_percent_decode('http%3A%2F%2Fwww.example.com%2Fsearch%3Fq%3DFather%92s-Day%26uuid%3D012346789ABCDEF') USING PARAMETERS charset='WINDOWS-1252');
                               iconv                               
-------------------------------------------------------------------
 http://www.example.com/search?q=Father’s-Day&uuid=012346789ABCDEF
(1 row)
