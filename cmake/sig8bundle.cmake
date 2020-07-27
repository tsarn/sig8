# This file is intended to be called using cmake -P

file(WRITE "${SIG8_BUNDLE_PATH}"
        "const char *${SIG8_BUNDLE_NAME} = \"\"
")

list(LENGTH SIG8_RESOURCE_PATHS N)

foreach(index RANGE 1 "${N}")
    list(GET SIG8_RESOURCE_PATHS -1 path)
    list(GET SIG8_RESOURCE_NAMES -1 name)
    list(REMOVE_AT SIG8_RESOURCE_PATHS -1)
    list(REMOVE_AT SIG8_RESOURCE_NAMES -1)

    file(READ "${path}" contents HEX)
    file(SIZE "${path}" size)

    file(APPEND "${SIG8_BUNDLE_PATH}" "
// res://${name}
\"${name}\\0\"\"${size}\\0\"\"${contents}\"
")
endforeach()
file(APPEND "${SIG8_BUNDLE_PATH}" ";")
