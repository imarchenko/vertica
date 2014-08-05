/* Fuzzy String Match User Defined Scalar Function
   Written by Igor Marchenko (imarchen@gmail.com)
   July 31, 2014

   To compile:
   $ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o fuzzymatch.so fuzzymatch.cpp /opt/vertica/sdk/include/Vertica.cpp

   To load into Vertica:
   \set libfile '\''`pwd`'/fuzzymatch.so\'';
   DROP LIBRARY FuzzyMatch CASCADE;
   CREATE LIBRARY FuzzyMatch AS :libfile;
   CREATE FUNCTION fuzzy_match AS LANGUAGE 'C++' NAME 'FuzzyMatchFactory' LIBRARY FuzzyMatch FENCED;

   To unload from Vertica:
   DROP LIBRARY FuzzyMatch CASCADE;

   To use:
   SELECT fuzzy_match(<varchar column>, <varchar column> [USING PARAMETERS [ignore_case = true|false], [ignore_space = true|false], [ignore_special = true|false]], [ignore_digits = true|false]) FROM <table>;
   The optional parameters:
    ignore_case [true|false] - this will perform a case-insensitive comparison of the two input strings
    ignore_space [true|false] - this will ignore all whitespace characters ( \t\n\v\f\r) when comparing the two input strings
    ignore_special [true|false] - this will ignore all non-alphanumeric or whitespace characters ( \t\n\v\f\r) when comapring the two input strings
    ignore_digits [true|false] - this will ignore all digits [0-9] when comparing the two input strings

   This function will return a percentage of how close the two strings are

   Example:
   SELECT fuzzy_match('The brown dog jumped over the green fence!', 'the green dog jumped over the 1 brown fence' USING PARAMETERS ignore_case = true, ignore_space = true, ignore_special = true, ignore_digits = true);
    This should return that these two strings are 82% the same

 */

#include "Vertica.h"
#include <algorithm>
#include <string>
#include <cctype>
#include <vector>

using namespace std;
using namespace Vertica;

// The max size of a VARCHAR in Vertica
#define VARCHAR_SIZE 65000

// Code copied from http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

// Returns if a character is special (any character that is not alphanumeric or whitespace character)
bool isspecial(char c) {
    return !isalnum(c) && !isspace(c);
}

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class FuzzyMatch : public ScalarFunction
{    
    // Code copied from http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
    private:
    int levenshtein(const char *s1, const char *s2)
    {
        unsigned int s1len, s2len, x, y, lastdiag, olddiag;
        s1len = strlen(s1);
        s2len = strlen(s2);
        unsigned int column[s1len+1];
        for (y = 1; y <= s1len; y++)
            column[y] = y;
        for (x = 1; x <= s2len; x++) {
            column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }

    return(column[s1len]);
}

    public:
    virtual void processBlock(ServerInterface &srvInterface, BlockReader &arg_reader, BlockWriter &res_writer)
    {
        do {
            unsigned int distanceMatch;
            float percentMatch;
            bool ignore_case = false;
            bool ignore_space = false;
            bool ignore_special = false;
            bool ignore_digits = false;
            
            ParamReader paramReader = srvInterface.getParamReader();
            if (paramReader.containsParameter("ignore_space"))
                ignore_space = paramReader.getBoolRef("ignore_space");

            if (paramReader.containsParameter("ignore_case"))
                ignore_case = paramReader.getBoolRef("ignore_case");

            if (paramReader.containsParameter("ignore_special"))
                ignore_special = paramReader.getBoolRef("ignore_special");
            
            if (paramReader.containsParameter("ignore_digits"))
                ignore_digits = paramReader.getBoolRef("ignore_digits");
            
            // Grab strings data from Vertica
            string str1 = arg_reader.getStringRef(0).str();
            string str2 = arg_reader.getStringRef(1).str();

            // Tokenize the strings for some pre-processing (if needed)
            vector<string> str1words;
            vector<string> str2words;
                        
            if (ignore_space) {
                str1.erase(remove_if(str1.begin(), str1.end(), (int(*)(int))isspace), str1.end());
                str2.erase(remove_if(str2.begin(), str2.end(), (int(*)(int))isspace), str2.end());
            }
            if (ignore_digits) {
                str1.erase(remove_if(str1.begin(), str1.end(), (int(*)(int))isdigit), str1.end());
                str2.erase(remove_if(str2.begin(), str2.end(), (int(*)(int))isdigit), str2.end());
            }
            if (ignore_special) {
                str1.erase(remove_if(str1.begin(), str1.end(), isspecial), str1.end());
                str2.erase(remove_if(str2.begin(), str2.end(), isspecial), str2.end());
            }
            if (ignore_case) {
                transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
                transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
            }

            distanceMatch = levenshtein(str1.c_str(), str2.c_str());
            percentMatch = (1 - (distanceMatch / (float)max(str1.length(), str2.length()))) * 100;

            res_writer.setFloat(percentMatch);
            res_writer.next();
        } while (arg_reader.next());
  }
};

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class FuzzyMatchFactory : public ScalarFunctionFactory
{
	virtual ScalarFunction *createScalarFunction(ServerInterface &interface)
    {
    	return vt_createFuncObj(interface.allocator, FuzzyMatch);
    }

	virtual void getPrototype(ServerInterface &interface, ColumnTypes &argTypes, ColumnTypes &returnType)
    {
    	argTypes.addVarchar();
        argTypes.addVarchar();
    	returnType.addFloat();
    }

    virtual void getReturnType(ServerInterface &srvInterface, const SizedColumnTypes &argTypes, SizedColumnTypes &returnType)
    {
    	returnType.addFloat();
    }
    
    virtual void getParameterType(ServerInterface &srvInterface, SizedColumnTypes &parameterTypes)
    {
        parameterTypes.addBool("ignore_space");
        parameterTypes.addBool("ignore_case");
        parameterTypes.addBool("ignore_special");
        parameterTypes.addBool("ignore_digits");
    }
};

RegisterFactory(FuzzyMatchFactory);
