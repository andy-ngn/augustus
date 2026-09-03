# Pantheon: extra build targets for the Augustus fork used by Pantheon.
#
# Included (OPTIONAL) from the very end of the upstream CMakeLists.txt so every
# upstream variable (SOURCE_FILES, ASSETS_DIR, USE_FLAGS, ...) is available.
# Everything Pantheon-specific lives here or in new source directories; the
# upstream targets are untouched while the options below are OFF.

option(PANTHEON_VIEWER "Build the Pantheon viewer: full SDL game as a MODULARIZE'd ES module (Emscripten only)" OFF)

if(PANTHEON_VIEWER)
    if(NOT ${TARGET_PLATFORM} STREQUAL "emscripten")
        message(FATAL_ERROR "PANTHEON_VIEWER requires -DTARGET_PLATFORM=emscripten")
    endif()

    # C API exported to JavaScript (filled in from milestone 1 on).
    set(PANTHEON_API_FILES
    )

    set(PANTHEON_VIEWER_FILES ${SOURCE_FILES})
    list(REMOVE_ITEM PANTHEON_VIEWER_FILES ${PROJECT_SOURCE_DIR}/res/shell.html)

    add_executable(augustus-viewer ${PANTHEON_VIEWER_FILES} ${PANTHEON_API_FILES})
    target_compile_definitions(augustus-viewer PRIVATE PANTHEON PANTHEON_VIEWER)

    set(PANTHEON_VIEWER_LINK_FLAGS
        "-s MODULARIZE=1"
        "-s EXPORT_ES6=1"
        "-s EXPORT_NAME=createAugustusViewer"
        "-s ENVIRONMENT=web,worker"
        "-s INVOKE_RUN=0"
        "-s INITIAL_MEMORY=268435456"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s EXPORTED_FUNCTIONS=[\"_main\",\"_malloc\",\"_free\"]"
        "-s EXPORTED_RUNTIME_METHODS=[\"callMain\",\"FS\",\"ccall\",\"cwrap\",\"HEAPU8\",\"HEAP32\",\"HEAPU16\",\"HEAPU32\"]"
        "-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[\"$autoResumeAudioContext\"]"
        "--preload-file ${ASSETS_DIR}@/assets"
    )
    if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
        list(APPEND PANTHEON_VIEWER_LINK_FLAGS "-s ASSERTIONS=1")
    endif()
    string(JOIN " " PANTHEON_VIEWER_LINK_FLAGS_STR ${PANTHEON_VIEWER_LINK_FLAGS})
    set_target_properties(augustus-viewer PROPERTIES
        SUFFIX ".mjs"
        LINK_FLAGS "${PANTHEON_VIEWER_LINK_FLAGS_STR}"
    )
endif()
