


#ifndef __UTIL_MACROS_H__
#define __UTIL_MACROS_H__


#ifndef AMBER_MAKEWORD
#define AMBER_MAKEWORD(a, b)      ((unsigned short)(((unsigned char)((unsigned int)(a) & 0xff)) | ((unsigned short)((unsigned char)((unsigned int)(b) & 0xff))) << 8))
#endif 

#ifndef AMBER_MAKELONG
#define AMBER_MAKELONG(a, b)      ((int)(((unsigned short)((unsigned int)(a) & 0xffff)) | ((unsigned int)((unsigned short)((unsigned int)(b) & 0xffff))) << 16))
#endif 

#ifndef AMBER_LOWORD
#define AMBER_LOWORD(l)           ((unsigned short)((unsigned int)(l) & 0xffff))
#endif 

#ifndef AMBER_HIWORD
#define AMBER_HIWORD(l)           ((unsigned short)((unsigned int)(l) >> 16))
#endif 

#ifndef AMBER_LOBYTE
#define AMBER_LOBYTE(w)           ((unsigned char)((unsigned int)(w) & 0xff))
#endif 

#ifndef AMBER_HIBYTE
#define AMBER_HIBYTE(w)           ((unsigned char)((unsigned int)(w) >> 8))

#define STATIC_ASSERT(exp) { int nArr[(exp)?1:-1]; nArr; }

#define CHECK_BREAK(exp) if (!(exp)){ break; }
#define CHECK_CONTINUE(exp) if (!(exp)){ continue; }
#define CHECK_RETURN(exp) if (!(exp)){ return; }
#define CHECK_RETURN_VALUE(exp, value) if (!(exp)){ return value; }
#define CHECK_RETURN_FALSE( exp ) CHECK_RETURN_VALUE( exp, false );

#endif 

/** @def DEPRECATED_ATTRIBUTE
* Only certain compilers support __attribute__((deprecated)).
*/
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define DEPRECATED_ATTRIBUTE(desc) __attribute__((deprecated(desc)))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#define DEPRECATED_ATTRIBUTE(desc) __declspec(deprecated(desc)) 
#else
#define DEPRECATED_ATTRIBUTE(desc)
#endif 

#endif // __UTIL_MACROS_H__