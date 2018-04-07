#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void App_Main();

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    App_Main();
    return 0;
}

int main(void) {
    App_Main();
    return 0;
}