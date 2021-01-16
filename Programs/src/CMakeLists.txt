# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(EmotionalNES
  STATIC
  cassette.h
  constants.h
  Cpu.h
  Ppu.h
  System.h
  cassette.cpp
  Cpu.cpp
  Ppu.cpp
  System.cpp
  )

# TODO: ライブラリのインクルードディレクトリを設定