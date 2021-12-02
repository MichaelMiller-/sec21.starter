#find_package(Catch2 CONFIG REQUIRED)
#find_package(Boost 1.70 REQUIRED)

function(test filename)
    project(test.${filename})
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/${filename}.cpp)

    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

    target_link_libraries(${PROJECT_NAME} PUBLIC Catch2::Catch2)
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)

    foreach (ARG ${ARGN})
        string(LENGTH "${ARG}" length_data_file)
        if (length_data_file)
            message(STATUS "copy configure file: ${CMAKE_SOURCE_DIR}/data/${ARG}")
            ## needed for console "make test"
            configure_file(${CMAKE_SOURCE_DIR}/data/${ARG} "${PROJECT_BINARY_DIR}/${ARG}" COPYONLY)
            ## needed for direct call from build directory
            configure_file(${CMAKE_SOURCE_DIR}/data/${ARG} "${CMAKE_BINARY_DIR}/${ARG}" COPYONLY)
            ## needed for Clion
            configure_file(${CMAKE_SOURCE_DIR}/data/${ARG} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ARG}" COPYONLY)
        endif ()
    endforeach ()

    add_test(
            NAME ${PROJECT_NAME}
            COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()

## unit tests via catch (alphabetical order)
##
## todo