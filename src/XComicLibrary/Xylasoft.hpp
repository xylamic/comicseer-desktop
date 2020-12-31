/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef XYLASOFT_HPP
#define XYLASOFT_HPP

#include <string>
#include <cstdlib>
#include <cstdint>
#include <algorithm>

// enable memory debugging in debug on window
#ifdef _WIN32
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#ifdef _WIN32
    #define WINVER 0x0502 // Windows XP SP2+
#endif

#endif // XYLASOFT_H
