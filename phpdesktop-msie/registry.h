// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <Winreg.h>

bool CreateRegistryKey(HKEY hKeyRoot, const wchar_t* path);
bool CreateRegistryString(HKEY hKeyRoot, const wchar_t* path, 
                                 const wchar_t* stringName, 
                                 const wchar_t* stringValue);
