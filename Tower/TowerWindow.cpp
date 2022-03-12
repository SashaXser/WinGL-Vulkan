// local includes
#include "TowerWindow.h"
#include "CFont.h"

// wingl includes
#include "Timer.h"
#include "Matrix.h"
#include "Vector.h"
#include "ReadTexture.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <map>
#include <ratio>
#include <chrono>
#include <cstdint>
#include <fstream>

#include <time.h>

TowerWindow::TowerWindow( ) :
mGameStatesCurrent      ( GAME_STATES_PRESS_SPACE ),
mpKeyboardCB            ( &TowerWindow::KeyboardCB )
{
   // setup the update and draw callbacks
   mUpdateDrawCB[GAME_STATES_PRESS_SPACE] = { &TowerWindow::DrawPressSpaceCB, NULL, &TowerWindow::UpdatePressSpaceCB, NULL };
   mUpdateDrawCB[GAME_STATES_PLAYING_GAME] = { &TowerWindow::DrawPlayingGameCB, NULL, &TowerWindow::UpdatePlayingGameCB, NULL };
   mUpdateDrawCB[GAME_STATES_GAME_OVER] = { &TowerWindow::DrawGameOverCB, NULL, &TowerWindow::UpdateGameOverCB, NULL };
   mUpdateDrawCB[GAME_STATES_FLASH_REMOVED_SQUARES] = { &TowerWindow::DrawFlashRemovedSquaresCB, NULL, &TowerWindow::UpdateFlashRemovedSquaresCB, NULL };
   mUpdateDrawCB[GAME_STATES_ENTER_NAME] = { &TowerWindow::DrawEnterNameCB, NULL, &TowerWindow::UpdateEnterNameCB, NULL };
   mUpdateDrawCB[GAME_STATES_SHOW_WINNERS] = { &TowerWindow::DrawShowWinnersCB, NULL, &TowerWindow::UpdateShowWinnersCB, NULL };
   mUpdateDrawCB[GAME_STATES_TITLE_SCREEN] = { &TowerWindow::DrawTitleScreenCB, NULL, &TowerWindow::UpdateTitleScreenCB, NULL };
}

TowerWindow::~TowerWindow( )
{
   // there should still be a valid context here
   WGL_ASSERT(wglGetCurrentContext());

   // release the font texture
   delete mGameOverWinnerDrawUpdateData.pFont;
}

bool TowerWindow::Create( unsigned int nWidth,
                          unsigned int nHeight,
                          const char * pWndTitle,
                          const void * /*pInitParams*/ )
{
   // width and height are hard coded at the moment
   nWidth = 479;
   nHeight = 1024;

   // initialize the opengl context
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 2, 1, true, false, false }, 
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // initialize the sound manager

      // initialize the sound samples
      //const unsigned int nTitleStream = rQSM.LoadStream("./TitleScreen.wav", true);
      //const unsigned int nPlayGameStream = rQSM.LoadStream("./PlayingGame.wav", true);
      //const unsigned int nWinnerStream = rQSM.LoadStream("./WinnerScreen.wav", true);
      //rQSM.PlayStream(nTitleStream);

      // modify the update and draw callback data
      // create and update draw data store for the press space screen
      LoadPressSpaceText(mPressSpaceUpdateDrawData);

      mUpdateDrawCB[GAME_STATES_PRESS_SPACE].pDrawData = &mPressSpaceUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_PRESS_SPACE].pUpdateData = &mPressSpaceUpdateDrawData;

      // create and update draw data store for the actual game
      LoadPlayingGameData(mPlayGameUpdateDrawData);
      //mPlayGameUpdateDrawData.nGameStream = nPlayGameStream;

      mUpdateDrawCB[GAME_STATES_PLAYING_GAME].pDrawData = &mPlayGameUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_PLAYING_GAME].pUpdateData = &mPlayGameUpdateDrawData;

      // create and update draw data store for showing winners
      mShowWinnersUpdateDrawData.pData = nullptr;
      LoadWinnersBuffer(mShowWinnersUpdateDrawData);

      mUpdateDrawCB[GAME_STATES_SHOW_WINNERS].pDrawData = &mShowWinnersUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_SHOW_WINNERS].pUpdateData = &mShowWinnersUpdateDrawData;

      // create and update draw data store for title screen
      //mTitleScreenUpdateDrawData.nTitleStream = nTitleStream;

      mUpdateDrawCB[GAME_STATES_TITLE_SCREEN].pDrawData = &mTitleScreenUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_TITLE_SCREEN].pUpdateData = &mTitleScreenUpdateDrawData;

      // create and update draw data store for game over
      mGameOverUpdateDrawData.dUpdateTime = 0.0;
      mGameOverUpdateDrawData.pTopRow = nullptr;
      mGameOverUpdateDrawData.pGameBoard = nullptr;
      mGameOverUpdateDrawData.pCurrentRow = nullptr;
      mGameOverUpdateDrawData.pNextFallingRow = nullptr;
      mGameOverUpdateDrawData.pPressSpaceUpdateDrawData = &mPressSpaceUpdateDrawData;
      mGameOverUpdateDrawData.pPlayGameUpdateDrawData = &mPlayGameUpdateDrawData;
      mGameOverUpdateDrawData.pShowWinnersUpdateDrawData = &mShowWinnersUpdateDrawData;
      mGameOverUpdateDrawData.pTitleScreenUpdateDrawData = &mTitleScreenUpdateDrawData;

      mUpdateDrawCB[GAME_STATES_GAME_OVER].pDrawData = &mGameOverUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_GAME_OVER].pUpdateData = &mGameOverUpdateDrawData;

      // create and update draw data store for removing squares
      mFlashRemovedSquaresUpdateDrawData.pGameOverUpdateDrawData = &mGameOverUpdateDrawData;
      
      mUpdateDrawCB[GAME_STATES_FLASH_REMOVED_SQUARES].pDrawData = &mFlashRemovedSquaresUpdateDrawData;
      mUpdateDrawCB[GAME_STATES_FLASH_REMOVED_SQUARES].pUpdateData = &mFlashRemovedSquaresUpdateDrawData;

      // create and update draw data store for entering initials
      mGameOverWinnerDrawUpdateData.bUpdateText = true;
      mGameOverWinnerDrawUpdateData.pInitials[0] = '\0';
      mGameOverWinnerDrawUpdateData.pInitials[1] = '\0';
      mGameOverWinnerDrawUpdateData.pInitials[2] = '\0';
      mGameOverWinnerDrawUpdateData.pInitials[3] = '\0';
      mGameOverWinnerDrawUpdateData.pFont = new CFont(0,
                                                      Vec3f(0.0f, 0.0f, 0.0f),
                                                      ".\\Tower\\Font.tga",
                                                      20, 25,
                                                      "",
                                                      1.0f,
                                                      CFont::ALIGN_CENTER);

      mUpdateDrawCB[GAME_STATES_ENTER_NAME].pDrawData = &mGameOverWinnerDrawUpdateData;
      mUpdateDrawCB[GAME_STATES_ENTER_NAME].pUpdateData = &mGameOverWinnerDrawUpdateData;

      // create an update draw callback object
      mUpdateDrawDataUpdatePass.pGameOverUpdateDrawData = &mGameOverUpdateDrawData;
      mUpdateDrawDataUpdatePass.pPlayGameUpdateDrawData = &mPlayGameUpdateDrawData;
      mUpdateDrawDataUpdatePass.pGameOverWinnerDrawUpdateData = &mGameOverWinnerDrawUpdateData;
      mUpdateDrawDataUpdatePass.pFlashRemovedSquaresUpdateDrawData = &mFlashRemovedSquaresUpdateDrawData;

      // setup the ortho params
      const Matrixf ortho = Matrixf::Ortho(0.0f, static_cast< float >(nWidth),
                                           0.0f, static_cast< float >(nHeight),
                                           -1.0f, 1.0f);
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(ortho);

      // setup the lookat params
      const Matrixf camera = Matrixf::LookAt(Vec3f(0.0f, 0.0f, 0.0f),
                                             Vec3f(0.0f, 0.0f, -1.0f),
                                             Vec3f(0.0f, 1.0f, 0.0f));
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(camera);

      // set the alpha function
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // set the clear color to black
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      
      return true;
   }
   else
   {
      // issue an error from the application that it could not be created
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, "Unable To Create 2.1 OpenGL Context");

      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int TowerWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   // determines the update rate in ms
   const double update_freq_in_ms = 1000.0 / 60.0;

   while (!bQuit)
   {
      // get the current time
      const double current_time_sec = Timer().GetCurrentTimeSec();

      // clear the color buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // process all the app messages and then render the scene
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
      {
         // determine the amount of elapsed time
         const std::chrono::duration< double, std::milli > duration_update_freq(update_freq_in_ms);
         const double elapsed_time_sec = std::chrono::duration_cast< std::chrono::duration< double, std::ratio< 1, 1 > > >(duration_update_freq).count();

         // update the scene
         const UpdateDrawCB * const pUpdateDrawCB = mUpdateDrawCB + mGameStatesCurrent;
         (this->*(pUpdateDrawCB->UpdateCB))(nullptr, pUpdateDrawCB->pUpdateData, elapsed_time_sec);

         // render the scene
         (this->*(pUpdateDrawCB->DrawCB))(nullptr, pUpdateDrawCB->pDrawData, elapsed_time_sec);

         // swap the buffers
         SwapBuffers(GetHDC());
      }

      // determine the amount of remaining time per frame
      const double delta_time_remaining_ms = update_freq_in_ms - (Timer().GetCurrentTimeMS() - current_time_sec * 1000.0);

      if (delta_time_remaining_ms >= 1.0)
      {
         Sleep(static_cast< uint32_t >(delta_time_remaining_ms));
      }
   }

   return appQuitVal;
}

void TowerWindow::OnDestroy( )
{
   // allow the base class to handle
   OpenGLWindow::OnDestroy();
}

LRESULT TowerWindow::MessageHandler( UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam )
{
   // determine the amount of elapsed time
   const std::chrono::duration< double, std::milli > duration_update_freq(1000.0 / 60.0);
   const double elapsed_time_sec = std::chrono::duration_cast< std::chrono::duration< double, std::ratio< 1, 1 > > >(duration_update_freq).count();

   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_KEYDOWN:
      // call the correct implamentation
      (this->*mpKeyboardCB)(&mUpdateDrawDataUpdatePass, static_cast< unsigned int >(wParam), elapsed_time_sec);

      break;

   // let the default implementation handle this
   default: result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam); break;
   }

   return result;
}

void TowerWindow::KeyboardCB( void * pVoid, unsigned int virtual_key, const double & /*rElapsedTime*/ )
{
   switch (virtual_key)
   {
   case VK_SPACE:
      if (mGameStatesCurrent == GAME_STATES_TITLE_SCREEN ||
          mGameStatesCurrent == GAME_STATES_PRESS_SPACE ||
          mGameStatesCurrent == GAME_STATES_SHOW_WINNERS)
      {
         // remove all the keys
   
         // add back only gameplay keys
   
         // transition the state
         mGameStatesCurrent = GAME_STATES_PLAYING_GAME;
   
         // obtain the update / draw data for this pass
         UpdateDrawDataUpdatePass * pUpdateData =
            static_cast< UpdateDrawDataUpdatePass * >(pVoid);
   
         // stop the title screen stream
         //QuickSoundManager::Instance().StopStream(
         //   pUpdateData->pGameOverUpdateDrawData->pTitleScreenUpdateDrawData->nTitleStream);
   
         // begin playing the main game stream
         //QuickSoundManager::Instance().PlayStream(
         //   pUpdateData->pPlayGameUpdateDrawData->nGameStream);
   
         // determine the absolute time
         pUpdateData->pPlayGameUpdateDrawData->dStartTime = Timer().GetCurrentTimeSec();
      }
      else if (mGameStatesCurrent == GAME_STATES_PLAYING_GAME)
      {
         // obtain the data
         UpdateDrawDataUpdatePass * pUpdateData = static_cast< UpdateDrawDataUpdatePass * >(pVoid);
         // obtain the game data
         PlayGameUpdateDrawData * pGameData = pUpdateData->pPlayGameUpdateDrawData;
         GameOverUpdateDrawData * pGameOverData = pUpdateData->pGameOverUpdateDrawData;
         GameOverWinnerDrawUpdateData * pGameOverWinnerData = pUpdateData->pGameOverWinnerDrawUpdateData;
         FlashRemovedSquaresUpdateDrawData * pFlashData = pUpdateData->pFlashRemovedSquaresUpdateDrawData;
   
         // obtain the current row
         const size_t rowIndex =
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
               mGameStatesCurrent = GAME_STATES_FLASH_REMOVED_SQUARES;
            }
            else if (rowIndex + 1 >= BOARD_DIM_ROWS)
            {
               // remove and add all the keys
   
               // point to a new keyboard callback
               mpKeyboardCB = &TowerWindow::EnterInitialsKeyboardCB;
   
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
               mGameStatesCurrent = GAME_STATES_ENTER_NAME;
   
               // obtain the finish time
               pUpdateData->pGameOverWinnerDrawUpdateData->dGameTime =
                  Timer().GetCurrentTimeSec() - pUpdateData->pPlayGameUpdateDrawData->dStartTime;
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
   
   case VK_ESCAPE:
      // shutdown the app
      PostQuitMessage(0);
   
      return;
   }
}

void TowerWindow::EnterInitialsKeyboardCB( void * pVoid, unsigned int virtual_key, const double & /*rElapsedTime*/ )
{
#define ADD_CHAR( c ) case #@c: if (pChar) { *pChar = #@c; pData->bUpdateText = true; } break;

   GameOverWinnerDrawUpdateData * pData =
      static_cast< UpdateDrawDataUpdatePass * >(pVoid)->pGameOverWinnerDrawUpdateData;

   char * pChar = 0;

   if (!pData->pInitials[0])      pChar = &pData->pInitials[0];
   else if (!pData->pInitials[1]) pChar = &pData->pInitials[1];
   else if (!pData->pInitials[2]) pChar = &pData->pInitials[2];

   switch (virtual_key)
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
   
   case VK_BACK:
      if (!pChar) pData->pInitials[2] = '\0';
      else if (pChar != pData->pInitials) *(pChar - 1) = '\0';
   
      pData->bUpdateText = true;
   
      break;
   
   case VK_RETURN:
      {
      // open the file to write to
      std::ofstream winnersFile;
      winnersFile.open(R"(.\Tower\winners.txt)", std::ios_base::out | std::ios_base::app);
   
      if (winnersFile.is_open())
      {
         winnersFile << pData->pInitials << " ";
         winnersFile.precision(1);
         winnersFile << std::fixed << pData->dGameTime << std::endl;
   
         winnersFile.close();
      }
      }
   
      // reset the keyboard callback
      mpKeyboardCB = &TowerWindow::KeyboardCB;
   
      // trasition to the game over state
      mGameStatesCurrent = GAME_STATES_GAME_OVER;
   
      break;
   }
}

void TowerWindow::RenderSquare( unsigned int nRow, unsigned int nColumn, bool bFilledIn )
{
   // determine the width and height of each box
   const int length = (GetSize().width - 2 * (BOARD_DIM_COLUMNS + 1)) / BOARD_DIM_COLUMNS;
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
          (mGameStatesCurrent != GAME_STATES_TITLE_SCREEN &&
           mGameStatesCurrent != GAME_STATES_PRESS_SPACE &&
           mGameStatesCurrent != GAME_STATES_SHOW_WINNERS))
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

void TowerWindow::LoadPressSpaceText( PressSpaceUpdateDrawData & oData )
{
   // load the press space bmp image
   const auto texture_data = ReadTexture< uint8_t >(R"(.\Tower\PressSpace.rgb)", GL_RGBA);

   // save the press space texture
   mPressSpaceTex = texture_data.pTexture;

   // since the image is loaded from the lower left
   // we need to set the end and front backwards
   oData.pLastRow = reinterpret_cast< unsigned int * >(texture_data.pTexture.get());
   oData.pFirstRow = oData.pLastRow + (texture_data.width * texture_data.height - 1);
   // set the current row to render from
   oData.pCurrentRow = oData.pFirstRow;

   // set the update time
   oData.dUpdateTime = 0.05;
}

void TowerWindow::LoadPlayingGameData( PlayGameUpdateDrawData & oData )
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

void TowerWindow::LoadWinnersBuffer( ShowWinnersUpdateDrawData & oData )
{
   // release the previous image
   delete [] oData.pData;
   oData.pData = NULL;

   // load the press space bmp image
   const auto texture_data = ReadTexture< uint8_t >(R"(.\Tower\ABC.rgb)", GL_RGBA);

   // save a reference to the buffer
   mWinnersTex = texture_data.pTexture;

   // load the winners file
   std::ifstream winnersFile;
   winnersFile.open(R"(.\Tower\winners.txt)");

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
      size_t size = vecWinners.size() * 15 + 15;

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
         reinterpret_cast< unsigned int * >(texture_data.pTexture.get()) +
                                           (texture_data.width * (texture_data.height - 1));

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

void TowerWindow::DrawPressSpaceCB( void * /*pWnd*/, void * pData, const double & /*rElapsedTime*/ )
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

void TowerWindow::DrawPlayingGameCB( void * /*pWnd*/, void * pData, const double & /*rElapsedTime*/ )
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

void TowerWindow::DrawFlashRemovedSquaresCB( void * /*pWnd*/, void * pData, const double & /*rElapsedTime*/ )
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

void TowerWindow::DrawGameOverCB( void * /*pWnd*/, void * pData, const double & /*rElapsedTime*/ )
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

void TowerWindow::DrawEnterNameCB( void * /*pWnd*/, void * pData, const double & rElapsedTime )
{
   GameOverWinnerDrawUpdateData * pDrawData =
      static_cast< GameOverWinnerDrawUpdateData * >(pData);

   // push the current matrix
   glPushMatrix();
   // move the text to the center
   glTranslatef(GetSize().width / 2.0f, GetSize().height / 2.0f, 0.0f);
   
   // render the text
   pDrawData->pFont->Draw(rElapsedTime);

   // pop the current matrix
   glPopMatrix();
}

void TowerWindow::DrawShowWinnersCB( void * pWnd, void * pData, const double & rElapsedTime )
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

void TowerWindow::DrawTitleScreenCB( void * /*pWnd*/, void * /*pData*/, const double & /*rElapsedTime*/ )
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

void TowerWindow::UpdatePressSpaceCB( void * /*pWnd*/, void * pData, const double & rElapsedTime )
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
         switch (mGameStatesCurrent)
         {
         case GAME_STATES_PRESS_SPACE:
            mGameStatesCurrent = GAME_STATES_SHOW_WINNERS;
            break;

         case GAME_STATES_SHOW_WINNERS:
            mGameStatesCurrent = GAME_STATES_TITLE_SCREEN;
            break;
         }
      }
   }
}

void TowerWindow::UpdatePlayingGameCB( void * /*pWnd*/, void * pData, const double & rElapsedTime )
{
   PlayGameUpdateDrawData * pUpdateData = static_cast< PlayGameUpdateDrawData * >(pData);

   // decrease the current time
   pUpdateData->dUpdateTime -= rElapsedTime;

   if (pUpdateData->dUpdateTime <= 0.0)
   {
      // determine the current row
      const size_t rowIndex =
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

void TowerWindow::UpdateFlashRemovedSquaresCB( void * /*pWnd*/, void * pData, const double & rElapsedTime )
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
               mGameStatesCurrent = GAME_STATES_PLAYING_GAME;
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
               mGameStatesCurrent = GAME_STATES_GAME_OVER;
            }
         }
      }
   }
}

void TowerWindow::UpdateGameOverCB( void * /*pWnd*/, void * pData, const double & rElapsedTime )
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
            mGameStatesCurrent = GAME_STATES_TITLE_SCREEN;

            // stop the playing game stream
            //QuickSoundManager::Instance().StopStream(
            //   pUpdateData->pPlayGameUpdateDrawData->nGameStream);

            // restart the title screen stream
            //QuickSoundManager::Instance().PlayStream(
            //   pUpdateData->pTitleScreenUpdateDrawData->nTitleStream);
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

void TowerWindow::UpdateEnterNameCB( void * /*pWnd*/, void * pData, const double & /*rElapsedTime*/ )
{
   GameOverWinnerDrawUpdateData * pUpdateData =
      static_cast< GameOverWinnerDrawUpdateData * >(pData);

   if (pUpdateData->bUpdateText)
   {
      // set the new information
      *(pUpdateData->pFont) = "Congratulations!!!\n\n";
      *(pUpdateData->pFont) += "Game completed in\n" + std::to_string(pUpdateData->dGameTime) + " seconds!!!\n\n";
      *(pUpdateData->pFont) += "Enter your initials:\n";
      *(pUpdateData->pFont) += pUpdateData->pInitials;

      // no longer updating text
      pUpdateData->bUpdateText = false;
   }
}

void TowerWindow::UpdateShowWinnersCB( void * pWnd, void * pData, const double & rElapsedTime )
{
   if (reinterpret_cast< ShowWinnersUpdateDrawData * >(pData)->pData)
   {
      UpdatePressSpaceCB(pWnd, pData, rElapsedTime);
   }
   else
   {
      // there are no winners to show, so just go back to
      // the title screen...
      mGameStatesCurrent = GAME_STATES_TITLE_SCREEN;
   }
}

void TowerWindow::UpdateTitleScreenCB( void * /*pWnd*/, void * /*pData*/, const double & /*rElapsedTime*/ )
{
   mGameStatesCurrent = GAME_STATES_PRESS_SPACE;
}

