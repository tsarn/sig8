add_executable(sig8tk main.c)
target_compile_features(sig8tk PRIVATE c_std_11)
target_link_libraries(sig8tk PRIVATE sig8)
