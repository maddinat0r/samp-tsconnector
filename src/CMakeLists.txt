include(AMXConfig)
include(AddSAMPPlugin)

set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/sdk")
find_package(SAMPSDK REQUIRED)

set(TSC_PLUGIN_CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/config_headers")
include_directories("${SAMPSDK_INCLUDE_DIR}" "${TSC_PLUGIN_CONFIG_INCLUDE_DIR}")

set(CMAKE_DEBUG_POSTFIX "_d")

configure_file(
	"../TSConnector.inc.in"
	"TSConnector.inc"
	@ONLY
)

configure_file(
	"version.hpp.in"
	"${TSC_PLUGIN_CONFIG_INCLUDE_DIR}/version.hpp"
	@ONLY
)


add_samp_plugin(teamspeak-connector 
	${SAMPSDK_DIR}/amxplugin.cpp
	${SAMPSDK_DIR}/amxplugin2.cpp
	${SAMPSDK_DIR}/amx/getch.c
	CCallback.cpp
	CCallback.hpp
	CNetwork.cpp
	CNetwork.hpp
	CServer.cpp
	CServer.hpp
	CUtils.cpp
	CUtils.hpp
	main.cpp
	main.hpp
	natives.cpp
	natives.hpp
	format.cc
	plugin.def
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WIN32_WINNT=0x0600)
endif()

if(NOT MSVC) #we have auto-linking in Visual Studio
	target_link_libraries(teamspeak-connector
		"${Boost_ATOMIC_LIBRARY}"
		"${Boost_CHRONO_LIBRARY}"
		"${Boost_DATE_TIME_LIBRARY}"
		"${Boost_REGEX_LIBRARY}"
		"${Boost_SYSTEM_LIBRARY}"
		"${Boost_THREAD_LIBRARY}"
	)
endif()


install(TARGETS teamspeak-connector DESTINATION "plugins/")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/TSConnector.inc" DESTINATION "pawno/include/")

set(CPACK_PACKAGE_VERSION "v${TSC_PLUGIN_VERSION}")
if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
