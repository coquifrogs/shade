#include "file_watching.h"

namespace fwatch {

#if defined(_WIN32)

bool checkFileModified(const char* path, Timestamp* lastTime) { 
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	FILETIME mTime;
	bool modified = false;
	if (hFile != INVALID_HANDLE_VALUE && GetFileTime(hFile, NULL, NULL, &mTime)) {
		if (CompareFileTime(&mTime, lastTime) == 1) {
			*lastTime = mTime;			
			modified = true;
		}
	}
	CloseHandle(hFile);
	return modified;
}

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