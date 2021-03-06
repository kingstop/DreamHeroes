PROJECT(project_dreamheroes)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


add_definitions(-DDEBUG -pthread -LINUX)
add_definitions("-g")
add_definitions(-DLOG4CXX_STATIC) 
add_definitions("-std=gnu++11")
IF(UNIX)
    MESSAGE("UNIX")
ELSEIF(WIN32)
    MESSAGE("WIN32")
ELSE()
 MESSAGE("not found")
ENDIF()


INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/include/linklist
   ./../../../external/boost/boost_1_59_0/
    ./../../../external/mysql++-3.2.2/lib/
    ./../../../external/google/protobuf/src/
    ./../../../external/json/include/
    ./../../../external/log4cplus/log4cplus-1.2.0/include/
    ./../../../external/mysql-5.7.10/build/include/
    ./../../../external/mysql-5.7.10/include/ 
    ./../../../external/curl/curl/include/
    ./common/
    /usr/local/mysql/include/
    /usr/include/mysql/
   /usr/include/ 
   /usr/local/include/
) 


LINK_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/lib 
    ${PROJECT_SOURCE_DIR}/../../../external_lib/boost_1_60/
    ${PROJECT_SOURCE_DIR}/../../../external_lib/google/
    ${PROJECT_SOURCE_DIR}/../../../external_lib/json/
    ${PROJECT_SOURCE_DIR}/../../../external_lib/log4cplus_1_2_0/	
    ${PROJECT_SOURCE_DIR}/../../../external_lib/mysql/
    ${PROJECT_SOURCE_DIR}/../../../external_lib/curl/ 
    /usr/local/lib 
    /usr/lib 
) 

 
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin/server/${PROJECT_NAME}/ ) 
set(CMAKE_EXE_LINKER_FLAGS "-lpthread -lrt -lrt -lidn -Wall -lssl -Wall")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib) 

#add_definitions(-DDEBUG -pthread -LINUX)
#add_definitions("-g")
#add_definitions(-DLOG4CXX_STATIC)
ADD_SUBDIRECTORY(common) 
ADD_SUBDIRECTORY(databaseserver)
ADD_SUBDIRECTORY(gameserver)
ADD_SUBDIRECTORY(gateserver)
ADD_SUBDIRECTORY(login)
ADD_SUBDIRECTORY(CurlTest)
