workspace "AstralCoreTests"
    configurations { "Debug", "Release" }

    newoption {
        trigger = "clang",
        description = "Utilise the clang compiler toolchain"
    }
    filter "options:clang"
        toolset "clang"
        buildoptions { "-g", "-gcodeview", "-march=native", "-Wno-nonportable-include-path", "-Wno-deprecated-declarations", "-Werror=return-type" }
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

    project "AstralCoreTest"
        kind "ConsoleApp"
        language "C++"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        objdir "obj/%{cfg.buildcfg}"
        includedirs {
            "../Astral.Core",
            "./"
        }
        files {
            "**.cpp"
        }
        defines "FORCE_NO_INTRINSICS"

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"