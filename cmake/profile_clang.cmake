function(is_clang_cl return)
    if (${CMAKE_CXX_COMPILER} MATCHES ".+clang-cl.+")
        set(${return} TRUE PARENT_SCOPE)
    else()
        set(${return} FALSE PARENT_SCOPE)
    endif()
endfunction()


function(set_clang_option option)
    # Clang-cl requires GCC-style options to be prefixed by /clang:
    is_clang_cl(clang_cl)

    if (${clang_cl})
        add_compile_options("/clang:${option}")
    else()
        add_compile_options(${option})
    endif()
endfunction()


function(set_compiler_profile)
    is_clang_cl(clang_cl)


    if (${clang_cl})
        add_compile_options(/EHsc)
    else()
        add_compile_options(-fexceptions)
    endif()

    set_clang_option(-fbracket-depth=4194304)
    set_clang_option(-fconstexpr-depth=4194304)
    set_clang_option(-fconstexpr-steps=4194304)


    # Enable extra warnings.
    set_clang_option(-Wall)
    set_clang_option(-Wextra)

    set_clang_option(-Wnon-virtual-dtor)
    set_clang_option(-Wcast-align)
    set_clang_option(-Woverloaded-virtual)
    set_clang_option(-Wformat=2)


    # Unused variables may exist when they are only used in the debug configuration.
    set_clang_option(-Wno-unused-variable)
    set_clang_option(-Wno-unused-parameter)
    set_clang_option(-Wno-unused-private-field)
    set_clang_option(-Wno-unused-local-typedef)

    # TODO: Enable when the compiler supports it.
    # set_clang_option(-Wlifetime)
endfunction()