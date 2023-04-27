add_library(file_dialog
  STATIC
  ./file_dialog.cpp
)
target_link_libraries(file_dialog
  PRIVATE
  imgui
)