workspace "OpenglDemo"
	architecture "x86_64"

	configurations  
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["Glm"] = "vendor/glm"
IncludeDir["StbImage"] = "vendor/stb_image"
IncludeDir["Opencv"] = "F:/00_MY/00_Vendors/opencv/opencv480/include"
IncludeDir["Eigen"] = "F:/00_MY/00_Vendors/eigen/include" 
IncludeDir["XIMEA"] = "F:/00_MY/00_Vendors/ximea/API/xiAPI"
IncludeDir["Pcap"] = "F:/00_MY/00_Vendors/pcap/npcap-sdk-1.13/Include"

include "vendor/GLFW"
include "vendor/Glad"
include "vendor/imgui"

project "OpenglDemo"
	location "./"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}") 

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/stb_image.h",
		"vendor/stb_image/stb_image.cpp"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"src/ImGui",
		"src/net",
		"src/tracking",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.StbImage}",
		"%{IncludeDir.Opencv}",
		"%{IncludeDir.Eigen}",
		"%{IncludeDir.XIMEA}",
		"%{IncludeDir.Pcap}"
	}

    -- 添加OpenCV的链接库
    libdirs { "F:/00_MY/00_Vendors/opencv/opencv480/x64/vc17/lib" }
	-- 添加XIMEA的链接库
    libdirs { "F:/00_MY/00_Vendors/ximea/API/xiAPI" }
	-- 添加Pcap的链接库
    libdirs { "F:/00_MY/00_Vendors/pcap/npcap-sdk-1.13/Lib/x64" }

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"xiapi64.lib",
		"Packet.lib",
		"wpcap.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		staticruntime "off"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		buildoptions "/MDd"
		symbols "On"
		links
		{
			"opencv_world480d.lib"
		}

	filter "configurations:Release"
		defines "HZ_RELEASE"
		buildoptions "/MD"
		optimize "On"
		links
		{
			"opencv_world480.lib"
		}

	filter "configurations:Dist"
		defines "HZ_DIST"
		buildoptions "/MD"
		optimize "On"
		links
		{
			"opencv_world480.lib"
		}