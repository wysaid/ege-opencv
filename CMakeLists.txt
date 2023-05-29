cmake_minimum_required(VERSION 3.14)

project(ege-opencv)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(faceTracker)

include_directories(${ROOT_DIR}/deps/opencv/include
        ${ROOT_DIR}/deps/faceTracker
        ${ROOT_DIR}/deps/faceTracker/include
)

file(GLOB_RECURSE FACE_TRACKER_SRC "${ROOT_DIR}/deps/faceTracker/src/*.cpp" "${ROOT_DIR}/deps/faceTracker/*.cpp")

target_sources(faceTracker PRIVATE ${FACE_TRACKER_SRC})

file(GLOB_RECURSE OPENCV_LIBS "${ROOT_DIR}/deps/opencv/lib/*.lib")

file(GLOB_RECURSE OPENCV_DLLS "${ROOT_DIR}/bin/opencv_*.dll")

target_link_libraries(faceTracker PUBLIC ${OPENCV_LIBS})

# #####
add_executable(sprite_on_face ${ROOT_DIR}/src/sprite_on_face.cpp)

target_link_libraries(sprite_on_face PUBLIC faceTracker)

foreach(file_i ${OPENCV_DLLS})
        add_custom_command(TARGET sprite_on_face POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${file_i}"
                $<TARGET_FILE_DIR:sprite_on_face>)
endforeach(file_i)

# #####
add_executable(sprite_on_face_async ${ROOT_DIR}/src/sprite_on_face_async.cpp)

target_link_libraries(sprite_on_face_async PUBLIC faceTracker)

foreach(file_i ${OPENCV_DLLS})
        add_custom_command(TARGET sprite_on_face_async POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${file_i}"
                $<TARGET_FILE_DIR:sprite_on_face_async>)
endforeach(file_i)