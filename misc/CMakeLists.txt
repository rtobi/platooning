set(SOURCES
    spline.h
    colorset.h
)

add_library(misc SHARED ${SOURCES})
target_include_directories(misc PUBLIC
    $<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_SOURCE_DIR}/sumo)
set_property(TARGET misc PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
