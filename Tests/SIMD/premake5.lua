project "SIMD"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++14"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    rtti "Off"
    includedirs {
        "../../Astral.Core"
    }

    files {
        "Program.cpp"
    }