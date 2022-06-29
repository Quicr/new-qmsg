add_library(qmsgEncoder
            data_buffer.cpp
            octet_string.cpp
            encoder.cpp
            serializer.cpp
            deserializer.cpp)

target_compile_options(qmsgEncoder PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Werror -Wall>
     $<$<CXX_COMPILER_ID:MSVC>: /WX>)

if (WIN32)
    target_link_libraries(qmsgEncoder PRIVATE ws2_32)
endif()

# Specify the internal and public include directories
target_include_directories(qmsgEncoder
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install target and associated include files
if (qmsg_INSTALL)
    install(TARGETS qmsgEncoder EXPORT qmsgEncoderTargets ARCHIVE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT qmsgEncoderTargets
            FILE qmsgEncoderTargets.cmake
            NAMESPACE QMSG::
            DESTINATION lib/cmake/qmsgEncoder/)
endif()
