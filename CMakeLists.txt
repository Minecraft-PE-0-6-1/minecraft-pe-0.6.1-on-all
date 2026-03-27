cmake_minimum_required(VERSION 3.21)
project(MinecraftPE)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

include(cmake/EnumOption.cmake)

if(EMSCRIPTEN)
    # When configuring web builds with "emcmake cmake -B build -S .", set PLATFORM to Web by default
    set(PLATFORM Web CACHE STRING "Platform to build for.")
endif()

enum_option(PLATFORM "Desktop;Web" "Platform to build for.")

find_package(Threads REQUIRED)
find_package(OpenSSL)

if (${PLATFORM} STREQUAL "Desktop")
    set(PLATFORM_CPP "PLATFORM_DESKTOP")

    if (MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing -Wno-narrowing -Wno-invalid-source-encoding -Wno-reserved-user-defined-literal")
    endif()

    if (WIN32)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        include_directories(misc/windows)
        set(EXTRA_LIBS ws2_32 winhttp)
    elseif(UNIX)
        find_library(pthread NAMES pthread)
        set(EXTRA_LIBS pthread m)
    endif()

elseif (${PLATFORM} STREQUAL "Web")
    set(PLATFORM_CPP "PLATFORM_WEB")
    set(EXTRA_LIBS "idbfs.js")
endif()

# I totally shocked
if(${PLATFORM} MATCHES "Web")
    set(PNG_LIB png)
    set(AL_LIBTYPE "STATIC")

    add_library(zlib INTERFACE IMPORTED)
    set_target_properties(zlib PROPERTIES
        INTERFACE_LINK_OPTIONS "-sUSE_ZLIB=1"
    )

    add_library(png INTERFACE IMPORTED)
    set_target_properties(png PROPERTIES
        INTERFACE_COMPILE_OPTIONS "-sUSE_LIBPNG=1" 
        INTERFACE_LINK_OPTIONS "-sUSE_LIBPNG=1"
    )

    add_library(glfw INTERFACE IMPORTED)
    set_target_properties(glfw PROPERTIES
        INTERFACE_LINK_OPTIONS "-sUSE_GLFW=3"
    )
else()
    set(PNG_LIB png_shared)
    set(AL_LIBTYPE "SHARED")

    CPMAddPackage(
        NAME "zlib"
        GIT_REPOSITORY "https://github.com/madler/zlib"
        GIT_TAG "v1.3.2"
    )

    CPMAddPackage(
        NAME "libpng"
        GIT_REPOSITORY "https://github.com/pnggroup/libpng.git"
        GIT_TAG "v1.6.55"
        OPTIONS
            "ZLIB_ROOT ${zlib_SOURCE_DIR}"
            "ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR}"
            "PNG_TOOLS OFF"
            "PNG_TESTS OFF"
    )

    CPMAddPackage(
        NAME "glfw"
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "3.4"
        EXCLUDE_FROM_ALL TRUE
        OPTIONS
            "GLFW_BUILD_EXAMPLES OFF"
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_DOCS OFF"
    )
endif()

CPMAddPackage(
    NAME "openal"
    GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
    GIT_TAG "1.25.1"
    OPTIONS
        "ALSOFT_EXAMPLES OFF"
        "ALSOFT_TESTS OFF"
        "ALSOFT_UTILS OFF"
        "LIBTYPE ${AL_LIBTYPE}"
        "ALSOFT_ENABLE_MODULES OFF"
        "ALSOFT_STATIC_STDCXX ON"
        "ALSOFT_STATIC_LIBGCC ON"
)

file(GLOB SERVER_SOURCES
    "project/lib_projects/raknet/jni/RaknetSources/*.cpp"
    "src/NinecraftApp.cpp"
    "src/Performance.cpp"
    "src/SharedConstants.cpp"

    "src/client/IConfigListener.cpp"
    "src/client/Minecraft.cpp"
    "src/client/OptionStrings.cpp"
    "src/client/Option.cpp"
    "src/client/Options.cpp"
    "src/client/OptionsFile.cpp"
    "src/client/ServerProfiler.cpp"

    "src/client/gamemode/CreativeMode.cpp"
    "src/client/gamemode/GameMode.cpp"
    "src/client/gamemode/SurvivalMode.cpp"

    "src/client/player/LocalPlayer.cpp"
    "src/client/player/RemotePlayer.cpp"
    "src/client/player/input/KeyboardInput.cpp"

    "src/locale/I18n.cpp"

    "src/main.cpp"
    "src/main_dedicated.cpp"

    "src/nbt/Tag.cpp"

    "src/network/ClientSideNetworkHandler.cpp"
    "src/network/NetEventCallback.cpp"
    "src/network/Packet.cpp"
    "src/network/RakNetInstance.cpp"
    "src/network/ServerSideNetworkHandler.cpp"
    "src/network/command/CommandServer.cpp"

    "src/platform/CThread.cpp"
    "src/platform/HttpClient.cpp"
    "src/platform/PngLoader.cpp"
    "src/platform/time.cpp"

    "src/platform/input/Controller.cpp"
    "src/platform/input/Keyboard.cpp"
    "src/platform/input/Mouse.cpp"
    "src/platform/input/Multitouch.cpp"

    "src/server/ArgumentsSettings.cpp"
    "src/server/ServerLevel.cpp"
    "src/server/ServerPlayer.cpp"

    "src/util/DataIO.cpp"
    "src/util/Mth.cpp"
    "src/util/PerfTimer.cpp"
    "src/util/StringUtils.cpp"

    "src/world/Direction.cpp"

    "src/world/entity/*.cpp"

    "src/world/entity/ai/control/MoveControl.cpp"

    "src/world/entity/animal/*.cpp"

    "src/world/entity/item/*.cpp"

    "src/world/entity/monster/*.cpp"

    "src/world/entity/player/Inventory.cpp"
    "src/world/entity/player/Player.cpp"

    "src/world/entity/projectile/Arrow.cpp"
    "src/world/entity/projectile/Throwable.cpp"

    "src/world/food/SimpleFoodData.cpp"

    "src/world/inventory/*.cpp"
    "src/world/item/*.cpp"
    "src/world/item/crafting/*.cpp"
    "src/world/level/*.cpp"

    "src/world/level/biome/Biome.cpp"
    "src/world/level/biome/BiomeSource.cpp"

    "src/world/level/chunk/LevelChunk.cpp"

    "src/world/level/dimension/Dimension.cpp"

    "src/world/level/levelgen/*.cpp"
    "src/world/level/levelgen/feature/Feature.cpp"
    "src/world/level/levelgen/synth/*.cpp"

    "src/world/level/material/Material.cpp"

    "src/world/level/pathfinder/Path.cpp"

    "src/world/level/storage/*.cpp"
    "src/world/level/tile/*.cpp"
    "src/world/level/tile/entity/*.cpp"

    "src/world/phys/HitResult.cpp"
) 

file(GLOB CLIENT_SOURCES 
    "src/client/*.cpp"

    "src/client/gamemode/*.cpp"
    "src/client/gui/*.cpp"
    "src/client/gui/components/*.cpp"
    "src/client/gui/screens/*.cpp"
    "src/client/gui/screens/crafting/*.cpp"
    "src/client/gui/screens/touch/*.cpp"

    "src/client/model/*.cpp"
    "src/client/model/geom/*.cpp"

    "src/client/particle/*.cpp"

    "src/client/player/*.cpp"
    "src/client/player/input/*.cpp"
    "src/client/player/input/touchscreen/*.cpp"

    "src/client/renderer/*.cpp"
    "src/client/renderer/culling/*.cpp"
    "src/client/renderer/entity/*.cpp"
    "src/client/renderer/ptexture/*.cpp"
    "src/client/renderer/tileentity/*.cpp"

    "src/client/sound/*.cpp"
    
    "src/locale/*.cpp"
    
    "src/nbt/*.cpp"

    "src/network/*.cpp"
    "src/network/command/*.cpp"

    "src/platform/*.cpp"
    "src/platform/audio/SoundSystemAL.cpp"
    "src/platform/input/*.cpp"

    "src/raknet/**.cpp"
    
    "src/server/**.cpp"
    
    "src/util/**.cpp"

    "src/world/*.cpp"
    "src/world/phys/*.cpp"
    "src/world/entity/*.cpp"
    "src/world/entity/ai/control/*.cpp"
    "src/world/entity/animal/*.cpp"
    "src/world/entity/item/*.cpp"
    "src/world/entity/monster/*.cpp"
    "src/world/entity/player/*.cpp"
    "src/world/entity/projectile/*.cpp"

    "src/world/food/*.cpp"
    "src/world/inventory/*.cpp"

    "src/world/item/*.cpp"
    "src/world/item/crafting/*.cpp"

    "src/world/level/*.cpp"
    "src/world/level/biome/*.cpp"
    "src/world/level/chunk/*.cpp"
    "src/world/level/dimension/*.cpp"
    "src/world/level/levelgen/*.cpp"
    "src/world/level/levelgen/feature/*.cpp"
    "src/world/level/levelgen/synth/*.cpp"
    "src/world/level/material/*.cpp"
    "src/world/level/pathfinder/*.cpp"
    "src/world/level/storage/*.cpp"
    "src/world/level/tile/*.cpp"
    "src/world/level/tile/entity/*.cpp"

    "src/SharedConstants.cpp"
    "src/main.cpp"
    "src/NinecraftApp.cpp"

    "src/AppPlatform_glfw.cpp"
    "src/main.cpp"
)

if (${PLATFORM} STREQUAL "Desktop")
    list(APPEND CLIENT_SOURCES glad/src/glad.c)
endif()

# Server
if(UNIX)
    add_executable("${PROJECT_NAME}-server" ${SERVER_SOURCES})

    target_compile_definitions("${PROJECT_NAME}-server" PUBLIC "STANDALONE_SERVER" "SERVER_PROFILER")

    target_include_directories("${PROJECT_NAME}-server" PUBLIC 
        "${CMAKE_SOURCE_DIR}/src/"
        "project/lib_projects/raknet/jni/RaknetSources"
    )

    target_link_libraries("${PROJECT_NAME}-server" ${CMAKE_THREAD_LIBS_INIT})
endif()

add_executable(${PROJECT_NAME} ${CLIENT_SOURCES})

target_compile_definitions(${PROJECT_NAME} PUBLIC ${PLATFORM_CPP})

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/glad/include/"
    "${CMAKE_SOURCE_DIR}/src"
    "${openal_SOURCE_DIR}/include"
    "${glfw_SOURCE_DIR}/include"
    "lib/include"
)

if(${PLATFORM} MATCHES "Web")
    set(CMAKE_CXX_STANDARD 11)
    # uuuh i hate it
    set(EM_FLAGS "-pthread -sUSE_PTHREADS=1 -sUSE_LIBPNG=1 -sSHARED_MEMORY=1")
    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EM_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EM_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EM_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/data@/data")

    target_compile_options(${PROJECT_NAME} PUBLIC
        "-Os"
        "-Wno-invalid-source-encoding"
        "-Wno-narrowing"
        "-Wno-deprecated-register"
        "-Wno-reserved-user-defined-literal"
    )

    target_link_options(${PROJECT_NAME} PUBLIC
        "-Os"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "-sLEGACY_GL_EMULATION=1"
        "-sGL_UNSAFE_OPTS=0"
        "-sEMULATE_FUNCTION_POINTER_CASTS=1"
        "-sALLOW_TABLE_GROWTH=1"
        "-sEXPORTED_RUNTIME_METHODS=['FS','stringToUTF8','UTF8ToString','cwrap','ccall','HEAP8','HEAPU8','HEAP32','HEAPU32']"
        "-sEXPORTED_FUNCTIONS=['_main']"
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("DEBUG MODE")

        target_link_options(${PROJECT_NAME} PUBLIC
            "-sASSERTIONS=2"
            "-sSTACK_OVERFLOW_CHECK=2"
            "-sSTACK_SIZE=5242880"
            "-sGL_DEBUG=1"
        )
    endif()

    target_compile_definitions(${PROJECT_NAME} PUBLIC "__EMSCRIPTEN__" "NO_SOUND" "NO_NETWORK")
endif()

# Client
target_compile_definitions(${PROJECT_NAME} PUBLIC "OPENGL_ES" "NO_EGL" ${PLATFORM})
target_link_libraries(${PROJECT_NAME} zlib ${PNG_LIB} OpenAL::OpenAL glfw ${EXTRA_LIBS})

if (OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HTTPCLIENT_USE_OPENSSL)
endif()

if (NOT UNIX)
    add_custom_command(
        TARGET ${PROJECT_NAME} 
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> 
            COMMAND_EXPAND_LISTS
    )
endif()

if(NOT ${PLATFORM} MATCHES "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} 
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data" $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
    )
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} 
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/misc/web/index.html" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

message(STATUS "Compiling with the flags:")
message(STATUS "  PLATFORM=" ${PLATFORM_CPP})