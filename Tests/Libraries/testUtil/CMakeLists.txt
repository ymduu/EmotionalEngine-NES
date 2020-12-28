# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(testUtil
  STATIC
    ${PROJECT_SOURCE_DIR}/Tests/Includes/testUtil.h
    testUtil.cpp
  )

target_include_directories(testUtil
  PUBLIC ${PROJECT_SOURCE_DIR}/Tests/Includes
  )