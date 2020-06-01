
# Add CPP Check
include(CppcheckTargets)
add_cppcheck_sources(test UNUSED_FUNCTIONS STYLE POSSIBLE_ERRORS FORCE)

file(
    GLOB
    headers
    *.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/*.h
)

file(
    GLOB
    sources
    *.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/*.c
)

add_executable(
    mapper2310
    ${sources}
    ${headers}
)
target_link_libraries(mapper2310 m pthread)
set_target_properties(mapper2310 PROPERTIES LINKER_LANGUAGE C)

install(
  TARGETS mapper2310
    DESTINATION lib
)

install(
    FILES ${headers}
    DESTINATION include/${CMAKE_PROJECT_NAME}
)
