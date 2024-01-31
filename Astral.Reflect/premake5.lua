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
        "src/*.cpp", 
        "../Astral.Json/Json.cpp",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"