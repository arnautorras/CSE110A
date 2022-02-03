// $Id: oclib.h,v 1.15 2021-12-20 13:09:16-08 - - $

// Bilingual file useable as a header file for both oc and g++.

#ifndef OCLIB_H
#define OCLIB_H

#ifdef __cplusplus
extern "C" {
using string = char*;
#endif

#define EOF (-1)
#define bool int
#define true 1
#define false 0

#define assert(expr) {if (not (expr)) fail (#expr, __FILE__, __LINE__);}

void fail (string expr, string file, int line);

void putchr (int chr);
void putint (int num);
void putstr (string str);

int getchr();
string getstr();
string getln();

#ifdef __cplusplus
}
#endif

#endif

