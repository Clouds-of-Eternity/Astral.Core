workspace "AstralCoreTests"
    configurations { "Debug", "Release" }

    newoption {
        trigger = "clang",
        description = "Utilise the clang compiler toolchain"
    }
    filter "options:clang"
        toolset "clang"
        buildoptions { "-g", "-gcodeview", "-Wno-deprecated-declarations", "-Werror=return-type" }
        linkoptions { "-fuse-ld=lld", "-g" }

    filter "system:windows"
        defines { "WINDOWS" }
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX" }
        architecture "x86_64"

    filter "system:macosx"
        defines { "MACOS", "POSIX" }
        system "macosx"

    project "AstralBaseTest"
        kind "ConsoleApp"
        language "C"
        cdialect "C11"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        objdir "obj/%{cfg.buildcfg}"
        includedirs {
            "../Astral.Base"
        }
        files {
            "**.c"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
