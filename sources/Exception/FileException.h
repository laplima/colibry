#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H

#include "Exception.h"

// -------------------------------------------------
// FILE READING EXCEPTION
//   Couldn't read file.
// -------------------------------------------------

namespace colibry {
    
    class FileException : public Exception {
    public:
	
		enum Type {
		    FILE_NOT_FOUND,
		    WRONG_FORMAT,
		    UNEXPECTED_EOF,
		    MISSING_HEADER
		};
	
		FileException(Type inType,
			std::string inWhere,
			std::string inFileName,
			unsigned long inLineNo=0);	
	
		[[nodiscard]] virtual const char *filename() const
			{ return mFileName.c_str(); }

		[[nodiscard]] virtual long line() const { return mLineNo; }
	
    private:
	
		std::string mFileName;
		unsigned long mLineNo;
    };

};

#endif
