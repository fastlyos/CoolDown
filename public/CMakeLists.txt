cmake_minimum_required(VERSION 2.6)
PROJECT( CoolDown-Public )
FILE(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
SET_PROPERTY( GLOBAL PROPERTY PUBLIC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_SUBDIRECTORY( error_code )
ADD_SUBDIRECTORY( torrent )
ADD_SUBDIRECTORY( job_history )
ADD_SUBDIRECTORY( tracker_proto )
ADD_SUBDIRECTORY( client_proto )
ADD_SUBDIRECTORY( netpack )

#MESSAGE( STATUS ${LIB_ERROR_CODE_FULLNAME} )
