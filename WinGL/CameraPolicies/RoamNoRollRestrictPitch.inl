// local includes
#include "../Vector.h"
#include "../Quaternion.h"
#include "../MathHelper.h"
#include "../MatrixHelper.h"

namespace camera_policy
{

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::RoamNoRollRestrictPitch( ) :
MIN_PITCH   ( static_cast< T >(MIN_PITCH_RATIO::num) / static_cast< T >(MIN_PITCH_RATIO::den) ),
MAX_PITCH   ( static_cast< T >(MAX_PITCH_RATIO::num) / static_cast< T >(MAX_PITCH_RATIO::den) )
{
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::RoamNoRollRestrictPitch( const RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > & policy ) :
RoamNoRollRestrictPitch()
{
   mViewMatrix = policy.mViewMatrix;
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::~RoamNoRollRestrictPitch( )
{
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > &
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::operator = ( const RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > & policy )
{
   mViewMatrix = policy.mViewMatrix;
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::LookAt( const vec_type & eye, const vec_type & center )
{
   mViewMatrix.MakeLookAt(eye, center, UP_VECTOR::up);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::TranslateRight( const type amount )
{
   Translate< 0 >(amount);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::TranslateForward( const type amount )
{
   Translate< 8 >(amount);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::TranslateUp( const type amount )
{
   Translate< 4 >(amount);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::Translate( const type amount, const vec_type & direction )
{
   // invert the camera matrix
   mViewMatrix.MakeInverse();

   // calculate the new translation
   const vec_type translation_vec = axis_vector * amount;

   // update the matrix
   const mat_type translation_mat(Vector4< T >(1, 0, 0, 0),
                                  Vector4< T >(0, 1, 0, 0),
                                  Vector4< T >(0, 0, 1, 0),
                                  Vector4< T >(translation_vec, 1));

   // in the current orientation of the camera, translate off of the basis vectors
   mViewMatrix = translation_mat * mViewMatrix;

   // invert the camera matrix back
   mViewMatrix.MakeInverse();
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::SetYaw( const type yaw_deg )
{
   // get the current ypr values
   const vec_type ypr = MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix);

   // update the matrix with the new yaw value
   Rotate(yaw_deg, ypr.Y());
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::SetPitch( const type pitch_deg )
{
   // get the current ypr values
   vec_type ypr = MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix);

   // make sure the pitch is within bounds
   ypr.Y() = math::Clamp(pitch_deg, MIN_PITCH, MAX_PITCH);

   // update the matrix with the new yaw value
   Rotate(ypr.X(), ypr.Y());
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::SetRoll( const type roll_deg )
{
   // roll does not influence this camera type... no action taken...
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::RotateYaw( const type amount_deg )
{
   // get the current ypr values
   const vec_type ypr = MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix);

   // increase yaw by the requested amount
   SetYaw(ypr.X() + amount_deg);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::RotatePitch( const type amount_deg )
{
   // get the current ypr values
   const vec_type ypr = MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix);

   // increase yaw by the requested amount
   SetPitch(ypr.Y() + amount_deg);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::RotateRoll( const type amount_deg )
{
   // roll does not influence this camera type... no action taken...
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
typename RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::type
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::GetYaw( ) const
{
   return MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix).X();
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
typename RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::type
RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR >::GetPitch( ) const
{
   return MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix).Y();
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
typename RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::type
RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::GetRoll( ) const
{
   return MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix).Z();
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::GetYPR( type * const yaw_deg, type * const pitch_deg, type * const roll_deg ) const
{
   MatrixHelper::DecomposeYawPitchRollDeg(mViewMatrix, yaw_deg, pitch_deg, roll_deg);
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
typename RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::vec_type
RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::GetEyePosition( ) const
{
   return vec_type(mViewMatrix.Inverse() * Vector< T, 4 >(T(0), T(0), T(0), T(1)));
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
typename RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::mat_type
RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::GetViewMatrix( ) const
{
   return mViewMatrix;
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
template < uint32_t AXIS >
void RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::Translate( const type amount )
{
   // invert the camera matrix
   mViewMatrix.MakeInverse();

   // get the eye and axis vector
   const vec_type eye_vector(mViewMatrix[12], mViewMatrix[13], mViewMatrix[14]);
   const vec_type axis_vector(mViewMatrix[AXIS], mViewMatrix[AXIS + 1], mViewMatrix[AXIS + 2]);

   // calculate the new eye
   const vec_type new_eye_vector = eye_vector + axis_vector * amount;

   // update the matrix
   mViewMatrix[12] = new_eye_vector.X();
   mViewMatrix[13] = new_eye_vector.Y();
   mViewMatrix[14] = new_eye_vector.Z();

   // invert the camera matrix back
   mViewMatrix.MakeInverse();
}

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >
void RoamNoRollRestrictPitch< T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR >::Rotate( const type yaw_deg, const type pitch_deg )
{
   // the result of the rotation of yaw and pitch
   const Quaternion< T > rotation =
      Quaternion< T >::Rotation(yaw_deg, UP_VECTOR::up) %
      Quaternion< T >::Rotation(pitch_deg, vec_type(T(1), T(0), T(0)));

   // invert the camera matrix
   mViewMatrix.MakeInverse();

   // get the eye location
   const vec_type eye(mViewMatrix[12], mViewMatrix[13], mViewMatrix[14]);

   // set the new rotation for the camera
   mViewMatrix = rotation.ToMatrix();

   // add the eye location to the matrix
   mViewMatrix[12] = eye.X();
   mViewMatrix[13] = eye.Y();
   mViewMatrix[14] = eye.Z();

   // invert the camera matrix back
   mViewMatrix.MakeInverse();
}

} // namespace camera_policy
