#include "cDetect.c"
#include "autoconf.c"

// defs
void zend_mature_stdc();
void zend_dlsym_check();
void zend_type_checks();
void zend_config_process();
void zend_double_check();

/* This is: php/Zend/configure.in

	This is going to be different from FLTK, mainly, since we have to be a tard bit different.
	Noamlly, Zend is configured within php's main configure.in. We are doing this independently, 
	so that we can do a components based build.
	
	Files that need to be generated:
		- php/zend/zend_config.h
		
	Notes:
		- Zend obviously has windows optimized files (.d/.dsp)!
*/
int main(int argc, char** argv) {
	config_begin();
	ac_register();
	
	// We register some options here.
	// cachefile is very neccessary for our caching...
   	config_option_register_group("cDetect extra");
	config_option_register("cache",0,"cache.txt",0,"cDetect cache file to use.");
	config_option_register("out",0,"php/Zend/zend_config.h",0,"Output config.h.");
	
	// Zend options
	config_option_register_group("Zend Engine");
	config_option_register(
		"with-zend-vm",0,"CALL",0,
		"Set virtual machine dispatch method. "
		"Type is one of \"CALL\", \"SWITCH\" or \"GOTO\""
	);
	config_option_register("enable-maintainer-zts",0,"no","yes","Enable thread safety - for code maintainers only!!");
	config_option_register("disable-inline-optimization",0,"no","yes","If building zend_execute.lo fails, try this switch");
	config_option_register("enable-debug",0,"no","yes","If building zend_execute.lo fails, try this switch");

	config_option_register("enable-zend-signals",0,"no","yes","Use zend signal handling");
	config_option_register("enable-debug",0,"no","yes","If building zend_execute.lo fails, try this switch");
	
	if(config_options(argc, argv)) {
		// The main logic happens here, just as always.
		zend_config_process();
		
		// Zend defines a top part, we define it too.
		// NOTE: It is obvious, that it wants to define this stuff if we are NOT on microsoft!
		// This check could be made simpler by FAR...
		config_tool_define(
			"ZEND_TOP",
			"#if defined(__GNUC__) && __GNUC__ >= 4\n"
			"# define ZEND_API __attribute__ ((visibility(\"default\")))\n"
			"# define ZEND_DLEXPORT __attribute__ ((visibility(\"default\")))\n"
			"#else\n"
			"# define ZEND_API\n"
			"# define ZEND_DLEXPORT\n"
			"#endif\n"
			"\n"
			"#define ZEND_DLIMPORT\n"
			"\n"
			"#undef uint\n"
			"#undef ulong\n"
			"\n"
			"/* Define if you want to enable memory limit support */\n"
			"#define MEMORY_LIMIT 0\n"
		);
		
		// Aaaand of course, a bottom.
		// Again, this could be simplified.
		config_tool_define(
			"ZEND_BOTTOM",
			"#ifndef ZEND_ACCONFIG_H_NO_C_PROTOS\n"
			"\n"
			"#ifdef HAVE_STDLIB_H\n"
			"# include <stdlib.h>\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_SYS_TYPES_H\n"
			"# include <sys/types.h>\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_SYS_SELECT_H\n"
			"#include <sys/select.h>\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_IEEEFP_H\n"
			"# include <ieeefp.h>\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_STRING_H\n"
			"# include <string.h>\n"
			"#else\n"
			"# include <strings.h>\n"
			"#endif\n"
			"\n"
			"#if ZEND_BROKEN_SPRINTF\n"
			"int zend_sprintf(char *buffer, const char *format, ...);\n"
			"#else\n"
			"# define zend_sprintf sprintf\n"
			"#endif\n"
			"\n"
			"#include <math.h>\n"
			"\n"
			"/* To enable the is_nan, is_infinite and is_finite PHP functions */\n"
			"#ifdef NETWARE\n"
			"	#define HAVE_ISNAN 1\n"
			"	#define HAVE_ISINF 1\n"
			"	#define HAVE_ISFINITE 1\n"
			"#endif\n"
			"\n"
			"#ifndef zend_isnan\n"
			"#ifdef HAVE_ISNAN\n"
			"#define zend_isnan(a) isnan(a)\n"
			"#elif defined(HAVE_FPCLASS)\n"
			"#define zend_isnan(a) ((fpclass(a) == FP_SNAN) || (fpclass(a) == FP_QNAN))\n"
			"#else\n"
			"#define zend_isnan(a) 0\n"
			"#endif\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_ISINF\n"
			"#define zend_isinf(a) isinf(a)\n"
			"#elif defined(INFINITY)\n"
			"/* Might not work, but is required by ISO C99 */\n"
			"#define zend_isinf(a) (((a)==INFINITY)?1:0)\n"
			"#elif defined(HAVE_FPCLASS)\n"
			"#define zend_isinf(a) ((fpclass(a) == FP_PINF) || (fpclass(a) == FP_NINF))\n"
			"#else\n"
			"#define zend_isinf(a) 0\n"
			"#endif\n"
			"\n"
			"#ifdef HAVE_FINITE\n"
			"#define zend_finite(a) finite(a)\n"
			"#elif defined(HAVE_ISFINITE) || defined(isfinite)\n"
			"#define zend_finite(a) isfinite(a)\n"
			"#elif defined(fpclassify)\n"
			"#define zend_finite(a) ((fpclassify((a))!=FP_INFINITE&&fpclassify((a))!=FP_NAN)?1:0)\n"
			"#else\n"
			"#define zend_finite(a) (zend_isnan(a) ? 0 : zend_isinf(a) ? 0 : 1)\n"
			"#endif\n"
			"\n"
			"#endif /* ifndef ZEND_ACCONFIG_H_NO_C_PROTOS */\n"
			"\n"
			"#ifdef NETWARE\n"
			"#ifdef USE_WINSOCK\n"
			"#/*This detection against winsock is of no use*/ undef HAVE_SOCKLEN_T\n"
			"#/*This detection against winsock is of no use*/ undef HAVE_SYS_SOCKET_H\n"
			"#endif\n"
			"#endif\n"
			"\n"
		);
		
		// Do some checks
		ac_header_stdc();
		zend_mature_stdc();
		ac_type_size_t();
		zend_type_checks();
		ac_header_mmap_anonymous();
		ac_header_dirent();
		
		config_function_check_library("dlopen", "dl");
		config_function_check("vprintf");
		config_function_check("memcmp");
		config_function_check("alloca");
		config_function_check("memcpy");
		config_function_check("strdup");
		config_function_check("getpid");
		config_function_check("kill");
		config_function_check("strtod");
		config_function_check("strtol");
		config_function_check("finite");
		config_function_check("fpclass");
		config_function_check("sigsetjmp");
		config_function_check("finite");
		config_function_check("isfinite");
		config_function_check("isinf");
		config_function_check("isnan");
		config_function_check("sigaction");
		
		// An OS check.
		#if __APPLE__ || __DARWIN__
		config_macro_define("DARWIN","1");
		#endif
	}
	config_end();
}

void zend_config_process() {
	config_cache_register( config_option_get("cache") );
	config_header_register( config_option_get("out") );

	config_report_string(
		"virtual machine dispatch method",
		config_option_get("with-zend-vm")
	);
	config_report_bool(
		"whether to enable thread-safety",
		config_equal( config_option_get("enable-maintainer-zts"), "yes") ? 1 : 0
	);
	config_report_bool(
		"whether to enable inline optimization for GCC",
		config_equal(config_option_get("disable-inline-optimization"), "yes")
	);
	
	// some more checks, right here.
	if(config_equal( config_option_get("with-zend-vm"), "SWITCH" )) {
		config_macro_define("ZEND_VM_KIND","1");
		config_macro_define("ZEND_VM_KIND_SWITCH","1");
	} else if(config_equal( config_option_get("with-zend-vm"), "GOTO" )) {
		config_macro_define("ZEND_VM_KIND","1");
		config_macro_define("ZEND_VM_KIND_GOTO","1");
	} else {
		config_macro_define("ZEND_VM_KIND","1");
		config_macro_define("ZEND_VM_KIND_CALL","1");
	}
	
	// I can has debug?
	if(config_equal( config_option_get("enable-debug"), "yes" )) {
		config_macro_define("ZEND_DEBUG","1");
	} else {
		// Hmmmmm....really?
		//config_macro_define("ZEND_DEBUG","0");
	}
	if(config_equal( config_option_get("enable-maintainer-zts"), "yes" )) {
		config_macro_define("ZTS","1");
	} else {
		// Hmmmmm....really?
		//config_macro_define("ZEND_DEBUG","0");
	}
	
	int use_sigs=0;
	if(
		config_equal( config_option_get("enable-zend-signals"), "yes" )
		&& (config_function_check("sigaction") != 0)
	) {
		config_macro_define("ZEND_SIGNALS","1");
		use_sigs = 1;
	}
	config_report_bool(
		"whether to enable zend signal handling",
		use_sigs
	);
	
	// One more, Jonny!
	int have_ur = (int)cdetect_file_exist("/dev/urandom");
	int have_ar = (int)cdetect_file_exist("/dev/arandom");
	
	if( have_ur != 0 ) {
		config_report_bool("whether /dev/urandom exists", have_ur);
		config_macro_define("HAVE_DEV_URANDOM","1");
	} else if( have_ar != 0 ) {
		config_report_bool("whether /dev/arandom exists", have_ar);
		config_macro_define("HAVE_DEV_ARANDOM","1");
	}
}
void zend_mature_stdc() {
	// We really check for them all.
	config_header_check("stdlib.h");
	config_header_check("stdarg.h");
	config_header_check("string.h");
	config_header_check("float.h");
	config_header_check("inttypes.h");
	config_header_check("stdint.h");
	config_header_check("limits.h");
	config_header_check("malloc.h");
	config_header_check("malloc/malloc.h");
	config_header_check("string.h");
	config_header_check("unistd.h");
	config_header_check("sys/types.h");
	config_header_check("sys/time.h");
	config_header_check("signal.h");
	config_header_check("unix.h");
	config_header_check("dlfcn.h");
}
void zend_type_checks() {
	config_type_check("uint")
		|| config_type_check("unsigned int");
	config_type_check("ulong")
		|| config_type_check("unsigned long");

	config_type_check("uint16_t")
		|| config_type_check_header("uint16_t","stddef.h") 
		|| config_type_check_header("uint16_t","inttypes.h");

	config_type_check("uint32_t")
		|| config_type_check_header("uint32_t","stddef.h") 
		|| config_type_check_header("uint32_t","inttypes.h");

	config_type_check("uint64_t")
		|| config_type_check_header("uint64_t","stddef.h") 
		|| config_type_check_header("uint64_t","inttypes.h");

	config_type_check("int16_t")
		|| config_type_check_header("int16_t","stddef.h") 
		|| config_type_check_header("int16_t","inttypes.h");

	config_type_check("int32_t")
		|| config_type_check_header("int32_t","stddef.h") 
		|| config_type_check_header("int32_t","inttypes.h");

	config_type_check("int64_t")
		|| config_type_check_header("int64_t","stddef.h") 
		|| config_type_check_header("int64_t","inttypes.h");
		
	// Extra type check.
	zend_double_check();
}
void zend_double_check() {
	int rt = config_compile_source(
		"#include <limits.h>                                                                               \n"
        "int main() {                                                                                      \n"
	    "    if (sizeof(long) == 4) {                                                                      \n"
		"        double d = (double) LONG_MIN * LONG_MIN + 2e9;                                            \n"
		"        if ((long) d == 2e9 && (long) -d == -2e9) {                                               \n"
		"	        exit(0);                                                                               \n"
		"        }                                                                                         \n"
	    "    } else if (sizeof(long) == 8) {                                                               \n"
		"        double correct = 18e18 - ((double) LONG_MIN * -2); /* Subtract ULONG_MAX + 1 */           \n"
		"        if ((long) 18e18 == correct) { /* On 64-bit, only check between LONG_MAX and ULONG_MAX */ \n"
		"	         exit(0);                                                                              \n"
		"        }                                                                                         \n"
	    "    }                                                                                             \n"
	    "    exit(1);                                                                                      \n"
        "}                                                                                                 \n",
        "");
    if( rt == 0) {
    	// true
    	config_macro_define("ZEND_DVAL_TO_LVAL_CAST_OK","1");
    }
    config_report_bool("double cast to long preserves least significant bits",rt);
}

/*AC_DEFUN([LIBZEND_DLSYM_CHECK],[
dnl
dnl Ugly hack to check if dlsym() requires a leading underscore in symbol name.
dnl
AC_MSG_CHECKING([whether dlsym() requires a leading underscore in symbol names])
_LT_AC_TRY_DLOPEN_SELF([
  AC_MSG_RESULT(no)
], [
  AC_MSG_RESULT(yes)
  AC_DEFINE(DLSYM_NEEDS_UNDERSCORE, 1, [Define if dlsym() requires a leading underscore in symbol names. ])
], [
  AC_MSG_RESULT(no)
], [])
])
*/
void zend_dlsym_check() {
	// Do this so we can actually use this!
	cdetect_save_files();
	// Note, we use a format here to define the header.
	char* source1 = "void derpderp() { }";
	char* source2 = 
		"#include \"@CONFIG_H@\"                                \n"
		"#ifdef HAVE_DLFCN                                      \n"
		"#include <dlfcn.h>                                     \n"
		"#endif                                                 \n"
		"                                                       \n"
		"int main() {                                           \n"
		"    void* hande;                                       \n"
		"    void* symbol;                                      \n"
		"#ifndef RTLD_LAZY                                      \n"
        "    int mode = 1;                                      \n"
        "#else                                                  \n"
       "    int mode = RTLD_LAZY;                               \n"
       "#endif                                                  \n"
       "    handle = dlopen(\"./dyna.@SHLIB_EXT@\", mode);      \n"
       "    if (handle == NULL) {                               \n"
	   "        printf (\"1\");                                 \n"
	   "        fflush(stdout);                                 \n"
	   "        exit(0);                                        \n"
       "    }                                                   \n"
       "    symbol = dlsym(handle, \"fred\");                   \n"
       "    if (symbol == NULL) {                               \n"
	   "        /* try putting a leading underscore */          \n"
	   "        symbol = dlsym(handle, \"_fred\");              \n"
	   "        if (symbol == NULL) {                           \n"
	   "            printf(\"2\");                              \n"
	   "            fflush(stdout);                             \n"
	   "            exit(0);                                    \n"
	   "        }                                               \n"
	   "        printf(\"3\");                                  \n"
       "    }                                                   \n" 
       "    else printf (\"4\");                                \n"
       "    fflush(stdout);                                     \n"
       "    exit(0);                                            \n"
       "}                                                       \n";
	// stuckness is suckness.
}

/*dnl test and set the alignment define for ZEND_MM
dnl this also does the logarithmic test for ZEND_MM.
AC_MSG_CHECKING(for MM alignment and log values)

AC_TRY_RUN([
#include <stdio.h>

typedef union _mm_align_test {
  void *ptr;
  double dbl;
  long lng;
} mm_align_test;

#if (defined (__GNUC__) && __GNUC__ >= 2)
#define ZEND_MM_ALIGNMENT (__alignof__ (mm_align_test))
#else
#define ZEND_MM_ALIGNMENT (sizeof(mm_align_test))
#endif

int main()
{
  int i = ZEND_MM_ALIGNMENT;
  int zeros = 0;
  FILE *fp;

  while (i & ~0x1) {
    zeros++;
    i = i >> 1;
  }

  fp = fopen("conftest.zend", "w");
  fprintf(fp, "%d %d\n", ZEND_MM_ALIGNMENT, zeros);  
  fclose(fp);

  exit(0);
}
], [
  LIBZEND_MM_ALIGN=`cat conftest.zend | cut -d ' ' -f 1`
  LIBZEND_MM_ALIGN_LOG2=`cat conftest.zend | cut -d ' ' -f 2`
  AC_DEFINE_UNQUOTED(ZEND_MM_ALIGNMENT, $LIBZEND_MM_ALIGN, [ ])
  AC_DEFINE_UNQUOTED(ZEND_MM_ALIGNMENT_LOG2, $LIBZEND_MM_ALIGN_LOG2, [ ]) 
], [], [
  dnl cross-compile needs something here
  LIBZEND_MM_ALIGN=8
])*/
/*dnl test for memory allocation using mmap("/dev/zero")
AC_MSG_CHECKING(for memory allocation using mmap("/dev/zero"))

AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef MAP_ANON
# ifdef MAP_ANONYMOUS
#  define MAP_ANON MAP_ANONYMOUS
# endif
#endif
#ifndef MREMAP_MAYMOVE
# define MREMAP_MAYMOVE 0
#endif
#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

#define SEG_SIZE (256*1024)

int main()
{
	int fd;
	void *seg;

	fd = open("/dev/zero", O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		return 1;
	}
	seg = mmap(NULL, SEG_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (seg == MAP_FAILED) {
		return 2;
	}
	if (munmap(seg, SEG_SIZE) != 0) {
		return 3;
	}
	if (close(fd) != 0) {
		return 4;
	}
	return 0;
}
], [
  AC_DEFINE([HAVE_MEM_MMAP_ZERO], 1, [Define if the target system has support for memory allocation using mmap("/dev/zero")])
  AC_MSG_RESULT(yes)
], [
  AC_MSG_RESULT(no)
], [
  dnl cross-compile needs something here
  AC_MSG_RESULT(no)
])*/
