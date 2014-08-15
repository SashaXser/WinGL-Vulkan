#ifndef _WGL_ASSERT_H_
#define _WGL_ASSERT_H_

#ifdef _DEBUG

#define __STRINGIFY( x ) #x
#define __TO_STRING( x ) __STRINGIFY( x )

#define WGL_YES    0
#define WGL_NO     1
#define WGL_CANCEL 2

#define WGL_ASSERT( exp ) \
{ \
static bool fire_assert = true; \
if (fire_assert && !(exp)) \
{ \
   int response = \
      __WglAssertMsgBox("WinGL Runtime Assert", \
                        "Assertion failed!\n\n" \
                        "Program: ...\n" \
                        "File: "__FILE__"\n" \
                        "Line: "__TO_STRING(__LINE__)"\n\n" \
                        "Expression: "#exp"\n\n" \
                        "Debug current application? Press:\n" \
                        "Yes - Debug current assert\n" \
                        "No - Ignore current assert indefinitely\n" \
                        "Cancel - Skip current assert"); \
   \
   if (response == WGL_YES) __debugbreak(); \
   else if (response == WGL_NO) fire_assert = false; \
} \
}

#define WGL_ASSERT_INIT( init, exp ) \
{ \
init; \
WGL_ASSERT( exp ); \
}

#define WGL_ASSERT_INIT_DEINIT( init, exp, deinit ) \
{ \
init; \
WGL_ASSERT( exp ); \
deinit; \
}

int __WglAssertMsgBox( const char * const pCaption, const char * const pMessage );

#else

#define WGL_ASSERT( exp )
#define WGL_ASSERT_INIT( init, exp )

#endif // _DEBUG

int WglMsgBox( const char * const pCaption, const char * const pMessage );

#endif // _WGL_ASSERT_H_
