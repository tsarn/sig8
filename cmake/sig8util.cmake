set(SIG8_PLATFORM_DIR "${PROJECT_SOURCE_DIR}/platform")

function(sig8_platform target)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        target_link_options(${target} PRIVATE
            --shell-file "${SIG8_PLATFORM_DIR}/emscripten/shell_minimal.html"
        )
    endif()

    target_link_libraries(${target} sig8::sig8)
endfunction()

function(sig8_bundle)
    cmake_parse_arguments(
        SIG8_BUNDLE
        "FORCE"
        "TARGET;NAME;FILE"
        "RESOURCES"
        ${ARGN}
    )

    set(SIG8_DEFAULT_BUNDLE NO)

    if (NOT DEFINED SIG8_BUNDLE_TARGET)
        message(FATAL_ERROR "sig8_bundle: TARGET not specified")
    endif()

    if (NOT DEFINED SIG8_BUNDLE_NAME)
        set(SIG8_BUNDLE_NAME SIG8_RESOURCE_BUNDLE)
        set(SIG8_DEFAULT_BUNDLE YES)
    endif()

    if (NOT DEFINED SIG8_BUNDLE_FILE)
        set(SIG8_BUNDLE_FILE sig8_resources)
    endif()

    if (NOT DEFINED SIG8_BUNDLE_RESOURCES)
        set(SIG8_BUNDLE_RESOURCES "")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        foreach(RES ${SIG8_BUNDLE_RESOURCES})
            target_link_options(${SIG8_BUNDLE_TARGET} PRIVATE
                --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/${RES}@${RES}"
            )
        endforeach()
        target_compile_definitions(${SIG8_BUNDLE_TARGET} PRIVATE SIG8_USE_RESOURCE_PATH="")
    elseif ((CMAKE_BUILD_TYPE MATCHES "Release") OR (SIG8_BUNDLE_FORCE))
        set(SIG8_BUNDLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SIG8_BUNDLE_FILE}")
        set(SIG8_BUNDLE_ARGS "")

        foreach(RES ${SIG8_BUNDLE_RESOURCES})
            list(APPEND SIG8_BUNDLE_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/${RES}")
            list(APPEND SIG8_BUNDLE_ARGS "${RES}")
        endforeach()

        add_custom_command(
            OUTPUT "${SIG8_BUNDLE_PATH}.c"
            COMMAND $<TARGET_FILE:bundler> "${SIG8_BUNDLE_NAME}" "${SIG8_BUNDLE_PATH}.c" ${SIG8_BUNDLE_ARGS}
            DEPENDS bundler ${SIG8_BUNDLE_RESOURCES}
            COMMENT "Generating bundles for target ${SIG8_BUNDLE_TARGET}"
            VERBATIM
        )

        file(WRITE "${SIG8_BUNDLE_PATH}.h"
                "#pragma once
extern const char *${SIG8_BUNDLE_NAME};"
        )

        target_include_directories(${SIG8_BUNDLE_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        target_sources(${SIG8_BUNDLE_TARGET} PRIVATE "${SIG8_BUNDLE_PATH}.h" "${SIG8_BUNDLE_PATH}.c")

        if (SIG8_DEFAULT_BUNDLE)
            target_compile_definitions(${SIG8_BUNDLE_TARGET} PRIVATE SIG8_USE_DEFAULT_BUNDLE)
        endif()
    else()
        target_compile_definitions(${SIG8_BUNDLE_TARGET} PRIVATE SIG8_USE_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")
    endif()
endfunction()
