project "AstralCanvasBindingsGenerator"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include",
        "../Astral.Core",
        "../Astral.Reflect/include"
    }
    files { 
        "*.cpp"
    }
    links "Astral.Reflect"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"