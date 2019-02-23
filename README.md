This library helps to debug software that uses the API for PassThru Vehicle Reprogramming (SAE J2534-1). It captures all necessary information to debug problems and presents that information to engineers in a human-readable form.

This software:

* Is intended for engineers. If you are not an engineer you can probably stop reading.
* Acts like a shim between the J2534-application and the J2534-device
* Works with any J2534-compliant vehicle interface
* Can be fully-integrated with an application or used with no changes at all

After loading a J2534 DLL the shim will continuously log debugging information to an internal circular memory-buffer and automatically capture a description string using PassThruGetLastError for most errors. This buffer can be saved to a UTF-8 encoded text file. Please refer to j2534-logger API documentation for details.

# Used when built-into an application

The preferred technique is to link `ptshim32.dll` with a J2534 application. Although this requires some small changes to a J2534 application, your application will have complete control and the shim will never popup an unexpected dialog box.

This library exports the standard PassThru functions plus 5 new functions:

```
long J2534_API PassThruLoadLibrary(char *szFunctionLibrary);
long J2534_API PassThruWriteToLogA(char *szMsg);
long J2534_API PassThruWriteToLogW(wchar_t *szMsg);
long J2534_API PassThruSaveLog(char *szFilename);
long J2534_API PassThruUnloadLibrary();
```

Like normal, the application should search the registry for installed J2534 devices. After the user selects a J2534 interface the application should call:

```
// Assume szFunctionLibrary is a NULL terminated ANSI string that contains
// the pathname to the J2534 DLL that we want to load

char * szFunctionLibrary = "c:\windows\system32\cdpls432.dll";
ptStatus = PassThruLoadLibrary(szFunctionLibrary);
```

The shim will load the desired DLL and start recording debugging information to a circular memory-buffer. The application may insert its own debugging text to the log with:

```
// For ANSI (char *) text
PassThruWriteToLogA("Adding an ANSI line to debug log");
// For UNICODE (wchar_t *) text
PassThruWriteToLogW(_T("Adding a UNICODE line to debug log"));
```

At any point the application can save the memory-buffer to a file by calling:

```
// Assume szFilename is a NULL terminated ANSI string that contains
// the pathname to the J2534 DLL that we want to load

char * szFilename = "C:\Users\Public\Documents\logfile.txt";
PassThruSaveLog(szFilename);
```

# Used like a PassThru vehicle interface

It is also possible to record debugging information with no changes to the original application. In this case you would compile `ptshim32.dll` with ALLOW_POPUP defined and list `ptshim32.dll` under the PassThruSupport.04.04 registry key like a normal J2534 Interface. Alternately you can download and install j2534-logger.msi.

The first function call (typically PassThruOpen) will open a dialog box:

- __Browse__: Allows you to choose an output filename.
- __Config__: Launches the device's configuration software, listed in the ConfigApplication registry key. This button is disabled if the EXE is not accessible.
- __OK__: Attempts to load the selected J2534 DLL, listed in the FunctionLibrary registry key. This button is disabled if the DLL is not accessible.
- __Cancel__: Exits without loading any J2534 DLL. The function call will return ERR_FAILED and the next function call will present the dialog again.

Once you select a J2534 interface you'll be "married" to that one until `ptshim32.dll` is unloaded or the process terminates. All debugging information will be written to the specified file.
