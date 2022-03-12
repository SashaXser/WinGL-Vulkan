// local includes
#include "PlanetsWindow.h"
#include "Earth.h"
#include "Planet.h"

// wingl includes
#include "Timer.h"
#include "Vector.h"
#include "MathHelper.h"
#include "MatrixHelper.h"

// std include
#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm>

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// helper macros
#define MILLION_KM_TO_KM( x ) ((x) * 1000000.0)
#define BILLION_KM_TO_KM( x ) ((x) * 1000000000.0)

#define GL_TO_KM( x ) ((x) * PlanetsWindow::GL_UNIT_TO_KM)
#define KM_TO_GL( x ) ((x) * PlanetsWindow::KM_TO_GL_UNIT)

#define DEG_TO_RAD( x ) math::DegToRad((x))
#define DELTA_ROT_PER_SEC( x ) (2.0 * M_PI / ((x) * PlanetsWindow::SECONDS_PER_DAY))

#define HOURS_TO_DAYS( x ) ((x) / PlanetsWindow::HOURS_IN_DAY)
#define YEARS_TO_DAYS( x ) ((x) * PlanetsWindow::DAYS_IN_YEAR)

#define DEFINE_MAJOR_MINOR_DISTANCE_AXES( min, maj, convert ) \
   PlanetsWindow::DefineMinorAxis((maj), (min), (convert)), \
   PlanetsWindow::DefineMajorAxis((maj), (min), (convert)), \
   PlanetsWindow::DefineDistance((maj), (min), (convert))

// static constants
const double PlanetsWindow::GL_UNIT_TO_KM = 250000.0;
const double PlanetsWindow::KM_TO_GL_UNIT = 1.0 / GL_UNIT_TO_KM;

const double PlanetsWindow::HOURS_IN_DAY = 24.0;
const double PlanetsWindow::DAYS_IN_YEAR = 365.20833333;
const double PlanetsWindow::SECONDS_PER_DAY = 30.0;

void PlanetsWindow::DefineMajorMinorAxes( double & major, double & minor, double & distance, const bool convert_to_gl )
{
   // a typical ellipse equation
   // x^2 / a^2 + y^2 / b^2 = 1
   // a represents the major axis
   // b represents the minor axis
   // a = ( min dis + max dis ) * 0.5
   // b = sqrt( a^2 - c^2 )
   // c = a - min dis

   // obtain the major axis a
   const double a = (major + minor) * 0.5;
   // calculate the distance from the center to the sun
   const double c = a - minor;
   // calculate the minor axis b
   const double b = std::sqrt((a * a) - (c * c));

   // these values are represented in kilometers.
   // convert the values to gl units
   major    = convert_to_gl ? KM_TO_GL(a) : a;
   minor    = convert_to_gl ? KM_TO_GL(b) : b;
   distance = convert_to_gl ? KM_TO_GL(c) : c;
}

double PlanetsWindow::DefineMajorAxis( double major, double minor, const bool convert_to_gl )
{ double distance = 0.0; DefineMajorMinorAxes(major, minor, distance, convert_to_gl); return major; }

double PlanetsWindow::DefineMinorAxis( double major, double minor, const bool convert_to_gl )
{ double distance = 0.0; DefineMajorMinorAxes(major, minor, distance, convert_to_gl); return minor; }

double PlanetsWindow::DefineDistance( double major, double minor, const bool convert_to_gl )
{ double distance = 0.0; DefineMajorMinorAxes(major, minor, distance, convert_to_gl); return distance; }

const double PlanetsWindow::MAJ_MIN_AXES_TRUE[MAX_PLANETS][3] =
{
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(MILLION_KM_TO_KM(46.0),  MILLION_KM_TO_KM(69.8),   true), // mercury
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(MILLION_KM_TO_KM(108.0), MILLION_KM_TO_KM(109.0),  true), // venus
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(MILLION_KM_TO_KM(146.0), MILLION_KM_TO_KM(152.0),  true), // earth
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(MILLION_KM_TO_KM(205.0), MILLION_KM_TO_KM(249.0),  true), // mars
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(MILLION_KM_TO_KM(741.0), MILLION_KM_TO_KM(817.0),  true), // jupiter
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(BILLION_KM_TO_KM(1.35),  BILLION_KM_TO_KM(1.5),    true), // saturn
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(BILLION_KM_TO_KM(2.7),   BILLION_KM_TO_KM(3.0),    true), // uranus
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(BILLION_KM_TO_KM(4.46),  BILLION_KM_TO_KM(4.54),   true), // neptune
   DEFINE_MAJOR_MINOR_DISTANCE_AXES(BILLION_KM_TO_KM(4.437), BILLION_KM_TO_KM(7.376),  true), // pluto
   MILLION_KM_TO_KM(0.0), MILLION_KM_TO_KM(0.0), 0.0,                                         // sun
};

const double PlanetsWindow::MAJ_MIN_AXES_FALSE[MAX_PLANETS][3] =
{
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((6),  (10), false), // mercury
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((8),  (12), false), // venus
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((10), (14), false), // earth
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((12), (16), false), // mars
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((14), (18), false), // jupiter
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((16), (20), false), // saturn
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((18), (22), false), // uranus
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((20), (24), false), // neptune
   DEFINE_MAJOR_MINOR_DISTANCE_AXES((22), (26), false), // pluto
   (0.0), (0.0), 0.0,                                   // sun
};

const double PlanetsWindow::PLANETARY_DIAMETERS[MAX_PLANETS] =
{
   KM_TO_GL(4878.0),   // mercury
   KM_TO_GL(12104.0),  // venus
   KM_TO_GL(12753.0),  // earth
   KM_TO_GL(6785.0),   // mars
   KM_TO_GL(142800.0), // jupiter
   KM_TO_GL(119871.0), // saturn
   KM_TO_GL(51488.0),  // uranus
   KM_TO_GL(49493.0),  // neptune
   KM_TO_GL(2390.0),   // pluto
   KM_TO_GL(1400000.0) // sun
};

const double PlanetsWindow::PLANETARY_TILTS[MAX_PLANETS][2] =
{
   DEG_TO_RAD(7.0),     DEG_TO_RAD(0.0),     // mercury
   DEG_TO_RAD(3.4),     DEG_TO_RAD(177.34),  // venus
   DEG_TO_RAD(0.0),     DEG_TO_RAD(23.439),  // earth
   DEG_TO_RAD(1.85),    DEG_TO_RAD(25.19),   // mars
   DEG_TO_RAD(1.31),    DEG_TO_RAD(3.12),    // jupiter
   DEG_TO_RAD(2.49),    DEG_TO_RAD(26.73),   // saturn
   DEG_TO_RAD(0.77),    DEG_TO_RAD(97.86),   // uranus
   DEG_TO_RAD(1.77),    DEG_TO_RAD(29.60),   // neptune
   DEG_TO_RAD(17.184),  DEG_TO_RAD(122.46),  // pluto
   DEG_TO_RAD(0.0),     DEG_TO_RAD(7.25)     // sun
};

const double PlanetsWindow::PLANETARY_TIME[MAX_PLANETS][2] =
{
   DELTA_ROT_PER_SEC(58.65),                 DELTA_ROT_PER_SEC(YEARS_TO_DAYS(0.24)),     // mercury
   DELTA_ROT_PER_SEC(243.0),                 DELTA_ROT_PER_SEC(YEARS_TO_DAYS(0.62)),     // venus
   DELTA_ROT_PER_SEC(1.0),                   DELTA_ROT_PER_SEC(YEARS_TO_DAYS(1.0)),      // earth
   DELTA_ROT_PER_SEC(1.025),                 DELTA_ROT_PER_SEC(YEARS_TO_DAYS(1.88)),     // mars
   DELTA_ROT_PER_SEC(HOURS_TO_DAYS(9.8)),    DELTA_ROT_PER_SEC(YEARS_TO_DAYS(12.0)),     // jupiter
   DELTA_ROT_PER_SEC(HOURS_TO_DAYS(10.67)),  DELTA_ROT_PER_SEC(YEARS_TO_DAYS(29.5)),     // saturn
   DELTA_ROT_PER_SEC(HOURS_TO_DAYS(17.24)),  DELTA_ROT_PER_SEC(YEARS_TO_DAYS(84.0)),     // uranus
   DELTA_ROT_PER_SEC(HOURS_TO_DAYS(17.24)),  DELTA_ROT_PER_SEC(YEARS_TO_DAYS(165.0)),    // neptune
   DELTA_ROT_PER_SEC(6.387),                 DELTA_ROT_PER_SEC(YEARS_TO_DAYS(248.0)),    // pluto
   DELTA_ROT_PER_SEC(27.0),                  0.0                                         // sun
};

PlanetsWindow::PlanetsWindow( ) :
mpMajMinAxes            ( PlanetsWindow::MAJ_MIN_AXES_FALSE ),
mpOrbitDisplayList      ( PlanetsWindow::mOrbitDispListsFalse ),
mElapsedTimeMultiplier  ( 1.0 ),
mCamStepSpeed           ( 0.15f )
{
   std::memset(mppPlanets, 0x00, sizeof(mppPlanets));
}

PlanetsWindow::~PlanetsWindow( )
{
}

void PlanetsWindow::OnDestroy( )
{
   // must destroy other stuff here
   std::for_each(mppPlanets, mppPlanets + sizeof(mppPlanets) / sizeof(*mppPlanets),
      [ ] ( Planet * const pPlanet ) { delete pPlanet; });

   // release the display lists
   glDeleteLists(mOrbitDispListsTrue[0], sizeof(mOrbitDispListsTrue) / sizeof(*mOrbitDispListsTrue));
   glDeleteLists(mOrbitDispListsFalse[0], sizeof(mOrbitDispListsFalse) / sizeof(*mOrbitDispListsFalse));

   // call the base class to clean things up
   OpenGLWindow::OnDestroy();
}

bool PlanetsWindow::Create( unsigned int nWidth,
                            unsigned int nHeight,
                            const char * pWndTitle,
                            const void * /*pInitParams*/ )
{
   // initialize the opengl context
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false }, 
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // create the specific data
      GenerateSceneData();
      GenerateOrbitalDisplayLists();

      // enable global states
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      // default the view matrix
      mViewMat.MakeLookAt(Vec3f(20.0f, 20.0f, 40.0f),
                          Vec3f(0.0f, 0.0f, 0.0f),
                          Vec3f(0.0f, 1.0f, 0.0f));

      // indicate the controls
      std::cout << std::endl
                << "Controls:" << std::endl
                << "asdw - move camera" << std::endl
                << "lbutton down - orientate camera" << std::endl
                << "+ / - - increase / decrease simulation time" << std::endl
                << "] / [ - increase / decrease camera step";
      
      return true;
   }
   else
   {
      // issue an error from the application that it could not be created
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, "Unable To Create 4.4 OpenGL Context");

      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int PlanetsWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   // stores the previous run time to calculate the elapsed time
   double previous_time_sec = Timer().GetCurrentTimeSec();

   while (!bQuit)
   {
      // get the current time
      const double current_time_sec = Timer().GetCurrentTimeSec();

      // process all the app messages and then render the scene
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
      {
         // render the scene
         RenderScene(current_time_sec - previous_time_sec);
      }

      // save the previous time
      previous_time_sec = current_time_sec;
   }

   return appQuitVal;
}

LRESULT PlanetsWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      {

      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(lParam & 0xFFFF),
                 static_cast< GLsizei >(lParam >> 16));

      // update the projection matrix
      const float aspect = static_cast< float >(lParam & 0xFFFF) / static_cast< float >(lParam >> 16);
      mProjMat.MakePerspective(45.0f, aspect, 0.1f, 300000.0f);

      }

      break;

   case WM_MOUSEMOVE:
      {

      // get current mouse positions
      const int32_t current_mouse_x = static_cast< int32_t >(lParam & 0xFFFF);
      const int32_t current_mouse_y = static_cast< int32_t >(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // get the delta between current and previous positions
         const auto delta_x = current_mouse_x - GetPreviousMousePosition().x;
         const auto delta_y = current_mouse_y - GetPreviousMousePosition().y;

         // obtain the current yaw and pitch rotations from the view matrix
         float view_yaw = 0.0f, view_pitch = 0.0f;
         MatrixHelper::DecomposeYawPitchRollDeg< float >(mViewMat, &view_yaw, &view_pitch, nullptr);

         // update the yaw and pitch values
         // need to make this more granular
         // need to get the elapsed time here
         view_yaw += delta_x * 0.05f;
         view_pitch += delta_y * 0.05f;

         // make sure the value for pitch is within [-90, 90]
         view_pitch = math::Clamp(view_pitch, -89.9f, 89.9f);

         // obtain the current position of the camera
         const Vec4f camera_pos = mViewMat.Inverse() * Vec4f();

         // construct the new view matrix
         mViewMat = (Matrixf::Translate(Vec3f(camera_pos)) *
                     Matrixf::Rotate(view_yaw, 0.0f, 1.0f, 0.0f) *
                     Matrixf::Rotate(view_pitch, 1.0f, 0.0f, 0.0f)).Inverse();
      }

      }

      break;

   case WM_CHAR:

      // switch based on aswd being pressed
      switch (wParam)
      {
      case 'a':
      case 'd':
         // take the inverse of the view matrix to get into world space
         mViewMat.MakeInverse();

         // strafe translate based on the current view matrix
         mViewMat = (mViewMat * Matrixf::Translate(wParam == 'a' ? -mCamStepSpeed : mCamStepSpeed, 0.0f, 0.0f)).Inverse();

         break;

      case 'w':
      case 's':
         // take the inverse of the view matrix to get into world space
         mViewMat.MakeInverse();

         // view translate based on the current view matrix
         mViewMat = (mViewMat * Matrixf::Translate(0.0f, 0.0f, wParam == 'w' ? -mCamStepSpeed : mCamStepSpeed)).Inverse();

         break;

      case '=':
      case '-':
         // increase / decrease the multiplier
         mElapsedTimeMultiplier = math::Clamp(mElapsedTimeMultiplier * (wParam == '=' ? 10.0 : 0.1), 0.01, 100000.0);

         break;

      case '[':
      case ']':
         // increase / decrease the camera step speed
         mCamStepSpeed = math::Clamp(mCamStepSpeed + (wParam == ']' ? 0.001f : -0.001f), 0.001f, 1.0f);

         break;

      case '{':
      case '}':
         // increase / decrease the camera step speed
         mCamStepSpeed = math::Clamp(mCamStepSpeed + (wParam == '}' ? 0.01f : -0.01f), 0.001f, 1.0f);

         break;
      }

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

void PlanetsWindow::RenderScene( const double elapsed_time_sec )
{
   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // update and render the scene
   UpdateScene(elapsed_time_sec);
   DrawScene(elapsed_time_sec);

   // swap the front and back
   SwapBuffers(GetHDC());
}

void PlanetsWindow::UpdateScene( const double elapsed_time_sec )
{
   // elapsed time for the frame
   const double sim_elapsed_time_secs = elapsed_time_sec * mElapsedTimeMultiplier;

   // the world position of the sun
   const Vec3f world_pos_sun = mppPlanets[SUN]->GetWorldPosition();

   // the lighting matrix
   const Matrixf lighting_matrix = mViewMat.Inverse().Transpose();

   Planet ** pPlanet = mppPlanets;
   for (int i = 0; i < MAX_PLANETS; ++i, ++pPlanet)
   {
      if (i != SUN)
      {
         // let the planet know the sun's location
         (*pPlanet)->UpdateSunWorldPosition(world_pos_sun);
         // update the lighting matrix based on the view matrix
         (*pPlanet)->UpdateWorldToEyeSpaceLighting(lighting_matrix);
      }

      // update the planet
      (*pPlanet)->Update(elapsed_time_sec, sim_elapsed_time_secs);
   }
}

void PlanetsWindow::DrawScene( const double /*elapsed_time_sec*/ )
{
   // must update to be more modern...  do that later...

   // update the projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(mProjMat);

   // update the modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(mViewMat);

   // obtain a pointer to all the planets
   Planet ** pPlanet = mppPlanets;

   for (uint32_t i = 0; i < MAX_PLANETS; ++i, ++pPlanet)
   {
      // render the planet
      (*pPlanet)->Render();

      if (i != SUN)
      {
         // push the matrix to the stack
         glPushMatrix();
         
         // rotate the view by the planetary tilt
         glMultMatrixf((*pPlanet)->GetOrbitalTiltMatrix());
         // translate the orbit by the distance from the sun
         glTranslatef(static_cast< float >(mpMajMinAxes[i][2]), 0.0f, 0.0f);
         
         // call the display list to render the orbits
         glCallList(mpOrbitDisplayList[i]);

         // remove the matrix from the stack
         glPopMatrix();
      }
   }
}

void PlanetsWindow::GenerateSceneData( )
{
   // create mercury
   mppPlanets[MERCURY] = new Planet("Textures\\MercuryColor.jpg",
                                    static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[MERCURY]),
                                    PlanetsWindow::PLANETARY_TILTS[MERCURY],
                                    PlanetsWindow::PLANETARY_TIME[MERCURY],
                                    mpMajMinAxes[MERCURY]);

   // create venus
   mppPlanets[VENUS] = new Planet("Textures\\VenusColor.jpg",
                                  static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[VENUS]),
                                  PlanetsWindow::PLANETARY_TILTS[VENUS],
                                  PlanetsWindow::PLANETARY_TIME[VENUS],
                                  mpMajMinAxes[VENUS]);
   
   // create earth
   mppPlanets[EARTH] = new Earth("Textures\\EarthColor.jpg",
                                 "Textures\\EarthLights.jpg",
                                 "Textures\\EarthClouds.jpg",
                                 static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[EARTH]),
                                 PlanetsWindow::PLANETARY_TILTS[EARTH],
                                 PlanetsWindow::PLANETARY_TIME[EARTH],
                                 mpMajMinAxes[EARTH],
                                 "earth.vert", "earth.frag",
                                 2.5, 1.0);
   
   // create mars
   mppPlanets[MARS] = new Planet("Textures\\MarsColor.jpg",
                                 static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[MARS]),
                                 PlanetsWindow::PLANETARY_TILTS[MARS],
                                 PlanetsWindow::PLANETARY_TIME[MARS],
                                 mpMajMinAxes[MARS]);

   // create jupiter
   mppPlanets[JUPITER] = new Planet("Textures\\JupiterColor.jpg",
                                    static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[JUPITER]),
                                    PlanetsWindow::PLANETARY_TILTS[JUPITER],
                                    PlanetsWindow::PLANETARY_TIME[JUPITER],
                                    mpMajMinAxes[JUPITER]);

   // create saturn
   mppPlanets[SATURN] = new Planet("Textures\\SaturnColor.jpg",
                                   static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[SATURN]),
                                   PlanetsWindow::PLANETARY_TILTS[SATURN],
                                   PlanetsWindow::PLANETARY_TIME[SATURN],
                                   mpMajMinAxes[SATURN]);

   // create uranus
   mppPlanets[URANUS] = new Planet("Textures\\UranusColor.jpg",
                                   static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[URANUS]),
                                   PlanetsWindow::PLANETARY_TILTS[URANUS],
                                   PlanetsWindow::PLANETARY_TIME[URANUS],
                                   mpMajMinAxes[URANUS]);

   // create neptune
   mppPlanets[NEPTUNE] = new Planet("Textures\\NeptuneColor.jpg",
                                    static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[NEPTUNE]),
                                    PlanetsWindow::PLANETARY_TILTS[NEPTUNE],
                                    PlanetsWindow::PLANETARY_TIME[NEPTUNE],
                                    mpMajMinAxes[NEPTUNE]);

   // create pluto
   mppPlanets[PLUTO] = new Planet("Textures\\PlutoColor.jpg",
                                  static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[PLUTO]),
                                  PlanetsWindow::PLANETARY_TILTS[PLUTO],
                                  PlanetsWindow::PLANETARY_TIME[PLUTO],
                                  mpMajMinAxes[PLUTO]);

   // create the sun
   mppPlanets[SUN] = new Planet("Textures\\SunColor.jpg",
                                static_cast< float >(PlanetsWindow::PLANETARY_DIAMETERS[SUN]),
                                PlanetsWindow::PLANETARY_TILTS[SUN],
                                PlanetsWindow::PLANETARY_TIME[SUN],
                                mpMajMinAxes[SUN],
                                "sun.vert", "sun.frag");
}

void PlanetsWindow::GenerateOrbitalDisplayLists( )
{
   // this data should move into its own class

   // constants that define the orbit
   const double delta_rad = math::DegToRad(0.05);
   const double two_pi_rad = math::DegToRad(360.0);

   // render each of the orbits
   for (int i = 0; i < 2; ++i)
   {
      // determine the orbital display list to construct
      uint32_t * const pOrbitDispList = i == 0 ? mOrbitDispListsTrue : mOrbitDispListsFalse;
      const double (* const pMajMinAxes)[3] = i == 0 ? MAJ_MIN_AXES_TRUE : MAJ_MIN_AXES_FALSE;

      for (int j = 0; j < 9; ++j)
      {
         // request a new display list
         pOrbitDispList[j] = glGenLists(1);

         // begin creating the new list
         glNewList(pOrbitDispList[j], GL_COMPILE);

         // set the color to white
         glColor3f(1.0f, 1.0f, 1.0f);

         // begin rendering a line loop
         glBegin(GL_LINE_LOOP);

         // get the major and minor axis
         const double & dMinor = (*(pMajMinAxes + j))[0];
         const double & dMajor = (*(pMajMinAxes + j))[1];

         // render a complete circle
         for (double rotation_rad = 0.0; rotation_rad < two_pi_rad; rotation_rad += delta_rad)
         {
            glVertex3d(dMajor * std::cos(rotation_rad),
                       0.0,
                       dMinor * std::sin(rotation_rad));
         }

         // end the rendering
         glEnd();

         // end the list
         glEndList();
      }
   }
}
