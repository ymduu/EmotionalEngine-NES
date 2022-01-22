# TORIAEZU: 雑に全部リンクしちゃう
# 辞書順に配置すること
add_library(EmotionalNES
  STATIC
  Apu.h
  cassette.h
  constants.h
  Cpu.h
  Pad.h
  Ppu.h
  System.h
  ${PROJECT_SOURCE_DIR}/Include/Nes.h
  ${PROJECT_SOURCE_DIR}/Include/Nes_Pad.h
  Apu.cpp
  cassette.cpp
  Cpu.cpp
  Nes.cpp
  Pad.cpp
  Ppu.cpp
  System.cpp
  )

target_include_directories(EmotionalNES
  PUBLIC ${PROJECT_SOURCE_DIR}/Include
  PRIVATE ${PROJECT_SOURCE_DIR}/Programs/src/
  )