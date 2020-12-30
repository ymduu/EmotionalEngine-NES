# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(EmotionalNES
  STATIC
  cassette.h
  constants.h
  Cpu.h
  System.h
  cassette.cpp
  Cpu.cpp
  System.cpp
  )

# TODO: ライブラリのインクルードディレクトリを設定