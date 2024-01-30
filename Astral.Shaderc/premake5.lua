project "Astral.Shaderc"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "dependencies/include",
        "../Astral.Core",
        "../Astral.Json",
        "../Astral.Reflect/include"
    }

    files { 
        "*.cpp", 
        "*.h", 
        "../Astral.Reflect/include/*.hpp",
        "../Astral.Json/Json.cpp",
        "../Astral.Json/Json.hpp"
    }
    links { "dependencies/shaderc_combined.lib", "dependencies/spirv-cross-c-shared.lib", "Astral.Reflect" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"