///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name : 
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
///Copyright (C) 2012 Cedric Guillemet
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
///so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 


#ifndef LIBBASE_H__
#define LIBBASE_H__

#include "ZBaseDefs.h"
#include "ZBaseMaths.h"
#include "ZCollisionsUtils.h"
#include "ZMathsFunc.h"

// platform
/****************************************/
#if defined(_WIN32)
	// Windows (32-bit and 64-bit)
	#define GIZMO_PLATFORM_WINDOWS 1
	#ifdef _WIN64
		// Windows (64-bit only)
		#define GIZMO_PLATFORM_WINDOWS_64 1
	#endif
#elif defined(__APPLE__)
	#define GIZMO_PLATFORM_APPLE 1
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR
		#define GIZMO_PLATFORM_IOS_SIMULATOR 1
	#elif TARGET_OS_IPHONE
		#define GIZMO_PLATFORM_IOS 1
	#elif TARGET_OS_MAC
		#define GIZMO_PLATFORM_MAC_OS 1
	#else
		#error "Unknown Apple platform"
	#endif
#elif defined(ANDROID) || defined(__ANDROID__)
	#define GIZMO_PLATFORM_ANDROID 1
#elif defined(__EMSCRIPTEN__)
	#define GIZMO_PLATFORM_EMSCRIPTEN 1
#elif defined(__linux__)
	#define GIZMO_PLATFORM_LINUX 1
#elif defined(__unix__) // all unices not caught above
	#define GIZMO_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
	#define GIZMO_PLATFORM_POSIX 1
#else
	#error "LibGizmo: unknown platform"
#endif

#ifdef _MSC_VER
	#define GIZMO_MSVC 1
#endif
/****************************************/

#endif

