// platform includes
#include <windows.h>

// local includes
#include "ProjectiveTextureWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   ProjectiveTextureWindow * pProjWnd = new ProjectiveTextureWindow;
   pProjWnd->Create(800, 600, "Projective Texture");
   return pProjWnd->Run();
}