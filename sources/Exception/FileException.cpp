#include "FileException.h"

using namespace std;
using namespace colibry;

FileException::FileException(const Type inType,
			     const string& inWhere,
			     const string& inFileName,
			     const unsigned long inLineNo)
    : Exception(inType,inWhere), mFileName(inFileName), mLineNo(inLineNo)
{
    switch (mType) {
    case FILE_NOT_FOUND:
	mWhat = "File not found.";
	break;
    case WRONG_FORMAT:
	mWhat = "Wrong format.";
	break;
    case UNEXPECTED_EOF:
	mWhat = "Unexpected end of file.";
	break;
    case MISSING_HEADER:
	mWhat = "Missing header.";
	break;
    default:
	mWhat = "Unknown error.";
    }
}
