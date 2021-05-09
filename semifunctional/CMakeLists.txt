include(create_target)

create_target(
    semifunctional
    EXECUTABLE
    0 0 1
    # Dependencies:
    CONAN_PKG::boost
    CONAN_PKG::range-v3
    CONAN_PKG::ctre
)