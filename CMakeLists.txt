add_executable(mylayout zwx_helper.h mylayout.cpp)
include_directories(
	/usr/lib64/wx/include/gtk3-unicode-3.0
	/usr/include/wx-3.0
)

target_compile_definitions(mylayout PUBLIC -D_FILE_OFFSET_BITS=64 PUBLIC -DWXUSINGDLL PUBLIC -D__WXGTK__ PUBLIC -DwxUSE_GUI)
#target_link_libraries(mylayout -lpthread   -lwx_gtk3u_xrc-3.0 -lwx_gtk3u_webview-3.0 -lwx_gtk3u_html-3.0 -lwx_gtk3u_qa-3.0 -lwx_gtk3u_adv-3.0 -lwx_gtk3u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0)
target_link_libraries(mylayout -lpthread -lwx_gtk3u_core-3.0 -lwx_baseu-3.0)
