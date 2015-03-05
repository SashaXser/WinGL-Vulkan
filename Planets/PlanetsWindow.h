#ifndef _PLANETS_WINDOW_H_
#define _PLANETS_WINDOW_H_

// wingl includes
#include "Matrix.h"
#include "OpenGLWindow.h"
#include "ShaderProgram.h"

// std includes
#include <cstdint>

// forward declarations
class Planet;

class PlanetsWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   PlanetsWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~PlanetsWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // enumerations that defines the planets
   // the sun is not a planet, but is allowed to
   // be used a special case for certain things
   enum Planets
   {
      MERCURY,       VENUS,
      EARTH,         MARS,
      JUPITER,       SATURN,
      URANUS,        NEPTUNE,
      PLUTO,         SUN,
      MAX_PLANETS
   };

   // renders the scene
   void RenderScene( const double elapsed_time_sec );
   void UpdateScene( const double elapsed_time_sec );
   void DrawScene( const double elapsed_time_sec );

   // generates the scene data
   void GenerateSceneData( );
   void GenerateOrbitalDisplayLists( );

   // defines the major / minor axes pointer
   const double (* const mpMajMinAxes)[3];

   // defines the display lists that will render the orbits
   uint32_t mOrbitDispListsTrue[MAX_PLANETS - 1];
   uint32_t mOrbitDispListsFalse[MAX_PLANETS - 1];

   // defines the orbit display list pointer
   const uint32_t * const mpOrbitDisplayList;

   // defines the elapsed time multiplier
   double mElapsedTimeMultiplier;

   // defines all the planets that will be rendered
   Planet * mppPlanets[MAX_PLANETS];

   // defines the projection and camera matrices
   Matrixf mProjMat;
   Matrixf mViewMat;

   // defines the camera step speed
   float mCamStepSpeed;

   // static helper functions to initialize the major / minor axes arrays
   static void DefineMajorMinorAxes( double & major, double & minor, double & distance, const bool convert_to_gl );
   static double DefineMajorAxis( double major, double minor, const bool convert_to_gl );
   static double DefineMinorAxis( double major, double minor, const bool convert_to_gl );
   static double DefineDistance( double major, double minor, const bool convert_to_gl );

   // defines all the major and minor axes for the planets
   // each planet starts with the minimum and maximum solar
   // distances in kilometers.  these values will then be
   // converted into the local gl units.  the third unit
   // store the distance from the center of the ellipse to the sun
   static const double MAJ_MIN_AXES_TRUE[MAX_PLANETS][3];

   // defines all the major and minor axes for the planets
   // this is the same as above except this will allow all the planets
   // to be viewed in relation to one another.  the orbital path will be wrong.
   static const double MAJ_MIN_AXES_FALSE[MAX_PLANETS][3];

   // defines all the planets diameters
   // distances are converted to gl units
   static const double PLANETARY_DIAMETERS[MAX_PLANETS];

   // defines the planetary ecliptics and any axial tilts
   // first value is the ecliptic and the second is the axial
   // all values given in degrees
   static const double PLANETARY_TILTS[MAX_PLANETS][2];

   // defines all the planets time values
   // first values indicates the revolution for the planet
   // second values indicates the revolutions around the sun
   static const double PLANETARY_TIME[MAX_PLANETS][2];

   // constants that define the scale factor
   static const double GL_UNIT_TO_KM;
   static const double KM_TO_GL_UNIT;

   // constants that define the time scaling
   static const double HOURS_IN_DAY;
   static const double DAYS_IN_YEAR;
   static const double SECONDS_PER_DAY;

};

#endif // _PLANETS_WINDOW_H_
