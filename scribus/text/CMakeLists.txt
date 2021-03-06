INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/scribus
	${FREETYPE_INCLUDE_DIRS}
	${CAIRO_INCLUDE_DIRS}
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_FPIC}")

SET(SCRIBUS_TEXT_MOC_CLASSES
	storytext.h
	boxes.h
)

SET(SCRIBUS_TEXT_LIB_SOURCES
	boxes.cpp
	frect.cpp
	fsize.cpp
	screenpainter.cpp
	sctext_shared.cpp
	specialchars.cpp
	storytext.cpp
	textlayout.cpp
	textlayoutpainter.cpp
	textshaper.cpp
)

QT5_WRAP_CPP(SCRIBUS_TEXT_MOC_SOURCES ${SCRIBUS_TEXT_MOC_CLASSES})

SET(SCRIBUS_TEXT_LIB "scribus_text_lib")
ADD_LIBRARY(${SCRIBUS_TEXT_LIB} STATIC ${SCRIBUS_TEXT_LIB_SOURCES} ${SCRIBUS_TEXT_MOC_SOURCES})
# This is a convenience library that for linkage purposes is part of Scribus's
# main API.
SET_TARGET_PROPERTIES(${SCRIBUS_TEXT_LIB}
	PROPERTIES
	COMPILE_FLAGS -DCOMPILE_SCRIBUS_MAIN_APP
)

