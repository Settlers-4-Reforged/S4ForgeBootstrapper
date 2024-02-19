#pragma once

extern S4API ModAPI;

#ifndef FORGE_VERSION
#define FORGE_VERSION 1.0.0
#endif

#define STRINGIZE_NX(A)         L ## #A
#define STRINGIZE(A)            STRINGIZE_NX(A)
#define FORGE_VERSION_STRING STRINGIZE(FORGE_VERSION)


