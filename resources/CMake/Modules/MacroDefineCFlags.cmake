MACRO(macro_define_cflags)

if (CMAKE_COMPILER_IS_GNUCXX)

   if (CMAKE_SYSTEM_NAME MATCHES Linux)
   
   set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")

# #    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual")

#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcoverage-mismatch")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-free-nonheap-object")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wjump-misses-init")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wignored-qualifiers")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit-function-declaration")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit-int")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winit-self")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winline")
#    #set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmaybe-uninitialized")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-pointer-cast")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winvalid-pch")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunsafe-loop-optimizations")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlong-long")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmain")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-braces")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-field-initializers")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-include-dirs")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-mudflap")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-multichar")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wnonnull")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-overflow")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Woverlength-strings")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpacked")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpacked-bitfield-compat")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wparentheses")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wreturn-type")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsequence-point")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-compare")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-conversion")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstack-protector")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-aliasing")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-overflow")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-enum")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsync-nand")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wtrampolines")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wtrigraphs")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wtype-limits")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wuninitialized")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunknown-pragmas")
# # #    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-local-typedefs")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvariadic-macros")
# # #   set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvector-operation-performance")
# #    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")

   
#    
   set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
   set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -O1")
#    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pie -fPIC -fPIE")
# #    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-z,relro,-z,now")
   set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector")

   if (CMAKE_BUILD_TYPE MATCHES Release)
      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -mmmx -msse -msse2 -m3dnow")
   endif (CMAKE_BUILD_TYPE MATCHES Release)

   if (CMAKE_BUILD_TYPE MATCHES Debug)
   
   #warnings flags
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wchar-subscripts")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wclobbered")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdisabled-optimization")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-div-by-zero")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdouble-promotion")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wempty-body")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wenum-compare")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-endif-labels")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfloat-equal")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat=2")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-contains-nul")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-extra-args")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-nonliteral")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-security")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-y2k")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvla")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvolatile-register-var")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-parameter")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-value")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-variable")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-parameter")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-variable")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-function")
      set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-label")

      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -pg -DDEBUG")
      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
   endif (CMAKE_BUILD_TYPE MATCHES Debug)


   endif (CMAKE_SYSTEM_NAME MATCHES Linux)
endif (CMAKE_COMPILER_IS_GNUCXX)


ENDMACRO(macro_define_cflags)
