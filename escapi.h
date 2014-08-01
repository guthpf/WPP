#pragma once

//Adapted from ESCAPI (http://sol.gfxile.net/escapi/)
/*
    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <http://unlicense.org/>
*/

#ifdef WIN32

/* Extremely Simple Capture API */

struct SimpleCapParams
{
	/* Target buffer. 
	 * Must be at least mWidth * mHeight * sizeof(int) of size! 
	 */
	int * mTargetBuf;
	/* Buffer width */
	int mWidth;
	/* Buffer height */
	int mHeight;
};


/* Sets up the ESCAPI DLL and the function pointers below. Call this first! */
/* Returns number of capture devices found (same as countCaptureDevices, below) */
extern int setupESCAPI();

/* return the number of capture devices found */
typedef int (*countCaptureDevicesProc)();
extern countCaptureDevicesProc countCaptureDevices;

/* initCapture tries to open the video capture device. 
 * Returns 0 on failure, 1 on success. 
 * Note: Capture parameter values must not change while capture device
 *       is in use (i.e. between initCapture and deinitCapture).
 *       Do *not* free the target buffer, or change its pointer!
 */
typedef int (*initCaptureProc)(unsigned int deviceno, struct SimpleCapParams *aParams);
extern initCaptureProc initCapture;

/* deinitCapture closes the video capture device. */
typedef void (*deinitCaptureProc)(unsigned int deviceno);
extern deinitCaptureProc deinitCapture;

/* doCapture requests video frame to be captured. */
typedef void (*doCaptureProc)(unsigned int deviceno);
extern doCaptureProc doCapture;

/* isCaptureDone returns 1 when the requested frame has been captured.*/
typedef int (*isCaptureDoneProc)(unsigned int deviceno);
extern isCaptureDoneProc isCaptureDone;

/* Get the user-friendly name of a capture device. */
typedef void (*getCaptureDeviceNameProc)(unsigned int deviceno, char *namebuffer, int bufferlength);
extern getCaptureDeviceNameProc getCaptureDeviceName;

/* Returns the ESCAPI DLL version. 0x200 for 2.0 */
typedef int (*ESCAPIDLLVersionProc)();
extern ESCAPIDLLVersionProc ESCAPIDLLVersion;

#endif
