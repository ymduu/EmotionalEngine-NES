# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(testUtil
  STATIC
    ${PROJECT_SOURCE_DIR}/Tests/Includes/testUtil.h
    testUtil.cpp
  )

# テストライブラリは実装のすべてのヘッダをインクルードできる
target_include_directories(testUtil
  PUBLIC ${PROJECT_SOURCE_DIR}/Tests/Includes
  PUBLIC ${PROJECT_SOURCE_DIR}/Programs/src
  )