/**********************************
 * FILE NAME: stdincludes.h
 *
 * DESCRIPTION: standard header file
 **********************************/

#ifndef _STDINCLUDES_H_
#define _STDINCLUDES_H_

/*
 * Macros
 */
#define RING_SIZE 512
#define FAILURE -1
#define SUCCESS 0

/*
 * Standard Header files
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <stdint.h>
#ifdef __linux__
#include <execinfo.h>
#endif

#include <signal.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <queue>
#include <fstream>

using namespace std;

#define STDCLLBKARGS (void *env, char *data, int size)
#define STDCLLBKRET	void
#define DEBUGLOG 1
typedef unsigned int uint;
#endif	/* _STDINCLUDES_H_ */
