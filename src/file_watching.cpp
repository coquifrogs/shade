#include "file_watching.h"

namespace fwatch {

#if defined(_WIN32)

bool checkFileModified(const char* path, Timestamp* lastTime) { return false; }

#else

bool checkFileModified(const char* path, Timestamp* lastTime) {
	struct stat st;
	if(stat(path, &st) == 0) {
		if(st.st_mtime > *lastTime) {
			*lastTime = st.st_mtime;
			return true;
		}
	}
	return false;
}

#endif

};