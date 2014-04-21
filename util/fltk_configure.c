#if defined(__linux__) || defined(__APPLE__)
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <unistd.h>
   #define MODUS ,0711)
   #define MAXPATH 255
#elif _WIN32__ || _MS_DOS_
    #include <dir.h>
    #define MODUS )
#else
    #include <direct.h>
    #define MODUS )
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cDetect.c"
#include "autoconf.c"

// This is used to keep track of stuff.
cdetect_string_t cflags;

/**
	Set up the command line options.
*/
void fltk_setup_options(void) {
	// cachefile is very neccessary for our caching...
   	config_option_register_group("cDetect extra");
	config_option_register("cache",0,"cache.txt",0,"cDetect cache file to use.");
	config_option_register("out",0,"FLTK/config.h",0,"Output config.h.");

   	// Generic stuff is generic. ovo
   	config_option_register_group("FLTK");

	// Originaly, we used a case/switch thing here. But now, we use this.
	#ifdef __CYGWIN__
	config_option_register("enable-cygwin",0,"no","yes","Enable usage of Cygwin");
	#endif
	
	#ifndef __APPLE__
	// On OS X, we always prefer Cocoa.
	config_option_register("enable-x11",0,"no","yes","Enable X11");
	#endif
	
	// Cairo, not implemented yet.
	config_option_register("enable-cairoext",0,"no","yes","Enable use of Cairo within FLTK");
	config_option_register("enable-cairo",0,"no","yes","Enable use of libcairo");
	
	// GL, neccessary for Phoenix Engine!
	config_option_register("enable-gl",0,"no","yes","Enable OpenGL support/emulation");

	// Just your average pthreads
	config_option_register("enable-threads",0,"no","yes","Enable usage of threads");
	
	// Local stuff
	config_option_register("enable-localzlib",0,"no","yes","Enable local zlib");
	config_option_register("enable-localjpeg",0,"no","yes","Enable local jpeg support");
	config_option_register("enable-localpng",0,"no","yes","Enable local png support");
}

/**
	Process the options after they have been parsed.
*/
void fltk_process_options(void) {
	// Really easy, really useful.
	config_cache_register( config_option_get("cache") );
	config_header_register( config_option_get("out") );
	
	// If cairo was on, we TURN it on.
	if(config_equal(config_option_get("enable-cairoext"), "yes") == CDETECT_TRUE) {
		cdetect_macro_define("FLTK_USE_CAIRO","1");
		cdetect_macro_define("FLTK_HAVE_CAIRO","1");
		/* Have to do this when not too lazy....
		CAIRODIR="cairo"
          	CAIROFLAGS="`pkg-config --cflags cairo`"
          	CAIROLIBS="-lcairo -lpixman-1"
        	CXXFLAGS="$CAIROFLAGS $CXXFLAGS"
	  	LINKFLTKCAIRO="../lib/libfltk_cairo.a"
	  	FLTKCAIROOPTION="-L ../cairo -lfltk_cairo$SHAREDSUFFIX"
	  	LIBS="$CAIROLIBS $LIBS" 
	  	*/
	}
	if(config_equal(config_option_get("enable-cairoext"), "yes") == CDETECT_TRUE) {
		cdetect_macro_define("FLTK_HAVE_CAIRO","1");
		/*
		CAIRODIR="cairo"
          CAIROFLAGS="`pkg-config --cflags cairo`"
          CAIROLIBS="-lcairo -lpixman-1"
          CXXFLAGS="$CAIROFLAGS $CXXFLAGS"
	  	LINKFLTKCAIRO="../lib/libfltk_cairo.a"
	  	FLTKCAIROOPTION="-L ../cairo -lfltk_cairo$SHAREDSUFFIX"
		*/
	}
}

/**
	The actual configuration part for headers, functions, types and quirks.
*/
void fltk_process_tools(void) {
	// General purpose return variable o.o
	int have_u;
	
	// standarts!
	config_header_check("sys/types.h");
	config_header_check("sys/stat.h");
	config_header_check("stdlib.h");
	config_header_check("string.h");
	config_header_check("memory.h");
	config_header_check("strings.h");
	config_header_check("inttypes.h");
	config_header_check("stdint.h");
	config_header_check("unistd.h");
	config_header_check("sys/select.h");
	config_header_check("sys/stdtypes.h");
	
	// We use "long long" for stuff, and for formatting as well as casting.
	have_u = config_type_check("long long");
	if(have_u != 0) {
		// Define the formating.
		config_macro_define("FLTK_LLFMT","\"%lld\"");
		config_macro_define("FLTK_LLCAST","(long long)");
	} else {
		config_macro_define("FLTK_LLFMT","\"%ld\"");
		config_macro_define("FLTK_LLCAST","(long)");
	}

	// U16, U32 and U64 are uint16_t, uint32_t and uint64_t. Thanks to FLTK mailing list!
	have_u = config_type_check("uint16_t")
		|| config_type_check_header("uint16_t","stddef.h") 
		|| config_type_check_header("uint16_t","inttypes.h");
	if(have_u != 0) config_macro_define("U16","uint16_t");	
	
	have_u = config_type_check("uint32_t")
		|| config_type_check_header("uint32_t","stddef.h") 
		|| config_type_check_header("uint32_t","inttypes.h");
	if(have_u != 0) config_macro_define("U32","uint32_t");

	have_u = config_type_check("uint64_t")
		|| config_type_check_header("uint64_t","stddef.h") 
		|| config_type_check_header("uint64_t","inttypes.h");
	if(have_u != 0) config_macro_define("U64","uint64_t");
	
	// dirent header check
	ac_header_dirent();
	
	// Test for POSIX-y scandir() function.
	have_u = config_compile_source(
		"#include <dirent.h>\n"
	  	"int func (const char *d, dirent ***list, void *sort) {\n"
	    "int n = scandir(d, list, 0, (int(*)(const dirent **, const dirent **))sort);\n"
	    "}\n", ""
	);
	config_function_check("scandir");
	if(have_u != 0) {
		// We are on posix.
		config_macro_define("HAVE_SCANDIR_POSIX","1");
	}

	// print functions...
	config_function_check("vsnprintf");
	config_function_check("snprintf");
	
	// Strings
	config_function_check("strcasecmp");	
	config_function_check("strlcat");
	config_function_check("strlcpy");
	
	// time
	config_header_check("locale.h");
	config_function_check("localeconv");
	
	// Math
	config_function_check_library("pow","m");

	// Somehow do the LARGEFILE checks...
	
	// dl-stuff
	config_function_check_library("dlsym", "dl");
	config_function_check_library("dlopen", "dl");
	config_header_check("dlfcn.h");
	
	// FLTK local libraries: jpeg, png, zlib. Configure then.
	// jpeg
	if(config_equal(config_option_get("enable-localjpeg"), "no") == CDETECT_TRUE) {
		config_function_check_library("jpeg_CreateCompress","jpeg");
		config_header_check("jpeglib.h");
	} else {
		cdetect_string_append(cflags, " -IFLTK/jpeg");
	}
	
	// zip
	if(config_equal(config_option_get("enable-localzlib"), "no") == CDETECT_TRUE) {
		config_function_check_library("gzgets","z");
		config_header_check("zlib.h");
	} else {
		cdetect_string_append(cflags, " -IFLTK/zlib");
	}

	// png
	if(config_equal(config_option_get("enable-localzlib"), "no") == CDETECT_TRUE) {
		config_function_check_library("png_read_info","png");
		config_header_check("png.h")
			|| config_header_check("libpng/png.h");
	} else {
		cdetect_string_append(cflags, " -IFLTK/png");
	}
	
	// Threading
	config_header_check("pthread.h");
	have_u = config_function_check_library("pthread_create", "pthread")
		|| config_function_check_library("pthread_create", "pthreads");
	if(have_u != 0) config_macro_define("HAVE_PTHREAD","1");
		
	// Graphics, GL. +1 for OS X, as this is always going to be true.
	#ifndef __APPLE__
	config_header_check("GL/gl.h") 
		&& config_macro_define("HAVE_GL","1");
	config_header_check("GL/glut.h");
	// OS X needs these flags: -framework AGL -framework OpenGL -framework ApplicationServices
	config_function_check_library("glXGetProcAddressARB","GL");
	#else
	config_report_bool("OpenGL Framework",1);
	config_macro_define("HAVE_GL","1");
	config_header_define("GL/glut.h",1);
	#endif
	
	// fontconfig, x11. NOT NEEDED ON OS X!
	#ifndef __APPLE__
	config_tool_check("FTCONFIG","freetype-config");
	if(cdetect_tool_exist("freetype-config") == CDETECT_TRUE) {
		config_function_check_library("FcPatternCreate","fontconfig");
		config_header_check("X11/Xft/Xft.h");
		config_function_check_library("XftDrawCreate","Xft");
	}
	config_header_check("X11/extensions/Xdbe.h");
	config_function_check_library("XdbeQueryExtension","Xext");
	config_header_check("X11/extensions/Xfixes.h");
	config_function_check_library("XFixesQueryExtension","Xfixes");
	#endif
	
	// Do the endianness from mac_endian!
	int endian = ac_c_bigendian(); // 1 big, 0 little
	config_macro_define_format("WORDS_BIGENDIAN", "%d", endian);
}

/**
	These are some defines that are cound in configh.in. They are set genericaly for now. 
	However, they might get their own --options later.
*/
void fltk_general_config(void) {
		// This is NOT a tool...but we do this, so we can use @BLABLABLA@ later.
		config_tool_define("FL_MAJOR_VERSION","1");
		config_tool_define("FL_MINOR_VERSION","3");
		config_tool_define("FL_PATCH_VERSION","2");
		config_tool_define("FL_API_VERSION","1.3");

		// Defined cause they need to be.
		config_macro_define("USE_COLORMAP","1");
		config_macro_define("FLTK_DOCDIR", "\"\"");
		config_macro_define("FLTK_DATADIR", "\"\"");
		config_macro_define("BORDER_WIDTH","2");
		
		#ifdef __APPLE__
		// ifdef == win...at least, in this case.
		config_macro_define("__APPLE_QUARTZ__","1");
		#endif
}

// Do the configs.
int main(int argc, char** argv) {
	// Begin config for FLTK!
	config_begin();
	ac_register();
	
	// globals
	cflags = cdetect_string_format("");

	// We have to do this kinda stuff before the parsing.
	fltk_setup_options();

    if (config_options(argc, argv)) {
		// Register some builds.
		config_build_register("util/fltk.cflags.in","out/fltk.cflags");

    	// configure.in ...
		fltk_process_options();
		fltk_general_config();
		fltk_process_tools();
		
		// configuring is done, write to file, after preparing.
		cdetect_string_append(cflags, " ");
		cdetect_string_append(cflags, config_tool_get("CFLAGS"));
		config_tool_define("CFLAGS", cflags->content);
    }
	config_end();
	
	// Sweeeeep...sweeeeeeep.
    cdetect_string_destroy(cflags);
}

		// Just running a test here.
		/*
cdetect_string_t create_cds_with_value(char* value) {
	cdetect_string_t rt = cdetect_string_create();
	cdetect_string_append(rt, value);
	return rt;
}

		cdetect_string_t input = cdetect_string_create();
		cdetect_string_t output= cdetect_string_create();
		//input->content = "FLTK, version @FL_MAJOR_VERSION@.@FL_MINOR_VERSION@.@FL_PATCH_VERSION@ - API: @FL_API_VERSION@.";
		cdetect_string_append(input, "FLTK: @FL_MAJOR_VERSION@.@FL_MINOR_VERSION@.@FL_PATCH_VERSION@ -- API: @FL_API_VERSION@");
		cdetect_substitute_string(input, &output);
		printf("INPUT: %s\n",input->content);
		printf("RESULT: %s\n",output->content);
		*/
