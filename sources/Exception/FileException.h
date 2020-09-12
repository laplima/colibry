#ifndef __FILEEXCEPTION_H__
#define __FILEEXCEPTION_H__

#include "Exception.h"

// -------------------------------------------------
// FILE READING EXCEPTION
//   Couldn't read file.
// -------------------------------------------------

namespace colibry {
    
    class FileException : public Exception {
    public:
	
	enum Type  {
	    FILE_NOT_FOUND,
	    WRONG_FORMAT,
	    UNEXPECTED_EOF,
	    MISSING_HEADER
	};
	
	FileException(const Type inType,
		       const std::string& inWhere,
		       const std::string& inFileName,
		       const unsigned long inLineNo=0);	
	virtual ~FileException() throw() {}
	
	virtual const char *filename() const
	{ return mFileName.c_str(); }

	virtual long line() const
	{ return mLineNo; }
	
    private:
	
	std::string mFileName;
	unsigned long mLineNo;
    };

};

#endif
