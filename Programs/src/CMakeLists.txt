# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(EmotionalNES
  STATIC
  cassette.h
  constants.h
  Cpu.h
  Ppu.h
  System.h
  ${PROJECT_SOURCE_DIR}/Include/Nes.h
  cassette.cpp
  Cpu.cpp
  Nes.cpp
  Ppu.cpp
  System.cpp
  )

target_include_directories(EmotionalNES
  PUBLIC ${PROJECT_SOURCE_DIR}/Include
  PRIVATE ${PROJECT_SOURCE_DIR}/Programs/src/
  )