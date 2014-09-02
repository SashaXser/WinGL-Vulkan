cmake_minimum_required(VERSION 3.0.0)

if (NOT TARGET ${PROJECT_NAME_ZLIB})
   add_subdirectory("./input libs/zlib-1.2.8")
endif ( )

# pgn project setup
set(PNG_SHARED OFF)
set(PNG_STATIC ON)
set(PNG_TESTS OFF)
set(PNG_DEBUG OFF)
set(PNGARG OFF)
set(SKIP_INSTALL_ALL ON)

add_subdirectory("./input libs/libpng169")

# move png into ext-src folder
set_target_properties(png16_static PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/resil")

file(GLOB RES_IL_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/src-il/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src-il/src/*.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/src-il/include/*.h")
     
list(REMOVE_ITEM RES_IL_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/src-il/src/il_skia.cpp")

file(GLOB RES_IL_EXT_INC
     "${CMAKE_CURRENT_SOURCE_DIR}/include/il/*.h")
     
file(GLOB RES_IL_EXT_LIBS_JASPER
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/base/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/bmp/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jp2/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpc/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpg/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/mif/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/pgx/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/pnm/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/ras/*.[ch]"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/include/jasper/*.[ch]")
     
list(REMOVE_ITEM
     RES_IL_EXT_LIBS_JASPER
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpg/jpg_dec.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpg/jpg_dec.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpg/jpg_enc.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jasper-1.900.1/src/libjasper/jpg/jpg_enc.h")
     
file(GLOB RES_IL_EXT_LIBS_JPEG
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/*.[ch]")
     
list(REMOVE_ITEM
     RES_IL_EXT_LIBS_JPEG
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemdos.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemdos.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemmac.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemmac.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemansi.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemansi.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemname.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jmemname.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/wrjpgcom.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/wrjpgcom.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/rdjpgcom.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/rdjpgcom.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jpegtran.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/jpegtran.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/djpeg.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/djpeg.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/cjpeg.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/cjpeg.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/ckconfig.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/jpegsr9a/ckconfig.h")
     
file(GLOB RES_IL_EXT_LIBS_LCMS2
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/lcms2-2.5/include/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/lcms2-2.5/src/*.[ch]")
     
file(GLOB RES_IL_EXT_LIBS_TIFF
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/tiff-4.0.3/libtiff/*.[ch]")
     
list(REMOVE_ITEM
     RES_IL_EXT_LIBS_TIFF
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/tiff-4.0.3/libtiff/tif_unix.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/input libs/tiff-4.0.3/libtiff/tif_unix.h")
     
set(RES_IL_SRC_COMPLETE
    ${RES_IL_SRC}
    ${RES_IL_EXT_INC}
    ${RES_IL_EXT_LIBS_JASPER}
    ${RES_IL_EXT_LIBS_JPEG}
    ${RES_IL_EXT_LIBS_LCMS2}
    ${RES_IL_EXT_LIBS_TIFF})

add_library(resil STATIC ${RES_IL_SRC_COMPLETE})

add_definitions(/D JAS_WIN_MSVC_BUILD)
add_definitions(/wd4101 /wd4244 /wd4700 /wd4267 /wd4013 /wd4715 /wd4716 /wd4018 /wd4005)

string(REPLACE "/D NOMINMAX" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE "/D WIN32_LEAN_AND_MEAN" "/D _UNICODE /D UNICODE /D HAVE_CONFIG_H /D _CRT_NONSTDC_NO_WARNINGS" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE "/D NOMINMAX" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "/D WIN32_LEAN_AND_MEAN" "/D _UNICODE /D UNICODE /D HAVE_CONFIG_H /D _CRT_NONSTDC_NO_WARNINGS" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/il/config.h" CONFIG_H_CONTENTS)
string(REPLACE "#undef IL_NO_MNG" "#define IL_NO_MNG" CONFIG_H_CONTENTS ${CONFIG_H_CONTENTS})

file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/internal/config.h" ${CONFIG_H_CONTENTS})

source_group(il FILES ${RES_IL_SRC})
source_group(il-ext FILES ${RES_IL_EXT_INC})
source_group(ext-lib\\jasper FILES ${RES_IL_EXT_LIBS_JASPER})
source_group(ext-lib\\jpeg FILES ${RES_IL_EXT_LIBS_JPEG})
source_group(ext-lib\\lcms2 FILES ${RES_IL_EXT_LIBS_LCMS2})
source_group(ext-lib\\tiff FILES ${RES_IL_EXT_LIBS_TIFF})

target_include_directories(resil PRIVATE ./internal)
target_include_directories(resil PUBLIC ./include)
target_include_directories(resil PRIVATE ./src-il/include)
target_include_directories(resil PRIVATE "./input libs/jasper-1.900.1/src/libjasper/include")
target_include_directories(resil PRIVATE "./input libs/jpegsr9a")
target_include_directories(resil PRIVATE "./input libs/lcms2-2.5/include")
target_include_directories(resil PRIVATE "./input libs/tiff-4.0.3/libtiff")

target_link_libraries(resil PUBLIC png16_static)

set_target_properties(resil PROPERTIES FOLDER "${EXT_SRC_IDE_FOLDER}/resil")
