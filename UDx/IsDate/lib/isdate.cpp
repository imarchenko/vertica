/* IsDate User Defined Scalar Function
   Written by Igor Marchenko (imarchen@gmail.com)
   August 6, 2014

   To compile (on any Vertica node):
   $ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o isdate.so isdate.cpp /opt/vertica/sdk/include/Vertica.cpp

   To load into Vertica:
   \set libfile '\''`pwd`'/isdate.so\'';
   DROP LIBRARY IsDateLib CASCADE;
   CREATE LIBRARY IsDateLib AS :libfile;
   CREATE FUNCTION ISDATE AS LANGUAGE 'C++' NAME 'IsDateFactory' LIBRARY IsDateLib FENCED;

   To unload from Vertica:
   DROP LIBRARY IsDateLib CASCADE;

   To use:
   SELECT isdate(VARCHAR);

   This function will return a BOOLEAN value, determing if the the string is a valid date

   Example:
   SELECT isdate('test');
    isdate 
   --------
    f
   
   SELECT isdate('August 6, 2014');
    isdate 
   --------
    t

   SELECT isdate('8-6-2014');
    isdate 
   --------
    t
 */

#include "Vertica.h"

using namespace std;
using namespace Vertica;

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class IsDate : public ScalarFunction
{
public:
    virtual void processBlock(ServerInterface &srvInterface, BlockReader &arg_reader, BlockWriter &res_writer)
    {
        do {
            // Grab string data from Vertica and try converting to DateADT type
            DateADT d = dateIn(arg_reader.getStringRef(0).str().c_str(), 0);

            res_writer.setBool(d != vint_null);
            res_writer.next();
        } while (arg_reader.next());
    }
};

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class IsDateFactory : public ScalarFunctionFactory
{
    virtual ScalarFunction *createScalarFunction(ServerInterface &interface)
    {
        return vt_createFuncObj(interface.allocator, IsDate);
    }

    virtual void getPrototype(ServerInterface &interface, ColumnTypes &argTypes, ColumnTypes &returnType)
    {
        argTypes.addVarchar();
        returnType.addBool();
    }

    virtual void getReturnType(ServerInterface &srvInterface, const SizedColumnTypes &argTypes, SizedColumnTypes &returnType)
    {
        returnType.addBool();
    }
};

RegisterFactory(IsDateFactory);
