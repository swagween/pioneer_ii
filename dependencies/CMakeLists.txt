include(FetchContent)

# SFML
FetchContent_Declare(sfml
  GIT_REPOSITORY https://github.com/SFML/SFML
  #GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
  GIT_TAG 69ea0cd863aed1d4092b970b676924a716ff718b # 2.6.1
)
FetchContent_MakeAvailable(sfml)

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG 35b1148efb839381b84de9290d9caf0b66ad7d03 # 1.82
)

FetchContent_MakeAvailable(imgui)

# ImGui-SFML
FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml
  GIT_TAG 82dc2033e51b8323857c3ae1cf1f458b3a933c35 # 2.3
)
add_subdirectory(imgui-sfml)
