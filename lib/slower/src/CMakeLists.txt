
file(GLOB MEDIA_SOURCES *.cxx)
file(GLOB MEDIA_HEADER *.h)

add_library( slower  ${MEDIA_SOURCES} ${MEDIA_HEADERS} )

