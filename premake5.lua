-- toolset "clang"

workspace "Networking"
    architecture "x86_64"
    startproject "Networking"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

-- The output directory based on the configurations"C:/MSVC/VC/Tools/MSVC/14.38.33130/lib/x64",
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}


filter {}

-- Create a solution folder inside visual studio
group "Dependencies"
group "" -- End the solution folder here

project "Networking"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


    files
    {
        "socket/**.h",
        "socket/**.cpp",
        "main.cpp"
    }

    includedirs
    {
        "Socket",
        "libs",
        "C:/MinGW/openssl/include",


    }

    libdirs
	{
		"C:/MinGW/openssl/lib",
        "libs/OpenSSL"

	}

    links
    {

    }

    filter "system:windows"
        systemversion "latest"
        -- prebuildcommands { "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvars64.bat" }
        defines
        {
            "PLATFORM_WINDOWS"
        }
    
    filter "system:linux"
        systemversion "latest"

        defines
        {   
            "PLATFORM_LINUX"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "DIST"
        runtime "Release"
        optimize "on"