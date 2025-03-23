cmake_minimum_required(VERSION 3.30)
project(untitled)

set (CMAKE_CXX_STANDARD 23)

# Явно укажите пути к Boost
set(BOOST_ROOT "C:/boost")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include/boost-1_87")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

# Настройка поиска Boost
set(Boost_USE_STATIC_LIBS OFF)  # Используем shared-библиотеки
set(Boost_DEBUG ON)  # Включите для отладки

find_package(Boost 1.87 REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Boost includes: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

    add_executable(${PROJECT_NAME} main.cpp
            tokens.h
            lexer.h
            parser.h
            interrupts.h
    )
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${BOOST_LIBRARYDIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Boost не найден!")
endif()