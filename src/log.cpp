/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/log.h"

static FILE *logFile;

void Log::Init()
{
	setbuf(stdout, NULL);
	logFile = fopen("run.log","w");
}

void Log::Close()
{
	fclose(logFile);
}

void Log::Print(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}

void Log::Critical(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("Critical: ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}

void Log::ToFile(const char *str)
{
	fprintf(logFile, "%s", str);
}
