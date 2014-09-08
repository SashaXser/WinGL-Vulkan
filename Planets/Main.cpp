// project includes
#include "StdAfx.h"
#include "QuickGLWindow.h"
#include "Planet.h"

// public macros
#define MILLION_KM_TO_KM( x ) ((x) * 1000000.0)
#define BILLION_KM_TO_KM( x ) ((x) * 1000000000.0)

#define GL_TO_KM( x ) ((x) * k_dGL_UNIT_TO_KM)
#define KM_TO_GL( x ) ((x) * k_dKM_TO_GL_UNIT)

#define DELTA_ROT_PER_SEC( x ) (k_d2PI / ((x) * k_dSECONDS_PER_DAY))

#define HOURS_TO_DAYS( x ) ((x) / k_dHOURS_IN_DAY)
#define YEARS_TO_DAYS( x ) ((x) * k_dDAYS_IN_YEAR)

#define DEG_TO_RAD( x ) ((x) * k_dPI / 180.0)
#define RAD_TO_DEG( x ) ((x) * 180.0 / k_dPI)

// constants that define the scale factor
const double k_dGL_UNIT_TO_KM = 250000.0;
const double k_dKM_TO_GL_UNIT = 1.0 / k_dGL_UNIT_TO_KM;

// constants that define the time scaling
const double k_dHOURS_IN_DAY = 24.0;
const double k_dDAYS_IN_YEAR = 365.20833333;

const double k_dSECONDS_PER_DAY = 15.0;

// math constants
const double k_d2PI = 6.283185307179586476925286766559;
const double k_dPI = 3.1415926535897932384626433832795;

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

// defines all the major and minor axes for the planets
// each planet starts with the minimum and maximum solar
// distances in kilometers.  these values will then be
// converted into the local gl units.  the third unit
// store the distance from the center of the ellipse to the sun
double g_dMajMinAxesTrue[MAX_PLANETS][3] =
{
   MILLION_KM_TO_KM(46.0),  MILLION_KM_TO_KM(69.8),  0.0, // mercury
   MILLION_KM_TO_KM(108.0), MILLION_KM_TO_KM(109.0), 0.0, // venus
   MILLION_KM_TO_KM(146.0), MILLION_KM_TO_KM(152.0), 0.0, // earth
   MILLION_KM_TO_KM(205.0), MILLION_KM_TO_KM(249.0), 0.0, // mars
   MILLION_KM_TO_KM(741.0), MILLION_KM_TO_KM(817.0), 0.0, // jupiter
   BILLION_KM_TO_KM(1.35),  BILLION_KM_TO_KM(1.5),   0.0, // saturn
   BILLION_KM_TO_KM(2.7),   BILLION_KM_TO_KM(3.0),   0.0, // uranus
   BILLION_KM_TO_KM(4.46),  BILLION_KM_TO_KM(4.54),  0.0, // neptune
   BILLION_KM_TO_KM(4.437), BILLION_KM_TO_KM(7.376), 0.0, // pluto
   MILLION_KM_TO_KM(0.0),   MILLION_KM_TO_KM(0.0),   0.0  // sun
};

// defines all the major and minor axes for the planets
// this is the same as above except this will allow all the planets
// to be viewed in relation to one another.  the orbital path will be wrong.
double g_dMajMinAxesFalse[MAX_PLANETS][3] =
{
   (6),   (10),  0.0, // mercury
   (8),   (12),  0.0, // venus
   (10),  (14),  0.0, // earth
   (12),  (16),  0.0, // mars
   (14),  (18),  0.0, // jupiter
   (16),  (20),  0.0, // saturn
   (18),  (22),  0.0, // uranus
   (20),  (24),  0.0, // neptune
   (22),  (26),  0.0, // pluto
   (0.0), (0.0), 0.0  // sun
};

// defines the major / minor axes pointer
double (*g_pMajMinAxes)[3] = g_dMajMinAxesFalse;
//double (*g_pMajMinAxes)[3] = g_dMajMinAxesTrue;

// defines all the planets diameters
// distances are converted to gl units
double g_dDiameters[MAX_PLANETS] =
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
   KM_TO_GL(1400000)   // sun
};

// defines the planetary ecliptics and any axial tilts
// first value is the ecliptic and the second is the axial
// all values given in degrees
double g_dPlanetaryTilts[MAX_PLANETS][2] =
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

// defines all the planets time values
// first values indicates the revolution for the planet
// second values indicates the revolutions around the sun
double g_dPlanetaryTime[MAX_PLANETS][2] =
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

// defines the matrices for each of the nine planets
// first value indicates the planets ecliptic
// second value indicates the planets tilt
// third value indicates the planets rotation on axis
// forth value indicates the planets position
Matrix g_matPlanetaryMatrix[MAX_PLANETS][4];

// globals
QuickGLWindow *   g_pGLWindow = NULL;

// defines the display lists that will render the orbits
unsigned int g_nOrbitDispListsTrue[MAX_PLANETS - 1] = { 0 };
unsigned int g_nOrbitDispListsFalse[MAX_PLANETS - 1] = { 0 };
// defines the orbit display list pointer
unsigned int * g_pOrbitDisplayList = g_nOrbitDispListsFalse;
//unsigned int * g_pOrbitDisplayList = g_nOrbitDispListsTrue;
// defines the elapsed time multiplier
double g_dElapsedTimeMultiplier = 1;

// defines all the planets that will be rendered
Planet *    g_ppPlanets[MAX_PLANETS] = { 0 };

// window callbacks
void ShutdownCB( void * pClass );
void UpdateCB( const double & rElapsedTime );
void DrawCB( void * pClass, const double & rElapsedTime );

void MouseCB( void * pClass,
              const QuickMouse::MouseInput * pData,
              const double & rElapsedTime );
void KeyboardCB( void * pClass,
                 const QuickKeyboard::KeyboardInput * pData,
                 const double & rElapsedTime );

// sets the keyboard bindings
void SetupKeyboardBindings( QuickKeyboard & rKeyboard );
// setup the perspective view settings
void SetupPerspectiveView( Camera & rCamera );
// setup the major and minor axis for each of the planets
void ConstructMajorMinorAxes( );
// sets up the display list for the orbits
void ConstructOrbitDisplayLists( double (*pMajMinAxes)[3],
                                 unsigned int * pOrbitDispList );
// constructs the planets
void ConstructPlanets( );
// constructs the planets matrices
void ConstructPlanetMatrices( );

// define the main entry point
int __stdcall WinMain( HINSTANCE hInstance,
                       HINSTANCE hPrevious,
                       LPSTR lpCmdLine,
                       int nShowCmd )
{
   // create a new gl window
   g_pGLWindow = new QuickGLWindow(hInstance,
                                   //1280, 800,
                                   1024, 768,
                                   //800, 600,
                                   //640, 480,
                                   "Planets",
                                   //true
                                   false
                                   );

   // setup the window callbacks
   g_pGLWindow->SetUpdateCallback(UpdateCB);
   g_pGLWindow->SetDrawCallback(NULL, DrawCB);
   g_pGLWindow->SetMouseCallback(NULL, MouseCB);
   g_pGLWindow->SetShutdownCallback(NULL, ShutdownCB);
   g_pGLWindow->SetKeyboardCallback(NULL, KeyboardCB);

   // setup the keyboard bindings
   SetupKeyboardBindings(g_pGLWindow->GetKeyboard());

   // setup the perspective view settings
   SetupPerspectiveView(g_pGLWindow->GetCamera());

   // construct the major and minor axes
   ConstructMajorMinorAxes();

   // construct the planetary orbit display lists
   ConstructOrbitDisplayLists(g_dMajMinAxesTrue, g_nOrbitDispListsTrue);
   ConstructOrbitDisplayLists(g_dMajMinAxesFalse, g_nOrbitDispListsFalse);

   // construct the planetss
   ConstructPlanets();

   // construct the planets' matrices
   ConstructPlanetMatrices();

   // enable global states
   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   // begin running the app
   g_pGLWindow->Run();

   // release the window
   delete g_pGLWindow;

   return 0;
}

void ShutdownCB( void * pClass )
{
   Planet ** pPlanet = g_ppPlanets;

   for (int i = 0; i < MAX_PLANETS; ++i)
   {
      delete *(pPlanet++);
   }
}

void UpdateCB( const double & rElapsedTime )
{
   // elapsed time for the frame
   double dElapsedTime = rElapsedTime * g_dElapsedTimeMultiplier;

   // create a staic time variable for position
   static double fTimePos[10] = { 0.0 };

   Planet ** pPlanet = g_ppPlanets;

   for (int i = 0; i < MAX_PLANETS; ++i)
   {
      // update the time position
      fTimePos[i] += (dElapsedTime * g_dPlanetaryTime[i][1]);

      // construct the translation matrix
      MATRIX_TRANSLATE(g_matPlanetaryMatrix[i][3],
                      (float)(cos(fTimePos[i]) * (*(g_pMajMinAxes + i))[1]),
                       0.0f,
                      (float)(sin(fTimePos[i]) * -(*(g_pMajMinAxes + i))[0]));

      // compute a planetary rotation
      Matrix mRotation;
      MATRIX_ROTATE_Y(mRotation, Angle(AngleType::RADIANS,
                                      (float)(g_dPlanetaryTime[i][0] * dElapsedTime)));

      // update the local rotation
      g_matPlanetaryMatrix[i][2] = g_matPlanetaryMatrix[i][2] * mRotation;

      // update the planet
      (*(pPlanet++))->Update(rElapsedTime);
   }
}

void DrawCB( void * pClass,
             const double & rElapsedTime )
{
   // obtain a pointer to all the planets
   Planet ** pPlanet = g_ppPlanets;

   for (unsigned int i = 0; i < MAX_PLANETS; ++i)
   {
      // push the matrix to the stack
      glPushMatrix();

      // construct the world matrix for the planet
      Matrix mWorld = g_matPlanetaryMatrix[i][0] * g_matPlanetaryMatrix[i][3];
      Matrix mTrans; MATRIX_TRANSLATE(mTrans, (*(g_pMajMinAxes + i))[2], 0, 0);
      mWorld = mWorld * mTrans;

      // construct the local matrix for the planet
      Matrix mLocal = g_matPlanetaryMatrix[i][1] * g_matPlanetaryMatrix[i][2];

      // multiply the current matrix stack by the world then the local
      glMultMatrixf(mWorld.GetMatrixPtr());
      glMultMatrixf(mLocal.GetMatrixPtr());

      // render the planet
      (*(pPlanet++))->Render();

      glPointSize(4);
      glBegin(GL_POINTS);
      glVertex3f(0,0,0);
      glEnd();

      // remove the matrix from the stack
      glPopMatrix();

      if (i != SUN)
      {
         // push the matrix to the stack
         glPushMatrix();
         
         // rotate the view by the planetary tilt
         glMultMatrixf(g_matPlanetaryMatrix[i][0].GetMatrixPtr());
         // translate the orbit by the distance from the sun
         glTranslatef((*(g_pMajMinAxes + i))[2], 0.0f, 0.0f);
         
         // call the display list to render the orbits
         glCallList(g_pOrbitDisplayList[i]);

         // remove the matrix from the stack
         glPopMatrix();
      }
   }
}

void MouseCB( void * pClass,
              const QuickMouse::MouseInput * pData,
              const double & rElapsedTime )
{
   for (QuickMouse::MouseInput::const_iterator itBeg = pData->begin(),
                                               itEnd = pData->end();
        itBeg != itEnd;
        ++itBeg)
   {
      switch (itBeg->nType)
      {
      case QuickMouse::QMBT_XAXIS:
         {
            g_pGLWindow->GetCamera().YawRotation(Angle(AngleType::DEGREES,
                                                       -40.0 * rElapsedTime * itBeg->Data.unAxis));
         }

         break;

      case QuickMouse::QMBT_YAXIS:
         g_pGLWindow->GetCamera().PitchRotation(Angle(AngleType::DEGREES,
                                                      40.0 * rElapsedTime * itBeg->Data.unAxis));

         break;

      case QuickMouse::QMBT_ZAXIS:
         g_pGLWindow->GetCamera().RollRotation(Angle(AngleType::DEGREES,
                                                     10.0 * rElapsedTime * itBeg->Data.unAxis));

         break;

      case QuickMouse::QMBT_BUTTON1:
         if (itBeg->Data.bState && g_dElapsedTimeMultiplier < 100000.0)
         {
            g_dElapsedTimeMultiplier /= 0.1;
         }

         break;

      case QuickMouse::QMBT_BUTTON0:
         if (itBeg->Data.bState && g_dElapsedTimeMultiplier > 0.01)
         {
            g_dElapsedTimeMultiplier *= 0.1;
         }

         break;
      }
   }
}

void KeyboardCB( void * pClass,
                 const QuickKeyboard::KeyboardInput * pData,
                 const double & rElapsedTime )
{
   // local typedefs
   typedef QuickKeyboard::KeyboardInput::const_iterator ConstKBIter;

   for (ConstKBIter itKeyBeg = pData->begin(),
                    itKeyEnd = pData->end();
        itKeyBeg != itKeyEnd;
        ++itKeyBeg)
   {
      switch (*itKeyBeg)
      {
      case DIK_ESCAPE:
         g_pGLWindow->ShutdownApp();

         break;

      case DIK_A:
         g_pGLWindow->GetCamera().TranslateX((float)(-10.0 * rElapsedTime));

         break;

      case DIK_D:
         g_pGLWindow->GetCamera().TranslateX((float)(10.0 * rElapsedTime));

         break;

      case DIK_W:
         g_pGLWindow->GetCamera().TranslateZ((float)(-10.0 * rElapsedTime));

         break;

      case DIK_S:
         g_pGLWindow->GetCamera().TranslateZ((float)(10.0 * rElapsedTime));

         break;
      }
   }
}

void SetupPerspectiveView( Camera & rCamera )
{
   rCamera.SetPerspective(Angle(AngleType::DEGREES, 45),
                          1024.0f / 768.0f,
                          0.1f, 300000.0f);
   rCamera.SetCamera(Vector(0.0f, 0.0, 21.1f),
                     Vector(0.0f, 0.0f, 0.0f),
                     Vector(0.0f, 1.0f, 0.0f));
}

void SetupKeyboardBindings( QuickKeyboard & rKeyboard )
{
   // keyboard keys to process
   unsigned int nKeys[] =
   {
      DIK_ESCAPE,     // shutdown application
      DIK_A,          // strafe to the left
      DIK_D,          // strafe to the right
      DIK_W,          // move forward
      DIK_S           // move backwards
   };

   // setup the keyboard keys to process
   rKeyboard.AddKeys(nKeys, sizeof(nKeys) / sizeof(unsigned int));
}

void ConstructMajorMinorAxes( )
{
   // a typical ellipse equation
   // x^2 / a^2 + y^2 / b^2 = 1
   // a represents the major axis
   // b represents the minor axis
   // a = ( min dis + max dis ) * 0.5
   // b = sqrt( a^2 - c^2 )
   // c = a - min dis
   
   // calculate for each of the nine planets the major and minor axes
   for (int i = 0; i < 9; i++)
   {
      // obtain the major and minor values
      double * pMinor = &g_dMajMinAxesTrue[i][0];
      double * pMajor = &g_dMajMinAxesTrue[i][1];
      double * pDisToSun = &g_dMajMinAxesTrue[i][2];

      // obtain the major axis a
      double a = (*pMinor + *pMajor) * 0.5;
      // calculate the distance from the center to the sun
      double c = a - *pMinor;
      // calculate the minor axis b
      double b = std::sqrt((a * a) - (c * c));

      // these values are represented in kilometers.
      // convert the values to gl units
      *pMajor    = KM_TO_GL(a);
      *pMinor    = KM_TO_GL(b);
      *pDisToSun = KM_TO_GL(c);

      // obtain the major and minor false values
      pMinor = &g_dMajMinAxesFalse[i][0];
      pMajor = &g_dMajMinAxesFalse[i][1];
      pDisToSun = &g_dMajMinAxesFalse[i][2];

      if (*pMinor != *pMajor)
      {
         // obtain the major axis a
         a = (*pMinor + *pMajor) * 0.5;
         // calculate the distance from the center to the sun
         c = a - *pMinor;
         // calculate the minor axis b
         b = std::sqrt((a * a) - (c * c));

         // these values are the false gl values
         *pMajor = a;
         *pMinor = b;
         *pDisToSun = c;
      }
   }
}

void ConstructOrbitDisplayLists( double (*pMajMinAxes)[3],
                                 unsigned int * pOrbitDispList )
{
   // local(s)
   Angle a2PI(AngleType::DEGREES, 360.0);
   Angle aDelta(AngleType::DEGREES, 0.05);

   // convert to radians
   a2PI.SetUnit(AngleType::RADIANS);
   aDelta.SetUnit(AngleType::RADIANS);

   // render each of the orbits
   for (int i = 0; i < 9; i++)
   {
      // request a new display list
      pOrbitDispList[i] = glGenLists(1);

      // begin creating the new list
      glNewList(pOrbitDispList[i], GL_COMPILE);

      // set the color to white
      glColor3f(1.0f, 1.0f, 1.0f);

      // begin rendering a line loop
      glBegin(GL_LINE_LOOP);

      // get the major and minor axis
      double dMinor = (*(pMajMinAxes + i))[0];
      double dMajor = (*(pMajMinAxes + i))[1];

      // render a complete circle
      for (Angle aRadian(AngleType::RADIANS, 0.0);
           aRadian.GetValue() < a2PI.GetValue();
           aRadian.SetValue(aRadian.GetValue() + aDelta.GetValue()))
      {
         glVertex3d(dMajor * std::cos(aRadian.GetValue()),
                    0.0,
                    dMinor * std::sin(aRadian.GetValue()));
      }

      // end the rendering
      glEnd();

      // end the list
      glEndList();
   }
}

void ConstructPlanets( )
{
   // local(s)
   float fDiameter = 0.0f;

   // create mercury
   fDiameter = (float)g_dDiameters[MERCURY];

   g_ppPlanets[MERCURY] = new Planet("Textures\\MercuryColor.jpg", fDiameter);

   // create venus
   fDiameter = (float)g_dDiameters[VENUS];

   g_ppPlanets[VENUS] = new Planet("Textures\\VenusColor.jpg", fDiameter);
   
   // create earth
   fDiameter = (float)g_dDiameters[EARTH];

   g_ppPlanets[EARTH] = new Planet("Textures\\EarthColor.jpg",
                                   fDiameter,
                                   Angle(AngleType::DEGREES, 5.0),
                                   Angle(AngleType::DEGREES, 1.0));
   
   // create mars
   fDiameter = (float)g_dDiameters[MARS];

   g_ppPlanets[MARS] = new Planet("Textures\\MarsColor.jpg", fDiameter);

   // create jupiter
   fDiameter = (float)g_dDiameters[JUPITER];

   g_ppPlanets[JUPITER] = new Planet("Textures\\JupiterColor.jpg", fDiameter);

   // create saturn
   fDiameter = (float)g_dDiameters[SATURN];

   g_ppPlanets[SATURN] = new Planet("Textures\\SaturnColor.jpg", fDiameter);

   // create uranus
   fDiameter = (float)g_dDiameters[URANUS];

   g_ppPlanets[URANUS] = new Planet("Textures\\UranusColor.jpg", fDiameter);

   // create neptune
   fDiameter = (float)g_dDiameters[NEPTUNE];

   g_ppPlanets[NEPTUNE] = new Planet("Textures\\NeptuneColor.jpg", fDiameter);

   // create pluto
   fDiameter = (float)g_dDiameters[PLUTO];

   g_ppPlanets[PLUTO] = new Planet("Textures\\PlutoColor.jpg", fDiameter);

   // create the sun
   fDiameter = (float)g_dDiameters[SUN];

   g_ppPlanets[SUN] = new Planet("Textures\\SunColor.jpg", fDiameter);
}

void ConstructPlanetMatrices( )
{
   for (unsigned int i = 0; i < MAX_PLANETS; i++)
   {
      // obtain references to each of the matrices
      Matrix & rMatEcliptic = g_matPlanetaryMatrix[i][0];
      Matrix & rMatTilt     = g_matPlanetaryMatrix[i][1];
      Matrix & rMatRotation = g_matPlanetaryMatrix[i][2];
      Matrix & rMatTrans    = g_matPlanetaryMatrix[i][3];

      // setup the ecliptic
      MATRIX_ROTATE_Z(rMatEcliptic, Angle(AngleType::RADIANS, g_dPlanetaryTilts[i][0]));
      // setup the planet's tilt
      MATRIX_ROTATE_Z(rMatTilt, Angle(AngleType::RADIANS, g_dPlanetaryTilts[i][1]));
      // set the last matrix to the identity
      rMatRotation.Identity();
      rMatTrans.Identity();
   }
}