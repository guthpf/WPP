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

#include <windows.h>
#include "escapi.h"

countCaptureDevicesProc countCaptureDevices;
initCaptureProc initCapture;
deinitCaptureProc deinitCapture;
doCaptureProc doCapture;
isCaptureDoneProc isCaptureDone;
getCaptureDeviceNameProc getCaptureDeviceName;
ESCAPIDLLVersionProc ESCAPIDLLVersion;

/* Internal: initialize COM */
typedef void (*initCOMProc)();
initCOMProc initCOM;

#include <iostream>
using namespace std;


wstring to_wstring(string const& str)
{
    size_t len = mbstowcs(0, &str[0], 0);
    if (len == -1)
        return wstring(0, 0);
    else
    {
        wstring wstr(len, 0);
        mbstowcs(&wstr[0], &str[0], wstr.size());
        return wstr;
    }
}

int setupESCAPI()
{
  /* Load DLL dynamically */
  wstring wstr = to_wstring(string("escapi.dll"));
  HMODULE capdll = LoadLibrary(&wstr[0]);
  if (capdll == NULL)
    return 0;

  /* Fetch function entry points */
  countCaptureDevices = (countCaptureDevicesProc)GetProcAddress(capdll, "countCaptureDevices");
  initCapture = (initCaptureProc)GetProcAddress(capdll, "initCapture");
  deinitCapture = (deinitCaptureProc)GetProcAddress(capdll, "deinitCapture");
  doCapture = (doCaptureProc)GetProcAddress(capdll, "doCapture");
  isCaptureDone = (isCaptureDoneProc)GetProcAddress(capdll, "isCaptureDone");
  initCOM = (initCOMProc)GetProcAddress(capdll, "initCOM");
  getCaptureDeviceName = (getCaptureDeviceNameProc)GetProcAddress(capdll, "getCaptureDeviceName");
  ESCAPIDLLVersion = (ESCAPIDLLVersionProc)GetProcAddress(capdll, "ESCAPIDLLVersion");

  /* Check that we got all the entry points */
  if (initCOM == NULL ||
      ESCAPIDLLVersion == NULL ||
      getCaptureDeviceName == NULL ||
      countCaptureDevices == NULL ||
      initCapture == NULL ||
      deinitCapture == NULL ||
      doCapture == NULL ||
      isCaptureDone == NULL)
      return 0;

  /* Verify DLL version */
  if (ESCAPIDLLVersion() != 0x200)
    return 0;

  /* Initialize COM.. */
  initCOM();

  /* and return the number of capture devices found. */
  return countCaptureDevices();
}

#endif
