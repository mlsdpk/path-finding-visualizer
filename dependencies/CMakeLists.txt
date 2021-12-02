include(FetchContent)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY "https://github.com/SFML/SFML"
  GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab # 2.5.1
)

add_subdirectory(sfml)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  # GIT_TAG 55d35d8387c15bf0cfd71861df67af8cfbda7456
  GIT_TAG 719d9313041b85227a3e6deb289a313819aaeab3 # latest commit of docking-branch
)

FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml
  GIT_TAG 82dc2033e51b8323857c3ae1cf1f458b3a933c35
)

FetchContent_MakeAvailable(imgui)
add_subdirectory(imgui-sfml)
