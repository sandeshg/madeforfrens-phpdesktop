// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"

class SingleInstanceApplication {
  protected:
	DWORD lastError;
	HANDLE mutex;
  public:
    SingleInstanceApplication() {}
	void Initialize(TCHAR* mutexname) {
		mutex = CreateMutex(NULL, FALSE, mutexname);
		lastError = GetLastError();
	}
	~SingleInstanceApplication() {
		if (mutex) {
			CloseHandle(mutex);
			mutex = NULL;
		}
	}
	bool IsRunning() {
		return (lastError == ERROR_ALREADY_EXISTS);
	}

};