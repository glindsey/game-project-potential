#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "01";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2013";
	static const char UBUNTU_VERSION_STYLE[] = "13.08";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 9;
	static const long BUILD = 948;
	static const long REVISION = 5213;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1142;
	#define RC_FILEVERSION 1,9,948,5213
	#define RC_FILEVERSION_STRING "1, 9, 948, 5213\0"
	static const char FULLVERSION_STRING[] = "1.9.948.5213";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 48;
	

}
#endif //VERSION_H
