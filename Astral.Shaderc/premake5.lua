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
        "../Astral.Json/Json.cpp"
    }

    links { "%{VULKAN_SDK}/Lib/shaderc_combined.lib", "%{VULKAN_SDK}/Lib/spirv-cross-c-shared.lib", "Astral.Reflect" }

    filter "system:windows"
        links "dependencies/win-x64/native/spirv-cross.dll"

    filter "system:macosx"
        links "dependencies/osx-x64/native/libspirv-cross.dylib"

    filter "system:linux"
        links "dependencies/linux-x64/native/libspirv-cross.so"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"