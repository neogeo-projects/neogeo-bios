cmake_minimum_required(VERSION 3.10)
project(sp-s2.sp1 C ASM)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Speed up rebuilds if ccache is available.
option(ENABLE_CCACHE "Use ccache for faster rebuilds" ON)
if(ENABLE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_ASM_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  endif()
endif()

# Set target architecture
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

# Set target tools
set(CMAKE_C_COMPILER m68k-linux-gnu-gcc)
set(CMAKE_C_FLAGS "-m68000 -O2 -fstack-usage -ffreestanding -fno-builtin -Wl,--gc-sections -ffunction-sections -fdata-sections -Wl,-Map,bios.map")

set(CMAKE_ASM_COMPILER m68k-linux-gnu-gcc)
set(CMAKE_ASM_FLAGS "-m68000 -x assembler-with-cpp")

set(CMAKE_LINKER m68k-linux-gnu-ld)
set(OBJCOPY m68k-linux-gnu-objcopy)
set(ELFSTRIP m68k-linux-gnu-strip)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/neogeo)

include_directories(include)
include(ExternalProject)
ExternalProject_Add(
    checksum
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools/checksum
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)

add_executable(${PROJECT_NAME}.elf
    "src/backup_ram.c"
    "src/bios_calls_table.s"
    "src/bios_calls.c"
    "src/bios_test.c"
    "src/bios.c"
    "src/calendar.c"
    "src/coin.c"
    "src/eyecatcher.c"
    "src/game_menu.c"
    "src/graphic.c"
    "src/how_to_play.c"
    "src/input.c"
    "src/interrupt.c"
    "src/led.c"
    "src/mess_out.c"
    "src/service.c"
    "src/service_dips.c"
    "src/service_hardware.c"
    "src/utils.c"
    "src/vectors.c"
)

add_dependencies(${PROJECT_NAME}.elf checksum)

# Define options
## Region
set(REGION "EUROPE" CACHE STRING "Select target region: JAPAN, USA, or EUROPE")
set_property(CACHE REGION PROPERTY STRINGS JAPAN USA EUROPE)

if(NOT REGION STREQUAL "JAPAN" AND NOT REGION STREQUAL "USA" AND NOT REGION STREQUAL "EUROPE")
    message(FATAL_ERROR "REGION must be one of: JAPAN, USA, EUROPE")
endif()

## System
set(SYSTEM "MVS" CACHE STRING "Select system type: AES or MVS")
set_property(CACHE SYSTEM PROPERTY STRINGS AES MVS)

if(NOT SYSTEM STREQUAL "AES" AND NOT SYSTEM STREQUAL "MVS")
    message(FATAL_ERROR "SYSTEM must be either AES or MVS")
endif()

target_compile_definitions(${PROJECT_NAME}.elf
    PRIVATE $<$<COMPILE_LANGUAGE:C>:REGION_${REGION}>
    PRIVATE $<$<COMPILE_LANGUAGE:ASM>:REGION_${REGION}>
    PRIVATE $<$<COMPILE_LANGUAGE:C>:SYSTEM_${SYSTEM}>
    PRIVATE $<$<COMPILE_LANGUAGE:ASM>:SYSTEM_${SYSTEM}>   
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_FLAGS "-Wl,--build-id=none -T '${CMAKE_SOURCE_DIR}/linker.ld'")
target_link_options(${PROJECT_NAME}.elf PRIVATE -nostdlib)

# Strip ELF from symbols
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ELFSTRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf
    COMMENT "Striping symbols from elf-binary"
)

# Convert ELF to raw binary
# Raw binary means little endian, must convert it back to big endian.
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${OBJCOPY} -O binary --pad-to=0xC20000 --gap-fill 0xFF ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.swp
    COMMENT "Creating raw BIOS ROM image"
)

# Calculate and add checksum
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_BINARY_DIR}/tools/checksum ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.swp
    COMMENT "Adding checksum"
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD    
    COMMAND dd if=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.swp of=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} conv=swab
    COMMENT "Running conv=swap to match endianess"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/000-lo.lo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/000-lo.lo
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/sfix.sfix ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sfix.sfix
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/sm1.sm1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sm1.sm1
        COMMENT "Moving resources for MAME"
    )
endif()