workspace "AstralCoreTests"
    configurations { "Debug", "Release" }
    newoption {
        trigger = "clang",
        description = "Utilise the clang compiler toolchain"
    }
    newoption {
        trigger = "no-simd",
        description = "Force disable usage of SIMD instructions"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "RELEASE" }
        optimize "On"

    filter "system:windows"
        architecture "x86_64"
        defines "x86_64"
    
    filter "options:clang"
        toolset "clang"
        buildoptions { "-fpermissive", "-g", "-gcodeview" }
        linkoptions { "-fuse-ld=lld", "-g" }
    filter "options:no-simd"
        defines "FORCE_NO_INTRINSICS"

    include("SIMD")