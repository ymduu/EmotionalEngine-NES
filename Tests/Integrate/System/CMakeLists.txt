add_executable(testIntegrate_System testSystem.cpp)
# EmotionalNES ライブラリをリンク
target_link_libraries(testIntegrate_System EmotionalNES)
target_link_libraries(testIntegrate_System testUtil)

# TORIAEZU: テスト側は src ディレクトリの全ヘッダをインクルードできるようにしとく
target_include_directories(testIntegrate_System
  PRIVATE ${PROJECT_SOURCE_DIR}/Programs/src
  )