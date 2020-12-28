add_executable(testUnit_Cassette testCassette.cpp)
# EmotionalNES ライブラリをリンク
target_link_libraries(testUnit_Cassette EmotionalNES)
target_link_libraries(testUnit_Cassette testUtil)

# TORIAEZU: テスト側は src ディレクトリの全ヘッダをインクルードできるようにしとく
target_include_directories(testUnit_Cassette
  PRIVATE ${PROJECT_SOURCE_DIR}/Programs/src
  )