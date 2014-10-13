/* Convert any Supported iconv Character Encoding to UTF-8 Charset Encoding User Defined Scalar Function
   Written by Igor Marchenko (imarchen@gmail.com)
   March 12, 2014

   * All supported encoding character sets are available by running this iconv shell program
   $ iconv --list

   To compile:
   $ g++ -I /opt/vertica/sdk/include -I HelperLibraries -g -Wall -Wno-unused-value -shared -fPIC -O3 -o iconv.so iconv.cpp /opt/vertica/sdk/include/Vertica.cpp

   To load into Vertica:
   \set libfile '\''`pwd`'/iconv.so\'';
   DROP LIBRARY IConv CASCADE;
   CREATE LIBRARY IConv AS :libfile;
   CREATE FUNCTION iconv AS LANGUAGE 'C++' NAME 'IConvFactory' LIBRARY IConv FENCED;

   To unload from Vertica:
   DROP LIBRARY IConv CASCADE;

   To use:
   SELECT iconv(<varchar column> USING PARAMETERS charset='<encoding charset to convert from>') FROM <table>;

   Example:
   SELECT iconv(uri_percent_decode('http%3A%2F%2Fwww.example.com%2Fsearch%3Fq%3DFather%92s-Day%26uuid%3D012346789ABCDEF') USING PARAMETERS charset='WINDOWS-1252')
 */

#include "Vertica.h"
#include <string>
#include <iconv.h>

using namespace std;
using namespace Vertica;

// The max size of a VARCHAR in Vertica
#define VARCHAR_SIZE 65000

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class IConv : public ScalarFunction
{
	public:
    virtual void processBlock(ServerInterface &srvInterface, BlockReader &arg_reader, BlockWriter &res_writer)
    {
        do {
			// Define default charset encoding formats
			string encodeToCharset = "UTF-8//IGNORE";
			string encodeFromCharset = "WINDOWS-1252";
			
			ParamReader paramReader = srvInterface.getParamReader();
			if (paramReader.containsParameter("charset"))
				encodeFromCharset = paramReader.getStringRef("charset").str();

			// Grab data from Vertica row (column 0)
	        char inbuf[VARCHAR_SIZE];
			// Better way to do this without making so many copies?
			strcpy(inbuf, arg_reader.getStringRef(0).str().c_str());
			
			char outbuf[VARCHAR_SIZE];
			char *inptr = inbuf;
	        char *wrptr = outbuf;
	        size_t insize = VARCHAR_SIZE - 1;
	        size_t outsize = VARCHAR_SIZE - 1;

			// Convert the encoding charset
	        iconv_t cd = iconv_open(encodeToCharset.c_str(), encodeFromCharset.c_str());
		    if (cd == (iconv_t) - 1)
		    {
		    	if (errno == EINVAL)
					vt_report_error(0, "Conversion from '%s' to 'UTF-8' is not available. Run 'iconv --list', on your Linux shell, to see all available options.", encodeFromCharset.c_str());
                else
					vt_report_error(0, "Problem with conversion, in iconv_open. (error code: %d)", errno);
		    }

			iconv(cd, &inptr, &insize, &wrptr, &outsize);
			// Error checking might need some work
	        //size_t ret = iconv(cd, &inptr, &insize, &wrptr, &outsize);
			//if (ret == (size_t) - 1) {
			//	if (errno != EINVAL)
			//		vt_report_error(0, "Problem with conversion, in iconv function. (error code: %d)", errno);
			//}

	        if (iconv_close (cd) != 0)
				vt_report_error(0, "Problem with conversion, in iconv_close. (error code: %d)", errno);

			// Save the converted string for output to Vertica
            res_writer.getStringRef().copy(outbuf);
            res_writer.next();
        } while (arg_reader.next());
  }
};

// Standard subclassing of a Vertica scalar function (see SDK documentation)
class IConvFactory : public ScalarFunctionFactory
{
	virtual ScalarFunction *createScalarFunction(ServerInterface &interface)
    {
    	return vt_createFuncObj(interface.allocator, IConv);
    }

	virtual void getPrototype(ServerInterface &interface, ColumnTypes &argTypes, ColumnTypes &returnType)
    {
    	argTypes.addVarchar();
    	returnType.addVarchar();
    }

    virtual void getReturnType(ServerInterface &srvInterface, const SizedColumnTypes &argTypes, SizedColumnTypes &returnType)
    {
    	returnType.addVarchar(VARCHAR_SIZE);
    }
	
    virtual void getParameterType(ServerInterface &srvInterface, SizedColumnTypes &parameterTypes)
    {
        parameterTypes.addVarchar(VARCHAR_SIZE, "charset");
    }
};

RegisterFactory(IConvFactory);
