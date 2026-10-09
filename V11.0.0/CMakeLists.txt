cmake_minimum_required(VERSION 3.15)
project(v850_v11_tests C)

# Toolchain configuration
set(TOOLCHAIN "C:/HighTec/toolchains/v850/v11.0.0")
set(TARGET_ARCH "rh850-g4mh2")
set(CMAKE_C_COMPILER "${TOOLCHAIN}/bin/clang.exe")
set(CMAKE_C_FLAGS "-march=${TARGET_ARCH} -Wall -Wextra")

# Test source files
set(TEST_SRCS
    TCRH-371/test_multi_arch.c
    TCRH-453/test_u2b_cpu.c
    TCRH-424/test_aliased_regs.c
    TCRH-341/test_libs_improve.c
    TCRH-408/test_fxu_vector.c
    TCRH-409/test_fxu_asm_constraint.c
    TCRH-456/test_os_inline_unroll.c
    TCRH-442/test_stack_optimize.c
    TCRH-443/test_branch_fusion.c
    TCRH-445/test_mov_fusion_crash.c
    TCRH-448/test_empty_struct.c
    TCRH-451/test_interrupt_funcptr.c
    TCRH-454/test_inline_asm_sysreg.c
)

# Create compile targets for each test
foreach(src ${TEST_SRCS})
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)

    add_executable(${name} ${src})
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${dir}"
    )

    # FXU tests need -mfpu=fxu
    if(${dir} MATCHES "TCRH-408|TCRH-409")
        target_compile_options(${name} PRIVATE -mfpu=fxu)
    endif()
endforeach()

# Generate assembly files target
add_custom_target(asm_all ALL)
foreach(src ${TEST_SRCS})
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)

    set(EXTRA_FLAGS "")
    if(${dir} MATCHES "TCRH-408|TCRH-409")
        set(EXTRA_FLAGS "-mfpu=fxu")
    endif()

    add_custom_command(
        TARGET asm_all
        COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -O2 ${EXTRA_FLAGS} -S
                ${CMAKE_CURRENT_SOURCE_DIR}/${src}
                -o ${CMAKE_BINARY_DIR}/${dir}/${name}.s
        DEPENDS ${src}
    )
endforeach()
