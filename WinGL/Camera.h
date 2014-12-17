#ifndef _CAMERA_H_
#define _CAMERA_H_

// local includes
#include "Matrix.h"

#ifdef near
   #pragma push_macro("near")
   #undef near
   #define restore_near_marco
#endif // near

#ifdef far
   #pragma push_macro("far")
   #undef far
   #define restore_far_marco
#endif // far

template < typename Policy >
class Camera
{
public:
   // public typedefs
   typedef typename Policy::type       type;
   typedef typename Policy::mat_type   mat_type;
   typedef typename Policy::vec_type   vec_type;

   // constructor / destructor
    Camera( );
    Camera( const Camera< Policy > & camera );
    Camera( const vec_type & eye, const vec_type & center );
   ~Camera( );

   // copy operator
   Camera & operator = ( const Camera & camera );

   // initializes the camera at a specific location
   void LookAt( const vec_type & eye, const vec_type & center );

   // moves the camera by a set amount along the camera axes
   void TranslateRight( const type amount );
   void TranslateForward( const type amount );
   void TranslateUp( const type amount );

   // moves the camera by a set amount along the camera axes defined by direction
   void Translate( const type amount, const vec_type & direction );

   // rotates the camera
   void SetYaw( const type yaw_deg );
   void SetPitch( const type pitch_deg );
   void SetRoll( const type roll_deg );

   void RotateYaw( const type amount_deg );
   void RotatePitch( const type amount_deg );
   void RotateRoll( const type amount_deg );

   // obtains the current euler angles
   type GetYaw( ) const;
   type GetPitch( ) const;
   type GetRoll( ) const;

   // obtains the current euler angles and the pointers can be null
   void GetYPR( type * const yaw_deg, type * const pitch_deg, type * const roll_deg ) const;
   vec_type GetYPR( ) const;

   // obtains the current eye position
   vec_type GetEyePosition( ) const;

   // returns the view matrix
   mat_type GetViewMatrix( ) const;

   // sets the projection matrix
   void SetPerspective( const type fov, const type aspect, const type near, const type far );

   // obtains properties of the projection matrix
   type GetProjectionFOV( ) const;
   type GetProjectionAspect( ) const;
   type GetProjectionNear( ) const;
   type GetProjectionFar( ) const;

   // returns the projection matrix
   const mat_type & GetProjectionMatrix( ) const;

private:
   // camera policy used to define the type of camera
   Policy      mPolicy;

   // defines attributes for the projection
   type        mProjFOV;
   type        mProjAspect;
   type        mProjNear;
   type        mProjFar;

   // defines the projection matrix associated to the camera
   mat_type    mProjection;

};

template < typename Policy >
Camera< Policy >::Camera( )
{
}

template < typename Policy >
Camera< Policy >::Camera( const Camera< Policy > & camera )
{
   // fill in
   WGL_ASSERT(false);
}

template < typename Policy >
Camera< Policy >::Camera( const vec_type & eye, const vec_type & center )
{
   LookAt(eye, center);
}

template < typename Policy >
Camera< Policy >::~Camera( )
{
}

template < typename Policy >
Camera< Policy > & Camera< Policy >::operator = ( const Camera< Policy > & camera )
{
   // fill in
   WGL_ASSERT(false);
}

template < typename Policy >
void Camera< Policy >::LookAt( const vec_type & eye, const vec_type & center )
{
   mPolicy.LookAt(eye, center);
}

template < typename Policy >
void Camera< Policy >::TranslateRight( const type amount )
{
   mPolicy.TranslateRight(amount);
}

template < typename Policy >
void Camera< Policy >::TranslateForward( const type amount )
{
   mPolicy.TranslateForward(amount);
}

template < typename Policy >
void Camera< Policy >::TranslateUp( const type amount )
{
   mPolicy.TranslateUp(amount);
}

template < typename Policy >
void Camera< Policy >::Translate( const type amount, const vec_type & direction )
{
   mPolicy.Translate(amount, direction);
}

template < typename Policy >
void Camera< Policy >::SetYaw( const type yaw_deg )
{
   mPolicy.SetYaw(yaw_deg);
}

template < typename Policy >
void Camera< Policy >::SetPitch( const type pitch_deg )
{
   mPolicy.SetPitch(pitch_deg);
}

template < typename Policy >
void Camera< Policy >::SetRoll( const type roll_deg )
{
   mPolicy.SetRoll(roll_deg);
}

template < typename Policy >
void Camera< Policy >::RotateYaw( const type amount_deg )
{
   mPolicy.RotateYaw(amount_deg);
}

template < typename Policy >
void Camera< Policy >::RotatePitch( const type amount_deg )
{
   mPolicy.RotatePitch(amount_deg);
}

template < typename Policy >
void Camera< Policy >::RotateRoll( const type amount_deg )
{
   mPolicy.RotateRoll(amount_deg);
}

template < typename Policy >
typename Camera< Policy >::type Camera< Policy >::GetYaw( ) const
{
   return mPolicy.GetYaw();
}

template < typename Policy >
typename Camera< Policy >::type Camera< Policy >::GetPitch( ) const
{
   return mPolicy.GetPitch();
}

template < typename Policy >
typename Camera< Policy >::type Camera< Policy >::GetRoll( ) const
{
   return mPolicy.GetRoll();
}

template < typename Policy >
void Camera< Policy >::GetYPR( type * const yaw_deg, type * const pitch_deg, type * const roll_deg ) const
{
   mPolicy.GetYPR(yaw_deg, pitch_deg, roll_deg);
}

template < typename Policy >
typename Camera< Policy >::vec_type Camera< Policy >::GetYPR( ) const
{
   vec_type ypr;

   GetYPR(&ypr[0], &ypr[1], &ypr[2]);

   return ypr;
}

template < typename Policy >
typename Camera< Policy >::vec_type Camera< Policy >::GetEyePosition( ) const
{
   return mPolicy.GetEyePosition();
}

template < typename Policy >
typename Camera< Policy >::mat_type Camera< Policy >::GetViewMatrix( ) const
{
   return mPolicy.GetViewMatrix();
}

template < typename Policy >
typename void Camera< Policy >::SetPerspective( const type fov, const type aspect, const type near, const type far )
{
   mProjection.MakePerspective(fov, aspect, near, far);
}

template < typename Policy >
typename const Camera< Policy >::mat_type & Camera< Policy >::GetProjectionMatrix( ) const
{
   return mProjection;
}

#ifdef restore_near_marco
   #pragma pop_macro("near")
#endif // restore_near_marco

#ifdef restore_far_marco
   #pragma pop_macro("far")
#endif // restore_far_marco

#endif // _CAMERA_H_
