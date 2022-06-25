cmake_minimum_required(VERSION 3.10 )

project( QMsg )

add_subdirectory( lib/slower ) 

add_subdirectory( src/uiProc )  
add_subdirectory( src/secProc ) 
add_subdirectory( src/netProc ) 

add_subdirectory( src/slowTest ) 
