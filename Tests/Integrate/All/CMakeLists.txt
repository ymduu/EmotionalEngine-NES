add_executable(testIntegrate_All WIN32 testEmu.cpp)
# EmotionalNES ライブラリをリンク
target_link_libraries(testIntegrate_All EmotionalNES)
target_link_libraries(testIntegrate_All testUtil)

# DxLib リンク
target_link_libraries(
    testIntegrate_All
    DxLib
    DxUseCLib
    DxDrawFunc
    libjpeg
    libpng
    zlib
    libtiff
    libtheora_static
    vorbis_static
    vorbisfile_static
    ogg_static
    # よくわかんないけどエミュのフロントエンドでは多分つかわないだろ……
    #libbulletdynamics
    #libbulletcollision
    #libbulletmath
)

# TORIAEZU: テスト側は src ディレクトリの全ヘッダをインクルードできるようにしとく
target_include_directories(testIntegrate_System
  PRIVATE ${PROJECT_SOURCE_DIR}/Programs/src
  PRIVATE ${PROJECT_SOURCE_DIR}/Tests/Includes
  )

