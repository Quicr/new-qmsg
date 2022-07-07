project( netProc )

add_executable( netProc message_loop.cxx Network.cxx netProc.cxx )
target_link_libraries(netProc PRIVATE qmsgEncoder quicr)
target_compile_definitions(netProc PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_compile_options(netProc PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall -Wmissing-declarations>
     $<$<CXX_COMPILER_ID:MSVC>: /WX>)

add_executable(fakeSecProc message_loop.cxx fakeSecProc.cxx)
target_link_libraries(fakeSecProc PRIVATE qmsgEncoder quicr)
target_compile_definitions(netProc PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_compile_options(fakeSecProc PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall -Wmissing-declarations>
     $<$<CXX_COMPILER_ID:MSVC>: /WX>)
