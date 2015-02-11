#ifndef _TOWER_WINDOW_H_
#define _TOWER_WINDOW_H_

// wingl includes
//#include "Matrix.h"
#include "OpenGLWindow.h"
//#include "ShaderProgram.h"

// std includes
#include <memory>

// forward declarations
class CFont;

class TowerWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   TowerWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = nullptr ) override;

   // basic run implementation
   // will process messages until app quit
   virtual int Run( ) override;
   
protected:
   // destructor
   virtual ~TowerWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // private enumerations
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

   // private structures
   struct UpdateDrawCB
   {
      void (TowerWindow::*DrawCB)( void *, void *, const double & );
      void * pDrawData;
      void (TowerWindow::*UpdateCB)( void *, void *, const double & );
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

   // keyboard notifications
   void KeyboardCB( void * pVoid, unsigned int virtual_key, const double & rElapsedTime );
   void EnterInitialsKeyboardCB( void * pVoid, unsigned int virtual_key, const double & rElapsedTime );

   // rendering routines
   void RenderSquare( unsigned int nRow, unsigned int nColumn, bool bFilledIn );

   // loads / resets game data for each stage
   void LoadPressSpaceText( PressSpaceUpdateDrawData & oData );
   void LoadPlayingGameData( PlayGameUpdateDrawData & oData );
   void LoadWinnersBuffer( ShowWinnersUpdateDrawData & oData );

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

   // private member data
   // defines the current game state
   GameStates mGameStatesCurrent;

   // defines the draw and update callbacks for the defined game states
   UpdateDrawCB mUpdateDrawCB[GAME_STATES_MAX];

   // update / draw callback data
   PressSpaceUpdateDrawData            mPressSpaceUpdateDrawData;
   PlayGameUpdateDrawData              mPlayGameUpdateDrawData;
   ShowWinnersUpdateDrawData           mShowWinnersUpdateDrawData;
   TitleScreenUpdateDrawData           mTitleScreenUpdateDrawData;
   GameOverUpdateDrawData              mGameOverUpdateDrawData;
   FlashRemovedSquaresUpdateDrawData   mFlashRemovedSquaresUpdateDrawData;
   GameOverWinnerDrawUpdateData        mGameOverWinnerDrawUpdateData;
   UpdateDrawDataUpdatePass            mUpdateDrawDataUpdatePass;

   // defines the current keyboard callback
   void (TowerWindow::*mpKeyboardCB)( void *, unsigned int, const double & );

   // stores a safe reference to the texture data
   std::shared_ptr< uint8_t > mPressSpaceTex;
   std::shared_ptr< uint8_t > mWinnersTex;

};

#endif // _TOWER_WINDOW_H_
