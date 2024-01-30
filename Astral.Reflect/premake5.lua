project "Astral.Reflect"
    kind "StaticLib"
    language "C++"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include",
        "../Astral.Core",
        "../Astral.Json"
    }
    files { 
        "*.cpp", 
        "include/*.hpp", 
        "../Astral.Json/Json.cpp",
        "../Astral.Json/Json.hpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"