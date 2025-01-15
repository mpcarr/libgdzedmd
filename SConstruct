#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["gd_zedmd/", "libzedmd/third-party/include", "libzedmd/third-party/include/miniz"])
env.Append(LIBS=["libzedmd/third-party/build-libs/win/x64/libserialport64.lib", "ws2_32"])

sources = Glob("gd_zedmd/*.cpp") + Glob("libzedmd/**/*.cpp") + Glob("libzedmd/third-party/include/miniz/*.c")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "build/bin/libgdzedmd.{}.{}.framework/libgdzedmd.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "build/bin/libgdzedmd.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "build/bin/libgdzedmd.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "build/bin/libgdzedmd{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
