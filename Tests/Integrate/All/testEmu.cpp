#include "DxLib.h"

// ÉvÉçÉOÉâÉÄÇÕ WinMain Ç©ÇÁénÇ‹ÇËÇ‹Ç∑
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    ChangeWindowMode(TRUE);
    if( DxLib_Init() == -1) {
        return -1 ;
    }
    DrawBox(0, 0, 200, 100, (int) GetColor(255, 0, 0), TRUE);
    WaitKey();
    DxLib_End();
    return 0;
}