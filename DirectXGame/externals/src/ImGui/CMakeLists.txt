cmake_minimum_required(VERSION 3.20)
project(imgui LANGUAGES CXX)

# ImGui コアソース
set(IMGUI_SRC
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

# Docking / Viewport 用（demo）
set(IMGUI_DOCKING_SRC
    imgui_demo.cpp
)

# バックエンド（Win32 + DirectX12）
set(IMGUI_BACKEND
    backends/imgui_impl_win32.cpp
    backends/imgui_impl_dx12.cpp
)

add_library(imgui STATIC
    ${IMGUI_SRC}
    ${IMGUI_DOCKING_SRC}
    ${IMGUI_BACKEND}
)

# インクルードパス
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Docking / Viewport 有効化
target_compile_definitions(imgui PUBLIC
    IMGUI_ENABLE_DOCKING
    IMGUI_ENABLE_VIEWPORTS
)

# Windows / DirectX12 必須ライブラリ
target_link_libraries(imgui PUBLIC
    d3d12
    dxgi
    dxguid
)

# /MT / /MTd は外部（build_libs.bat）で制御
# CMAKE_MSVC_RUNTIME_LIBRARY を想定

# インストール設定
install(TARGETS imgui
    ARCHIVE DESTINATION lib
)

install(DIRECTORY .
    DESTINATION include/imgui
    FILES_MATCHING PATTERN "*.h"
)
