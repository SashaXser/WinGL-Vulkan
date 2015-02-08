// includes
#include "QuickGLWindow.h"
#include "QuickSoundManager.h"
#include "KeyboardInput.h"
#include "ImageLibrary.h"
#include "Camera.h"
#include "CFont.h"
#include "Timer.h"

#include <gl/gl.h>

#include <time.h>
#include <stdlib.h>
#include <memory.h>

#include <fstream>
#include <map>
#include <vector>

// global enumerations
enum GameStates
{
   GAME_STATES_PRESS_SPACE,
   GAME_STATES_PLAYING_GAME,
   GAME_STATES_GAME_OVER,
   GAME_STATES_FLASH_REMOVED_SQUARES,
   GAME_STATES_ENTER_NAME,
   GAME_STATES_SHOW_WINNERS,
   GAME_STATES_TITLE_SCREEN,
   GAME_STATES_MAX
};

enum BoardDimensions
{
   BOARD_DIM_ROWS = 15,
   BOARD_DIM_COLUMNS = 7,
   BOARD_DIM_MAX_LINE_SIZE = 3,
   BOARD_DIM_LEFT_MOST_COLUMN = -2,
   BOARD_DIM_RIGHT_MOST_COLUMN = 6
};

enum ColumnDirection
{
   COLUMN_DIRECTION_LEFT,
   COLUMN_DIRECTION_RIGHT
};

// global structures
struct UpdateDrawCB
{
   void (* DrawCB)( void *, void *, const double & );
   void * pDrawData;
   void (* UpdateCB)( void *, void *, const double & );
   void * pUpdateData;
};

struct TitleScreenUpdateDrawData
{
   unsigned int nTitleStream;
};

struct PressSpaceUpdateDrawData
{
   unsigned int * pFirstRow;
   unsigned int * pCurrentRow;
   unsigned int * pLastRow;
   double dUpdateTime;
};

struct ShowWinnersUpdateDrawData : public PressSpaceUpdateDrawData
{
   unsigned int * pData;
};

struct PlayGameUpdateDrawData
{
   bool * pLastRow;
   bool * pFirstRow;
   bool * pCurrentRow;
   int nCurrentColumn;
   ColumnDirection eColumnDir;
   bool pGameRowLine[BOARD_DIM_MAX_LINE_SIZE];
   bool pGameBoard[BOARD_DIM_ROWS][BOARD_DIM_COLUMNS];
   double dUpdateTimes[BOARD_DIM_ROWS];
   double dUpdateTime;
   double dStartTime;
   unsigned int nGameStream;
};

struct GameOverUpdateDrawData
{
   double dUpdateTime;
   bool (* pTopRow)[BOARD_DIM_COLUMNS];
   bool (* pCurrentRow)[BOARD_DIM_COLUMNS];
   bool (* pNextFallingRow)[BOARD_DIM_COLUMNS];
   bool (* pGameBoard)[BOARD_DIM_COLUMNS];
   PressSpaceUpdateDrawData * pPressSpaceUpdateDrawData;
   PlayGameUpdateDrawData * pPlayGameUpdateDrawData;
   ShowWinnersUpdateDrawData * pShowWinnersUpdateDrawData;
   TitleScreenUpdateDrawData * pTitleScreenUpdateDrawData;
};

struct FlashRemovedSquaresUpdateDrawData
{
   int nFlashCount;
   bool bIsGameOver;
   bool bDoFallingAnim;
   double dUpdateTime;
   bool * pFallingSquares[BOARD_DIM_MAX_LINE_SIZE];
   bool pGameBoard[BOARD_DIM_ROWS][BOARD_DIM_COLUMNS];
   GameOverUpdateDrawData * pGameOverUpdateDrawData;
};

struct GameOverWinnerDrawUpdateData
{
   bool bUpdateText;
   CFont * pFont;
   char pInitials[4];
   double dGameTime;
};

struct UpdateDrawDataUpdatePass
{
   GameOverUpdateDrawData * pGameOverUpdateDrawData;
   PlayGameUpdateDrawData * pPlayGameUpdateDrawData;
   GameOverWinnerDrawUpdateData * pGameOverWinnerDrawUpdateData;
   FlashRemovedSquaresUpdateDrawData * pFlashRemovedSquaresUpdateDrawData;
};

// quick gl window function declarations
void UpdateCB( const double & rElapsedTime );
void DrawCB( void * pVoid, const double & rElapsedTime );
void ShutdownCB( void * pVoid );

void KeyboardCB( void * pVoid,
                 const QuickKeyboard::KeyboardInput * pInput,
                 const double & rElapsedTime );
void EnterInitialsKeyboardCB( void * pVoid,
                              const QuickKeyboard::KeyboardInput * pInput,
                              const double & rElapsedTime );

// rendering routines
void RenderSquare( unsigned int nRow,
                   unsigned int nColumn,
                   bool bFilledIn );

// loads / resets game data for each stage
void LoadPressSpaceText( PressSpaceUpdateDrawData & oData );
void LoadPlayingGameData( PlayGameUpdateDrawData & oData );
void LoadWinnersBuffer( ShowWinnersUpdateDrawData & oData );

// global function declartions
// draw callbacks
void DrawPressSpaceCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawPlayingGameCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawFlashRemovedSquaresCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawGameOverCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawEnterNameCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawShowWinnersCB( void * pWnd, void * pData, const double & rElapsedTime );
void DrawTitleScreenCB( void * pWnd, void * pData, const double & rElapsedTime );

// update callbacks
void UpdatePressSpaceCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdatePlayingGameCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdateFlashRemovedSquaresCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdateGameOverCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdateEnterNameCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdateShowWinnersCB( void * pWnd, void * pData, const double & rElapsedTime );
void UpdateTitleScreenCB( void * pWnd, void * pData, const double & rElapsedTime );

// global variables
// allow the window to be global for the update phase
QuickGLWindow * g_pQGLWindow = NULL;

// defines the current game state
GameStates g_eGameStatesCurrent = GAME_STATES_TITLE_SCREEN;

// defines the draw and update callbacks for the game states
UpdateDrawCB g_pUpdateDrawCB[GAME_STATES_MAX] =
{
   { &DrawPressSpaceCB, NULL, &UpdatePressSpaceCB, NULL },
   { &DrawPlayingGameCB, NULL, &UpdatePlayingGameCB, NULL },
   { &DrawGameOverCB, NULL, &UpdateGameOverCB, NULL },
   { &DrawFlashRemovedSquaresCB, NULL, &UpdateFlashRemovedSquaresCB, NULL },
   { &DrawEnterNameCB, NULL, &UpdateEnterNameCB, NULL },
   { &DrawShowWinnersCB, NULL, &UpdateShowWinnersCB, NULL },
   { &DrawTitleScreenCB, NULL, &UpdateTitleScreenCB, NULL }
};

// defines windows boarders
const int g_nWidth = 479;
const int g_nHeight = 1024;

//const int g_nWidth = 1024;
//const int g_nHeight = 768;

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevious,
                    LPSTR lpCmdLine,
                    int nCmdLine )
{
   // setup the sleep resolution
   TIMECAPS timeCaps;
   timeGetDevCaps(&timeCaps, sizeof(timeCaps));
   timeBeginPeriod(timeCaps.wPeriodMin);

   // create the gl window
   g_pQGLWindow = new QuickGLWindow(hInstance,
                                    g_nWidth, g_nHeight,
                                    "Tower",
                                    false);

   // initialize the sound manager
   QuickSoundManager & rQSM = QuickSoundManager::Instance();
   rQSM.Initialize(48000, 16, g_pQGLWindow->GetWindow());

   // initialize the sound samples
   const unsigned int nTitleStream = rQSM.LoadStream("./TitleScreen.wav", true);
   const unsigned int nPlayGameStream = rQSM.LoadStream("./PlayingGame.wav", true);
   const unsigned int nWinnerStream = rQSM.LoadStream("./WinnerScreen.wav", true);
   rQSM.PlayStream(nTitleStream);
   //rQSM.PlaySnd(nTitleStream);

   // modify the update and draw callback data
   // create and update draw data store for the press space screen
   PressSpaceUpdateDrawData oPressSpaceUpdateDrawData;
   LoadPressSpaceText(oPressSpaceUpdateDrawData);

   g_pUpdateDrawCB[GAME_STATES_PRESS_SPACE].pDrawData = &oPressSpaceUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_PRESS_SPACE].pUpdateData = &oPressSpaceUpdateDrawData;

   // create and update draw data store for the actual game
   PlayGameUpdateDrawData oPlayGameUpdateDrawData;
   LoadPlayingGameData(oPlayGameUpdateDrawData);
   oPlayGameUpdateDrawData.nGameStream = nPlayGameStream;

   g_pUpdateDrawCB[GAME_STATES_PLAYING_GAME].pDrawData = &oPlayGameUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_PLAYING_GAME].pUpdateData = &oPlayGameUpdateDrawData;

   // create and update draw data store for showing winners
   ShowWinnersUpdateDrawData oShowWinnersUpdateDrawData;
   oShowWinnersUpdateDrawData.pData = 0;
   LoadWinnersBuffer(oShowWinnersUpdateDrawData);

   g_pUpdateDrawCB[GAME_STATES_SHOW_WINNERS].pDrawData = &oShowWinnersUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_SHOW_WINNERS].pUpdateData = &oShowWinnersUpdateDrawData;

   // create and update draw data store for title screen
   TitleScreenUpdateDrawData oTitleScreenUpdateDrawData;
   oTitleScreenUpdateDrawData.nTitleStream = nTitleStream;

   g_pUpdateDrawCB[GAME_STATES_TITLE_SCREEN].pDrawData = &oTitleScreenUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_TITLE_SCREEN].pUpdateData = &oTitleScreenUpdateDrawData;

   // create and update draw data store for game over
   GameOverUpdateDrawData oGameOverUpdateDrawData;
   oGameOverUpdateDrawData.dUpdateTime = 0.0;
   oGameOverUpdateDrawData.pTopRow = NULL;
   oGameOverUpdateDrawData.pGameBoard = NULL;
   oGameOverUpdateDrawData.pCurrentRow = NULL;
   oGameOverUpdateDrawData.pNextFallingRow = NULL;
   oGameOverUpdateDrawData.pPressSpaceUpdateDrawData = &oPressSpaceUpdateDrawData;
   oGameOverUpdateDrawData.pPlayGameUpdateDrawData = &oPlayGameUpdateDrawData;
   oGameOverUpdateDrawData.pShowWinnersUpdateDrawData = &oShowWinnersUpdateDrawData;
   oGameOverUpdateDrawData.pTitleScreenUpdateDrawData = &oTitleScreenUpdateDrawData;

   g_pUpdateDrawCB[GAME_STATES_GAME_OVER].pDrawData = &oGameOverUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_GAME_OVER].pUpdateData = &oGameOverUpdateDrawData;

   // create and update draw data store for removing squares
   FlashRemovedSquaresUpdateDrawData oFlashRemovedSquaresUpdateDrawData;
   oFlashRemovedSquaresUpdateDrawData.pGameOverUpdateDrawData = &oGameOverUpdateDrawData;
   
   g_pUpdateDrawCB[GAME_STATES_FLASH_REMOVED_SQUARES].pDrawData = &oFlashRemovedSquaresUpdateDrawData;
   g_pUpdateDrawCB[GAME_STATES_FLASH_REMOVED_SQUARES].pUpdateData = &oFlashRemovedSquaresUpdateDrawData;

   // create and update draw data store for entering initials
   GameOverWinnerDrawUpdateData oGameOverWinnerDrawUpdateData;
   oGameOverWinnerDrawUpdateData.bUpdateText = true;
   oGameOverWinnerDrawUpdateData.pInitials[0] = '\0';
   oGameOverWinnerDrawUpdateData.pInitials[1] = '\0';
   oGameOverWinnerDrawUpdateData.pInitials[2] = '\0';
   oGameOverWinnerDrawUpdateData.pInitials[3] = '\0';
   oGameOverWinnerDrawUpdateData.pFont = new CFont(0,
                                                   Vector(0.0f, 0.0f, 0.0f),
                                                   ".\\Font.tga",
                                                   20, 25,
                                                   "",
                                                   1.0f,
                                                   CFont::ALIGN_CENTER);

   g_pUpdateDrawCB[GAME_STATES_ENTER_NAME].pDrawData = &oGameOverWinnerDrawUpdateData;
   g_pUpdateDrawCB[GAME_STATES_ENTER_NAME].pUpdateData = &oGameOverWinnerDrawUpdateData;

   // create an update draw callback object
   UpdateDrawDataUpdatePass oUpdateDrawDataUpdatePass;
   oUpdateDrawDataUpdatePass.pGameOverUpdateDrawData = &oGameOverUpdateDrawData;
   oUpdateDrawDataUpdatePass.pPlayGameUpdateDrawData = &oPlayGameUpdateDrawData;
   oUpdateDrawDataUpdatePass.pGameOverWinnerDrawUpdateData = &oGameOverWinnerDrawUpdateData;
   oUpdateDrawDataUpdatePass.pFlashRemovedSquaresUpdateDrawData = &oFlashRemovedSquaresUpdateDrawData;

   // setup callbacks
   g_pQGLWindow->SetUpdateCallback(UpdateCB);
   g_pQGLWindow->SetDrawCallback(g_pQGLWindow, DrawCB);
   g_pQGLWindow->SetKeyboardCallback(&oUpdateDrawDataUpdatePass, KeyboardCB);
   g_pQGLWindow->SetShutdownCallback(g_pQGLWindow, ShutdownCB);

   // setup the ortho params
   g_pQGLWindow->GetCamera().SetOrthographic(0.0f, static_cast< float >(g_nWidth),
                                             0.0f, static_cast< float >(g_nHeight),
                                             -1.0f, 1.0f);

   // setup the lookat params
   g_pQGLWindow->GetCamera().SetCamera(Vector(0.0f, 0.0f, 0.0f),
                                       Vector(0.0f, 0.0f, -1.0f),
                                       Vector(0.0f, 1.0f, 0.0f));

   // setup keyboard bindings
   g_pQGLWindow->SetKeyboardKey(DIK_SPACE);
   g_pQGLWindow->SetKeyboardKey(DIK_ESCAPE);

   // set the mode to typewriter so the input only comes back as one key
   g_pQGLWindow->SetKeyboardMode(QuickKeyboard::TYPEWRITER_MODE);

   // set the alpha function
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // begin the main application
   g_pQGLWindow->Run();

   // release all the sounds
   rQSM.Uninitialize();

   // release the font
   delete oGameOverWinnerDrawUpdateData.pFont;

   // release the application
   delete g_pQGLWindow;

   // release the increased sleep resolution
   timeEndPeriod(timeCaps.wPeriodMin);

   return 0;
}

void DrawCB( void * pVoid, const double & rElapsedTime )
{
   static Timer sTimer;
   static const double sUpdateFreqInMS = 1000.0 / 120.0;

   // determine the sleep amount
   double dSleep = sUpdateFreqInMS - (sTimer.ElapsedTime() * 1000.0);

   if (dSleep > 0.0)
   {
      // zzz...
      Sleep(static_cast< unsigned long >(dSleep)); 
   }

   // call the elapsed time function to reset the value
   sTimer.ElapsedTime();

   UpdateDrawCB * pUpdateDrawCB = g_pUpdateDrawCB + g_eGameStatesCurrent;

   pUpdateDrawCB->DrawCB(pVoid, pUpdateDrawCB->pDrawData, rElapsedTime);
}

void UpdateCB( const double & rElapsedTime )
{
   UpdateDrawCB * pUpdateDrawCB = g_pUpdateDrawCB + g_eGameStatesCurrent;

   pUpdateDrawCB->UpdateCB(g_pQGLWindow, pUpdateDrawCB->pUpdateData, rElapsedTime);
}

void ShutdownCB( void * pVoid )
{
}

void KeyboardCB( void * pVoid,
                 const QuickKeyboard::KeyboardInput * pInput,
                 const double & rElapsedTime )
{
   typedef QuickKeyboard::KeyboardInput::const_iterator ConstKeyIterator;

   for (ConstKeyIterator itBeg = pInput->begin(),
                         itEnd = pInput->end();
        itBeg != itEnd;
        itBeg++)
   {
      switch (*itBeg)
      {
      case DIK_SPACE:
         if (g_eGameStatesCurrent == GAME_STATES_TITLE_SCREEN ||
             g_eGameStatesCurrent == GAME_STATES_PRESS_SPACE ||
             g_eGameStatesCurrent == GAME_STATES_SHOW_WINNERS)
         {
            // remove all the keys
            g_pQGLWindow->GetKeyboard().RemoveAllKeys();

            // add back only gameplay keys
            g_pQGLWindow->GetKeyboard().AddKey(DIK_SPACE);
            g_pQGLWindow->GetKeyboard().AddKey(DIK_ESCAPE);

            // transition the state
            g_eGameStatesCurrent = GAME_STATES_PLAYING_GAME;

            // obtain the update / draw data for this pass
            UpdateDrawDataUpdatePass * pUpdateData =
               static_cast< UpdateDrawDataUpdatePass * >(pVoid);

            // stop the title screen stream
            QuickSoundManager::Instance().StopStream(
               pUpdateData->pGameOverUpdateDrawData->pTitleScreenUpdateDrawData->nTitleStream);

            // begin playing the main game stream
            QuickSoundManager::Instance().PlayStream(
               pUpdateData->pPlayGameUpdateDrawData->nGameStream);

            // determine the absolute time
            pUpdateData->pPlayGameUpdateDrawData->dStartTime = Timer().AbsoluteTime();
         }
         else if (g_eGameStatesCurrent == GAME_STATES_PLAYING_GAME)
         {
            // obtain the data
            UpdateDrawDataUpdatePass * pUpdateData = static_cast< UpdateDrawDataUpdatePass * >(pVoid);
            // obtain the game data
            PlayGameUpdateDrawData * pGameData = pUpdateData->pPlayGameUpdateDrawData;
            GameOverUpdateDrawData * pGameOverData = pUpdateData->pGameOverUpdateDrawData;
            GameOverWinnerDrawUpdateData * pGameOverWinnerData = pUpdateData->pGameOverWinnerDrawUpdateData;
            FlashRemovedSquaresUpdateDrawData * pFlashData = pUpdateData->pFlashRemovedSquaresUpdateDrawData;

            // obtain the current row
            const unsigned int rowIndex =
               (pGameData->pCurrentRow - pGameData->pFirstRow) / BOARD_DIM_COLUMNS;

            // the user automatically looses blocks that are off the screen
            switch (pGameData->nCurrentColumn)
            {
            case 6:
            case -2:
               pGameData->pGameRowLine[2] = false;
               pGameData->pGameRowLine[1] = false;

               break;

            case 5:
               if (pGameData->pGameRowLine[2] == false) break;

            case -1:
               if (pGameData->pGameRowLine[2])
               {
                  pGameData->pGameRowLine[2] = false;
               }
               else
               {
                  pGameData->pGameRowLine[1] = false;
               }

               break;
            }

            if (rowIndex)
            {
               // determine how many columns are currently in play
               const unsigned int nColumnsInPlay = pGameData->pGameRowLine[2] ? 3 :
                                                   pGameData->pGameRowLine[1] ? 2 : 1;
               
               // determine the beginning location of the columns
               const unsigned int nCurrentColumn =
                  pGameData->nCurrentColumn >= 0 ? pGameData->nCurrentColumn : 0;

               // setup the flash data for the falling squares
               pFlashData->nFlashCount = 5;
               pFlashData->bIsGameOver = false;
               pFlashData->bDoFallingAnim = true;
               pFlashData->dUpdateTime = 0.2;
               memset(pFlashData->pFallingSquares, 0x00, sizeof(pFlashData->pFallingSquares));
               memcpy(pFlashData->pGameBoard, pGameData->pGameBoard, sizeof(pFlashData->pGameBoard));

               for (unsigned int column = nCurrentColumn;
                    nCurrentColumn + nColumnsInPlay > column;
                    column++)
               {
                  if (!pGameData->pGameBoard[rowIndex - 1][column])
                  {
                     pGameData->pGameBoard[rowIndex][column] = false;

                     if (pGameData->pGameRowLine[2]) pGameData->pGameRowLine[2] = false;
                     else if (pGameData->pGameRowLine[1]) pGameData->pGameRowLine[1] = false;
                     else pFlashData->bIsGameOver = true;

                     if (!pFlashData->pFallingSquares[0])
                     {
                        pFlashData->pFallingSquares[0] = &pFlashData->pGameBoard[rowIndex][column];
                     }
                     else if (!pFlashData->pFallingSquares[1])
                     {
                        pFlashData->pFallingSquares[1] = &pFlashData->pGameBoard[rowIndex][column];
                     }
                     else
                     {
                        pFlashData->pFallingSquares[2] = &pFlashData->pGameBoard[rowIndex][column];
                     }
                  }
               }

               if (pFlashData->pFallingSquares[0])
               {
                  // switch game modes
                  g_eGameStatesCurrent = GAME_STATES_FLASH_REMOVED_SQUARES;
               }
               else if (rowIndex + 1 >= BOARD_DIM_ROWS)
               {
                  // remove and add all the keys
                  g_pQGLWindow->GetKeyboard().RemoveAllKeys();
                  g_pQGLWindow->GetKeyboard().AddAllKeys();

                  // point to a new keyboard callback
                  g_pQGLWindow->SetKeyboardCallback(pVoid, EnterInitialsKeyboardCB);

                  // game is over, and you are a winner
                  pGameOverWinnerData->bUpdateText = true;
                  pGameOverWinnerData->pInitials[0] = '\0';
                  pGameOverWinnerData->pInitials[1] = '\0';
                  pGameOverWinnerData->pInitials[2] = '\0';
                  pGameOverWinnerData->pInitials[3] = '\0';

                  // setup the game over state
                  pGameOverData->dUpdateTime = 0.0;
                  pGameOverData->pGameBoard = pFlashData->pGameBoard;
                  pGameOverData->pCurrentRow = pFlashData->pGameBoard;
                  pGameOverData->pNextFallingRow = pFlashData->pGameBoard + 1;
                  pGameOverData->pTopRow = pFlashData->pGameBoard + BOARD_DIM_ROWS - 1;

                  // transition to the game over state
                  g_eGameStatesCurrent = GAME_STATES_ENTER_NAME;

                  // obtain the finish time
                  pUpdateData->pGameOverWinnerDrawUpdateData->dGameTime =
                     Timer().AbsoluteTime() - pUpdateData->pPlayGameUpdateDrawData->dStartTime;
               }
            }

            // move the row up a level
            pGameData->pCurrentRow = pGameData->pGameBoard[rowIndex + 1];

            // set the time to zero for an automatic update
            pGameData->dUpdateTime = 0.0;

            // randomize the current column location
            pGameData->nCurrentColumn = rand() % 5;

            // make sure that we remove blocks at certain levels
            if (rowIndex == 4)
            {
               pGameData->pGameRowLine[2] = false;
            }
            else if (rowIndex == 9)
            {
               pGameData->pGameRowLine[1] = false;
            }
         }

         return;

      case DIK_ESCAPE:
         // shutdown the app
         g_pQGLWindow->ShutdownApp();

         return;
      }
   }
}

void EnterInitialsKeyboardCB( void * pVoid,
                              const QuickKeyboard::KeyboardInput * pInput,
                              const double & rElapsedTime )
{
#define ADD_CHAR( c ) case DIK_##c: if (pChar) { *pChar = #@c; pData->bUpdateText = true; } break;

   typedef QuickKeyboard::KeyboardInput::const_iterator ConstKeyIterator;

   GameOverWinnerDrawUpdateData * pData =
      static_cast< UpdateDrawDataUpdatePass * >(pVoid)->pGameOverWinnerDrawUpdateData;

   char * pChar = 0;

   if (!pData->pInitials[0])      pChar = &pData->pInitials[0];
   else if (!pData->pInitials[1]) pChar = &pData->pInitials[1];
   else if (!pData->pInitials[2]) pChar = &pData->pInitials[2];

   for (ConstKeyIterator itBeg = pInput->begin(),
                         itEnd = pInput->end();
        itBeg != itEnd;
        itBeg++)
   {
      switch (*itBeg)
      {
      ADD_CHAR( 1 );
      ADD_CHAR( 2 );
      ADD_CHAR( 3 );
      ADD_CHAR( 4 );
      ADD_CHAR( 5 );
      ADD_CHAR( 6 );
      ADD_CHAR( 7 );
      ADD_CHAR( 8 );
      ADD_CHAR( 9 );
      ADD_CHAR( 0 );
      ADD_CHAR( A );
      ADD_CHAR( B );
      ADD_CHAR( C );
      ADD_CHAR( D );
      ADD_CHAR( E );
      ADD_CHAR( F );
      ADD_CHAR( G );
      ADD_CHAR( H );
      ADD_CHAR( I );
      ADD_CHAR( J );
      ADD_CHAR( K );
      ADD_CHAR( L );
      ADD_CHAR( M );
      ADD_CHAR( N );
      ADD_CHAR( O );
      ADD_CHAR( P );
      ADD_CHAR( Q );
      ADD_CHAR( R );
      ADD_CHAR( S );
      ADD_CHAR( T );
      ADD_CHAR( U );
      ADD_CHAR( V );
      ADD_CHAR( W );
      ADD_CHAR( X );
      ADD_CHAR( Y );
      ADD_CHAR( Z );

      case DIK_BACKSPACE:
         if (!pChar) pData->pInitials[2] = '\0';
         else if (pChar != pData->pInitials) *(pChar - 1) = '\0';

         pData->bUpdateText = true;

         break;

      case DIK_RETURN:
         {
         // open the file to write to
         std::ofstream winnersFile;
         winnersFile.open("winners.txt", std::ios_base::out | std::ios_base::app);

         if (winnersFile.is_open())
         {
            winnersFile << pData->pInitials << " ";
            winnersFile.precision(1);
            winnersFile << std::fixed << pData->dGameTime << std::endl;

            winnersFile.close();
         }
         }

         // reset the keyboard callback
         g_pQGLWindow->SetKeyboardCallback(pVoid, KeyboardCB);

         // trasition to the game over state
         g_eGameStatesCurrent = GAME_STATES_GAME_OVER;

         break;
      }
   }
}

void RenderSquare( unsigned int nRow,
                   unsigned int nColumn,
                   bool bFilledIn )
{
   // determine the width and height of each box
   const int length = (g_nWidth - 2 * (BOARD_DIM_COLUMNS + 1)) / BOARD_DIM_COLUMNS;
   const int halfLength = length / 2;
   
   // determine the middle location
   const unsigned int midX = (halfLength + 2) + (length * nColumn) + (2 * nColumn);
   const unsigned int midY = (halfLength + 2) + (length * nRow) + (2 * nRow);

   // push a matrix on the stack
   glPushMatrix();

   // translate to that location
   glTranslatef(static_cast< float >(midX),
                static_cast< float >(midY),
                0.0f);

   if (bFilledIn)
   {
      // enable blending
      glEnable(GL_BLEND);
      glEnable(GL_ALPHA_TEST);

      // render a filled in face
      glBegin(GL_TRIANGLE_FAN);

      const float alpha = 0.5f;

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glVertex3f(0.0f, 0.0f, 0.0f);

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glVertex3f(static_cast< float >(-halfLength),
                 static_cast< float >(halfLength),
                 0.0f);

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glVertex3f(static_cast< float >(-halfLength),
                 static_cast< float >(-halfLength),
                 0.0f);

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glVertex3f(static_cast< float >(halfLength),
                 static_cast< float >(-halfLength),
                 0.0f);

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glVertex3f(static_cast< float >(halfLength),
                 static_cast< float >(halfLength),
                 0.0f);

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glVertex3f(static_cast< float >(-halfLength),
                 static_cast< float >(halfLength),
                 0.0f);

      glEnd();

      // disable blending
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_BLEND);
   }
   else
   {
      if ((nRow == 5 || nRow == 10) &&
          (g_eGameStatesCurrent != GAME_STATES_TITLE_SCREEN &&
           g_eGameStatesCurrent != GAME_STATES_PRESS_SPACE &&
           g_eGameStatesCurrent != GAME_STATES_SHOW_WINNERS))
      {  
         glColor4f(1.0f, 1.0f, 1.0f, 0.25f);

         glEnable(GL_BLEND);

         // fill in the face
         glBegin(GL_POLYGON);

         glVertex3f(static_cast< float >(-halfLength),
                    static_cast< float >(halfLength),
                    0.0f);
         glVertex3f(static_cast< float >(-halfLength),
                    static_cast< float >(-halfLength),
                    0.0f);
         glVertex3f(static_cast< float >(halfLength),
                    static_cast< float >(-halfLength),
                    0.0f);
         glVertex3f(static_cast< float >(halfLength),
                    static_cast< float >(halfLength),
                    0.0f);

         glEnd();

         glDisable(GL_BLEND);
      }
     
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

      // render an open face
      glBegin(GL_LINE_LOOP);

      glVertex3f(static_cast< float >(-halfLength),
                 static_cast< float >(halfLength),
                 0.0f);
      glVertex3f(static_cast< float >(-halfLength),
                 static_cast< float >(-halfLength),
                 0.0f);
      glVertex3f(static_cast< float >(halfLength),
                 static_cast< float >(-halfLength),
                 0.0f);
      glVertex3f(static_cast< float >(halfLength),
                 static_cast< float >(halfLength),
                 0.0f);

      glEnd();
   }

   // remove the matrix from the stack
   glPopMatrix();
}

void DrawPressSpaceCB( void * pWnd,
                       void * pData,
                       const double & rElapsedTime )
{
   unsigned int * pScanLine = static_cast< PressSpaceUpdateDrawData * >(pData)->pCurrentRow;

   for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
   {
      for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
      {
         RenderSquare(BOARD_DIM_ROWS - row - 1,
                      BOARD_DIM_COLUMNS - column - 1,
                      *pScanLine-- == 0xFF000000);
      }
   }
}

void UpdatePressSpaceCB( void * pVoid,
                         void * pData,
                         const double & rElapsedTime )
{
   PressSpaceUpdateDrawData * pUpdateData = static_cast< PressSpaceUpdateDrawData * >(pData);

   // subtract the time
   pUpdateData->dUpdateTime -= rElapsedTime;

   if (pUpdateData->dUpdateTime <= 0)
   {
      // update the current row
      pUpdateData->pCurrentRow -= 7;

      // reset the time
      pUpdateData->dUpdateTime = 0.09;

      // update if out of bounds
      if (pUpdateData->pCurrentRow < pUpdateData->pLastRow)
      {
         // update the row information
         pUpdateData->pCurrentRow = pUpdateData->pFirstRow;
         // move on to the correct state
         switch (g_eGameStatesCurrent)
         {
         case GAME_STATES_PRESS_SPACE:
            g_eGameStatesCurrent = GAME_STATES_SHOW_WINNERS;
            break;

         case GAME_STATES_SHOW_WINNERS:
            g_eGameStatesCurrent = GAME_STATES_TITLE_SCREEN;
            break;
         }
      }
   }
}

void LoadPressSpaceText( PressSpaceUpdateDrawData & oData  )
{
   // load the press space bmp image
   const ImageLibrary::Image * pImage = ImageLibrary::Instance()->Load("PressSpace.rgb", false, false, false);

   // since the image is loaded from the lower left
   // we need to set the end and front backwards
   oData.pLastRow = static_cast< unsigned int * >(pImage->m_pImage->GetImage()->pData);
   oData.pFirstRow = oData.pLastRow + (pImage->m_pImage->GetSize().m_nWidth *
                                       pImage->m_pImage->GetSize().m_nHeight - 1);
   // set the current row to render from
   oData.pCurrentRow = oData.pFirstRow;

   // set the update time
   oData.dUpdateTime = 0.05;
}

void DrawPlayingGameCB( void * pWnd,
                        void * pData,
                        const double & rElapsedTime )
{
   PlayGameUpdateDrawData * pDrawData = static_cast< PlayGameUpdateDrawData * >(pData);

   for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
   {
      for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
      {
         RenderSquare(row, column, pDrawData->pGameBoard[row][column]);
      }
   }
}

void UpdatePlayingGameCB( void * pWnd,
                          void * pData,
                          const double & rElapsedTime )
{
   PlayGameUpdateDrawData * pUpdateData = static_cast< PlayGameUpdateDrawData * >(pData);

   // decrease the current time
   pUpdateData->dUpdateTime -= rElapsedTime;

   if (pUpdateData->dUpdateTime <= 0.0)
   {
      // determine the current row
      const unsigned int rowIndex =
         (pUpdateData->pCurrentRow - pUpdateData->pFirstRow) / BOARD_DIM_COLUMNS;
      
      // reset the time
      pUpdateData->dUpdateTime = pUpdateData->dUpdateTimes[rowIndex];

      // clear the current row
      pUpdateData->pCurrentRow[0] = false;
      pUpdateData->pCurrentRow[1] = false;
      pUpdateData->pCurrentRow[2] = false;
      pUpdateData->pCurrentRow[3] = false;
      pUpdateData->pCurrentRow[4] = false;
      pUpdateData->pCurrentRow[5] = false;
      pUpdateData->pCurrentRow[6] = false;

      // update the left most column based on the number of
      // active blocks in the current row.
      const int nUpdateLeftMost = pUpdateData->pGameRowLine[2] ? 0 :
                                  pUpdateData->pGameRowLine[1] ? 1 : 2;

      // make sure we move in the correct direction
      if (pUpdateData->eColumnDir == COLUMN_DIRECTION_LEFT &&
          pUpdateData->nCurrentColumn == BOARD_DIM_LEFT_MOST_COLUMN + nUpdateLeftMost)
      {
         pUpdateData->eColumnDir = COLUMN_DIRECTION_RIGHT;
      }
      else if (pUpdateData->eColumnDir == COLUMN_DIRECTION_RIGHT &&
               pUpdateData->nCurrentColumn == BOARD_DIM_RIGHT_MOST_COLUMN)
      {
         pUpdateData->eColumnDir = COLUMN_DIRECTION_LEFT;
      }

      // update based on the direction
      switch (pUpdateData->eColumnDir)
      {
      case COLUMN_DIRECTION_RIGHT:
         pUpdateData->nCurrentColumn++;

         break;

      case COLUMN_DIRECTION_LEFT:
         pUpdateData->nCurrentColumn--;

         break;
      }

      // point to the array of line bools
      bool * pGameRowLine = pUpdateData->pGameRowLine;

      // place the object on the board
      for (int nColBeg = pUpdateData->nCurrentColumn,
               nColEnd = pUpdateData->nCurrentColumn + BOARD_DIM_MAX_LINE_SIZE;
           nColBeg != nColEnd;
           nColBeg++, pGameRowLine++)
      {
         if (nColBeg >= 0 && nColBeg <= BOARD_DIM_RIGHT_MOST_COLUMN)
         {
            pUpdateData->pCurrentRow[nColBeg] = *pGameRowLine;
         }
      }
   }
}

void LoadPlayingGameData( PlayGameUpdateDrawData & oData )
{
   // save the game stream...
   unsigned int gameStream = oData.nGameStream;

   // clear all the data
   memset(&oData, 0x00, sizeof(oData));

   // restore game stream
   oData.nGameStream = gameStream;

   // create a table of the update times
   oData.dUpdateTimes[0]  = 0.100;
   oData.dUpdateTimes[1]  = 0.099;
   oData.dUpdateTimes[2]  = 0.098;
   oData.dUpdateTimes[3]  = 0.096;
   oData.dUpdateTimes[4]  = 0.095;
   oData.dUpdateTimes[5]  = 0.094;
   oData.dUpdateTimes[6]  = 0.088;
   oData.dUpdateTimes[7]  = 0.082;
   oData.dUpdateTimes[8]  = 0.076;
   oData.dUpdateTimes[9]  = 0.070;
   oData.dUpdateTimes[10] = 0.062;
   oData.dUpdateTimes[11] = 0.054;
   oData.dUpdateTimes[12] = 0.046;
   oData.dUpdateTimes[13] = 0.038;
   oData.dUpdateTimes[14] = 0.030;

   // set the current time to zero to do an automatic update
   oData.dUpdateTime = 0.0;

   // set the game row to all true
   oData.pGameRowLine[0] = true;
   oData.pGameRowLine[1] = true;
   oData.pGameRowLine[2] = true;

   // setup the rows
   oData.pFirstRow = oData.pGameBoard[0];
   oData.pLastRow = oData.pGameBoard[BOARD_DIM_ROWS - 1];
   oData.pCurrentRow = oData.pFirstRow;

   // randomize the current column location
   srand(static_cast< unsigned int >(time(NULL)));
   oData.nCurrentColumn = rand() % 5;

   // set the direction
   oData.eColumnDir = COLUMN_DIRECTION_LEFT;
}

void DrawFlashRemovedSquaresCB( void * pWnd,
                                void * pData,
                                const double & rElapsedTime )
{
   FlashRemovedSquaresUpdateDrawData * pDrawData =
      static_cast< FlashRemovedSquaresUpdateDrawData * >(pData);

   for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
   {
      for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
      {
         RenderSquare(row, column, pDrawData->pGameBoard[row][column]);
      }
   }
}

void UpdateFlashRemovedSquaresCB( void * pWnd,
                                  void * pData,
                                  const double & rElapsedTime )
{
   FlashRemovedSquaresUpdateDrawData * pUpdateData =
      static_cast< FlashRemovedSquaresUpdateDrawData * >(pData);

   // reduce the time
   pUpdateData->dUpdateTime -= rElapsedTime;

   if (pUpdateData->bDoFallingAnim)
   {
      // do the falling animation
      if (pUpdateData->dUpdateTime <= 0.0)
      {
         // determines if we can move on to the next stage
         unsigned int nMoveToNextStage = 0x01 | 0x02 | 0x04;

         for (unsigned int i = 0; i < BOARD_DIM_MAX_LINE_SIZE; i++)
         {
            if (pUpdateData->pFallingSquares[i] &&
                pUpdateData->pFallingSquares[i] >= &pUpdateData->pGameBoard[1][0])
            {
               // clear the current location of the block
               *(pUpdateData->pFallingSquares[i]) = false;
               // move the falling block down if possible
               if (*(pUpdateData->pFallingSquares[i] - BOARD_DIM_COLUMNS) == false)
               {
                  pUpdateData->pFallingSquares[i] -= BOARD_DIM_COLUMNS;
                  // this block is still falling
                  nMoveToNextStage &= ~(1 << i);
               }
               // activate the block location
               *(pUpdateData->pFallingSquares[i]) = true;
            }
         }

         if (nMoveToNextStage == 0x07)
         {
            pUpdateData->bDoFallingAnim = false;
         }
         else
         {
            pUpdateData->dUpdateTime = 0.1;
         }
      }
   }
   else
   {
      // do the flashing animation
      if (pUpdateData->dUpdateTime <= 0.0)
      {
         // update the time
         pUpdateData->dUpdateTime = 0.1;

         if (*(pUpdateData->pFallingSquares[0]))
         {
            // turn off the current square location
            for (unsigned int i = 0; i < BOARD_DIM_MAX_LINE_SIZE; i++)
            {
               if (pUpdateData->pFallingSquares[i])
               {
                  *(pUpdateData->pFallingSquares[i]) = false;
               }
            }
         }
         else
         {
            // turn on the current square location
            for (unsigned int i = 0; i < BOARD_DIM_MAX_LINE_SIZE; i++)
            {
               if (pUpdateData->pFallingSquares[i])
               {
                  *(pUpdateData->pFallingSquares[i]) = true;
               }
            }

            pUpdateData->nFlashCount--;
         }

         if (!pUpdateData->nFlashCount)
         {
            if (!pUpdateData->bIsGameOver)
            {
               // go back to the main game since we still have blocks left
               g_eGameStatesCurrent = GAME_STATES_PLAYING_GAME;
            }
            else
            {
               GameOverUpdateDrawData * pGameOverUpdateDrawData =
                  pUpdateData->pGameOverUpdateDrawData;

               // setup the game over state
               pGameOverUpdateDrawData->dUpdateTime = 0.0;
               pGameOverUpdateDrawData->pGameBoard = pUpdateData->pGameBoard;
               pGameOverUpdateDrawData->pCurrentRow = pUpdateData->pGameBoard;
               pGameOverUpdateDrawData->pNextFallingRow = pUpdateData->pGameBoard + 1;

               for (unsigned int row = BOARD_DIM_ROWS - 1; row >= 0; row--)
               {
                  if (pUpdateData->pGameBoard[row][0] || pUpdateData->pGameBoard[row][1] ||
                      pUpdateData->pGameBoard[row][2] || pUpdateData->pGameBoard[row][3] ||
                      pUpdateData->pGameBoard[row][4] || pUpdateData->pGameBoard[row][5] ||
                      pUpdateData->pGameBoard[row][6])
                  {
                     pGameOverUpdateDrawData->pTopRow = pUpdateData->pGameBoard + row;

                     break;
                  }
               }

               // go to the game over sequence
               g_eGameStatesCurrent = GAME_STATES_GAME_OVER;
            }
         }
      }
   }
}

void DrawGameOverCB( void * pWnd,
                     void * pData,
                     const double & rElapsedTime )
{
   GameOverUpdateDrawData * pDrawData =
      static_cast< GameOverUpdateDrawData * >(pData);

   for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
   {
      for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
      {
         RenderSquare(row, column, pDrawData->pGameBoard[row][column]);
      }
   }
}

void UpdateGameOverCB( void * pWnd,
                       void * pData,
                       const double & rElapsedTime )
{
   GameOverUpdateDrawData * pUpdateData =
      static_cast< GameOverUpdateDrawData * >(pData);

   pUpdateData->dUpdateTime -= rElapsedTime;

   if (pUpdateData->dUpdateTime <= 0.0)
   {
      // reset the time
      pUpdateData->dUpdateTime = 0.01;

      if (pUpdateData->pCurrentRow <= pUpdateData->pGameBoard)
      {
         // clear the current row
         (*pUpdateData->pCurrentRow)[0] = (*pUpdateData->pCurrentRow)[1] =
         (*pUpdateData->pCurrentRow)[2] = (*pUpdateData->pCurrentRow)[3] =
         (*pUpdateData->pCurrentRow)[4] = (*pUpdateData->pCurrentRow)[5] =
         (*pUpdateData->pCurrentRow)[6] = false;

         // this is the bottom row and we still have blocks to fall
         pUpdateData->pCurrentRow = pUpdateData->pNextFallingRow;
         // update the next falling row
         pUpdateData->pNextFallingRow++;

         if (pUpdateData->pCurrentRow > pUpdateData->pTopRow)
         {
            // reset some game state information
            LoadPressSpaceText(*pUpdateData->pPressSpaceUpdateDrawData);
            LoadPlayingGameData(*pUpdateData->pPlayGameUpdateDrawData);
            LoadWinnersBuffer(*pUpdateData->pShowWinnersUpdateDrawData);

            // move back to the title screen
            g_eGameStatesCurrent = GAME_STATES_TITLE_SCREEN;

            // stop the playing game stream
            QuickSoundManager::Instance().StopStream(
               pUpdateData->pPlayGameUpdateDrawData->nGameStream);

            // restart the title screen stream
            QuickSoundManager::Instance().PlayStream(
               pUpdateData->pTitleScreenUpdateDrawData->nTitleStream);
         }
      }
      else
      {
         // move the current row down
         memcpy(*(pUpdateData->pCurrentRow - 1),
                *pUpdateData->pCurrentRow,
                sizeof(*pUpdateData->pCurrentRow));

         // clear the current row
         (*pUpdateData->pCurrentRow)[0] = (*pUpdateData->pCurrentRow)[1] =
         (*pUpdateData->pCurrentRow)[2] = (*pUpdateData->pCurrentRow)[3] =
         (*pUpdateData->pCurrentRow)[4] = (*pUpdateData->pCurrentRow)[5] =
         (*pUpdateData->pCurrentRow)[6] = false;

         // move the current row down
         pUpdateData->pCurrentRow--;
      }
   }
}


void DrawEnterNameCB( void * pWnd,
                      void * pData,
                      const double & rElapsedTime )
{
   GameOverWinnerDrawUpdateData * pDrawData =
      static_cast< GameOverWinnerDrawUpdateData * >(pData);

   // push the current matrix
   glPushMatrix();
   // move the text to the center
   glTranslatef(g_nWidth / 2.0f, g_nHeight / 2.0f, 0.0f);
   
   // render the text
   pDrawData->pFont->Draw(rElapsedTime);

   // pop the current matrix
   glPopMatrix();
}

void UpdateEnterNameCB( void * pWnd,
                        void * pData,
                        const double & rElapsedTime )
{
   GameOverWinnerDrawUpdateData * pUpdateData =
      static_cast< GameOverWinnerDrawUpdateData * >(pData);

   if (pUpdateData->bUpdateText)
   {
      // format the number of seconds
      char strBuffer[128] = { 0 };
      sprintf(strBuffer,
              "Congratulations!!!\n\n"
              "Game completed in\n%.1f seconds!!!\n\n"
              "Enter your initials:\n",
              pUpdateData->dGameTime);

      // set the new information
      *(pUpdateData->pFont) = strBuffer;
      *(pUpdateData->pFont) += pUpdateData->pInitials;

      // no longer updating text
      pUpdateData->bUpdateText = false;
   }
}

void DrawShowWinnersCB( void * pWnd,
                        void * pData,
                        const double & rElapsedTime )
{
   if (reinterpret_cast< ShowWinnersUpdateDrawData * >(pData)->pData)
   {
      DrawPressSpaceCB(pWnd, pData, rElapsedTime);
   }
   else
   {
      for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
      {
         for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
         {
            RenderSquare(BOARD_DIM_ROWS - row - 1,
                         BOARD_DIM_COLUMNS - column - 1,
                         false);
         }
      }
   }
}

void UpdateShowWinnersCB( void * pWnd,
                          void * pData,
                          const double & rElapsedTime )
{
   if (reinterpret_cast< ShowWinnersUpdateDrawData * >(pData)->pData)
   {
      UpdatePressSpaceCB(pWnd, pData, rElapsedTime);
   }
   else
   {
      // there are no winners to show, so just go back to
      // the title screen...
      g_eGameStatesCurrent = GAME_STATES_TITLE_SCREEN;
   }
}

void LoadWinnersBuffer( ShowWinnersUpdateDrawData & oData )
{
   // release the previous image
   delete [] oData.pData;
   oData.pData = NULL;

   // load the press space bmp image
   const ImageLibrary::Image * pImage = ImageLibrary::Instance()->Load("ABC.rgb", false, false, false);

   // load the winners file
   std::ifstream winnersFile;
   winnersFile.open("winners.txt");

   // create a vector of strings
   std::map< double, std::string > vecWinners;
   vecWinners.insert(std::map< double, std::string >::value_type(0, "WINNERS"));

   while (!winnersFile.eof())
   {
      double dTime = 0.0;
      std::string winner;
      std::string time;
      winnersFile >> winner >> time;
      dTime = atof(time.c_str());
      std::string winnertime = winner + " " + time;
      if (!winner.empty())
         vecWinners.insert(std::map< double, std::string >::value_type(dTime, winnertime));
   }

   if (vecWinners.size() > 1)
   {
      // determine the space size
      unsigned int size = vecWinners.size() * 15 + 15;

      // determine the letter size
      for (std::map< double, std::string >::iterator itBeg = vecWinners.begin(),
                                                     itEnd = vecWinners.end();
           itBeg != itEnd;
           itBeg++)
      {
         size += (itBeg->second.size() * 9);

         // remove some rows for the white space
         if (itBeg->second.find(' ') != std::string::npos)
            size -= 5;
      }

      // create an image of that size
      unsigned int * pData = new unsigned int[7 * size];

      // assign the data
      oData.pData = pData;

      // set all the data to zero
      memset(pData, 0x00, 7 * size * sizeof(unsigned int));

      // obtain the first row of the image
      unsigned int * pFirstRow =
         static_cast< unsigned int * >(pImage->m_pImage->GetImage()->pData) +
                                      (pImage->m_pImage->GetSize().m_nWidth *
                                      (pImage->m_pImage->GetSize().m_nHeight - 1));

      pData += (7 * size) - (15 * 7);

      // start sucking in the data;
      for (std::map< double, std::string >::iterator itBeg = vecWinners.begin(),
                                                     itEnd = vecWinners.end();
           itBeg != itEnd;
           itBeg++)
      {
         for (std::string::iterator itStrBeg = itBeg->second.begin(),
                                    itStrEnd = itBeg->second.end();
              itStrBeg != itStrEnd;
              itStrBeg++)
         {
            unsigned int offset = 1;

            if ('.' <= *itStrBeg && *itStrBeg <= 'Z')
            {
               offset = *itStrBeg - '.';
            }
            else if ('a' <= *itStrBeg && *itStrBeg <= 'z')
            {
               offset = *itStrBeg - 'a';
            }
            else if (' ' == *itStrBeg)
            {
               for (unsigned int i = 0; i < 4; ++i)
               {
                  pData -= 7;
                  memset(pData, 0xFF, sizeof(*pData) * 7);
               }

               continue;
            }

            for (unsigned int i = 0; i < 9; i++)
            {
               pData -= 7;
               memcpy(pData, pFirstRow - (offset * 9 * 7) - (i * 7), 4 * 7);
            } 
         }

         pData -= (15 * 7);
      }

      // since the image is loaded from the lower left
      // we need to set the end and front backwards
      oData.pLastRow = oData.pData;
      oData.pFirstRow = oData.pLastRow + (7 * size - 1);
      // set the current row to render from
      oData.pCurrentRow = oData.pFirstRow;
   }

   oData.dUpdateTime = 0.05;
}

void DrawTitleScreenCB( void * pWnd,
                        void * pData,
                        const double & rElapsedTime )
{
   for (unsigned int row = 0; row < BOARD_DIM_ROWS; row++)
   {
      for (unsigned int column = 0; column < BOARD_DIM_COLUMNS; column++)
      {
         RenderSquare(BOARD_DIM_ROWS - row - 1,
                      BOARD_DIM_COLUMNS - column - 1,
                      false);
      }
   }
}

void UpdateTitleScreenCB( void * pWnd,
                          void * pData,
                          const double & rElapsedTime )
{
   g_eGameStatesCurrent = GAME_STATES_PRESS_SPACE;
}