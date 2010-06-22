/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Copyright Projet JRL-Japan, 2007
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * File:      VectorUTheta.cpp
 * Project:   SOT
 * Author:    Nicolas Mansard
 *
 * Version control
 * ===============
 *
 *  $Id$
 *
 * Description
 * ============
 *
 *
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <sot-core/vector-utheta.h>
#include <sot-core/debug.h>
using namespace std;
using namespace sot;

static const double ANGLE_MINIMUM = 0.0001;
static const double SINC_MINIMUM = 1e-8;
static const double COSC_MINIMUM = 2.5e-4;

sotVectorRotation& VectorUTheta::
fromMatrix( const MatrixRotation& rot )
{
  sotDEBUGIN(15) ;
  
  const ml::Matrix& rotmat = rot;
  double sina = sqrt( (rotmat(1,0)-rotmat(0,1))*(rotmat(1,0)-rotmat(0,1))
		      + (rotmat(2,0)-rotmat(0,2))*(rotmat(2,0)-rotmat(0,2))
		      + (rotmat(2,1)-rotmat(1,2))*(rotmat(2,1)-rotmat(1,2)) ) / 2.;
  double cosa = (rotmat(0,0)+rotmat(1,1)+rotmat(2,2)-1.0)/2.0;
  double theta = atan2(sina,cosa);  /* theta in (0, PI) since s > 0 */

  if ( ( sina>ANGLE_MINIMUM )||( cosa>0.0 ) ) /* general case */
    {
      double sinc = ( fabs(sina)>SINC_MINIMUM ) ? (sina/theta) : 1.;
      
      vector(0) = (rotmat(2,1)-rotmat(1,2))/(2*sinc);
      vector(1) = (rotmat(0,2)-rotmat(2,0))/(2*sinc);
      vector(2) = (rotmat(1,0)-rotmat(0,1))/(2*sinc);
    }
  else /* theta near PI */
    {
      vector(0) = theta*(sqrt((rotmat(0,0)-cosa)/(1-cosa)));
      if( (rotmat(2,1)-rotmat(1,2))<0 ) { vector(0) = -vector(0); }

      vector(1) = theta*(sqrt((rotmat(1,1)-cosa)/(1-cosa)));
      if( (rotmat(0,2)-rotmat(2,0))<0 ) { vector(1) = -vector(1); }

      vector(2) = theta*(sqrt((rotmat(2,2)-cosa)/(1-cosa)));
      if( (rotmat(1,0)-rotmat(0,1))<0 ) { vector(2) = -vector(2); }
    }
  
  sotDEBUGOUT(15) ;
  return *this;
}


MatrixRotation& VectorUTheta::
toMatrix( MatrixRotation& rot ) const
{
  sotDEBUGIN(15) ;

  double theta = sqrt( vector(0)*vector(0)+vector(1)*vector(1)+vector(2)*vector(2) );
  double si = sin(theta);
  double co = cos(theta);
  double sinc = ( fabsf( theta )>SINC_MINIMUM ) ? (si/theta) : 1.; 
  double mcosc = ( fabsf( theta )>COSC_MINIMUM ) ? ( (1-co)/(theta*theta) ) : .5;

  ml::Matrix &rotmat = rot;

  rotmat(0,0) = co + mcosc*vector(0)*vector(0);
  rotmat(0,1) = -sinc*vector(2) + mcosc*vector(0)*vector(1);
  rotmat(0,2) = sinc*vector(1) + mcosc*vector(0)*vector(2);
  rotmat(1,0) = sinc*vector(2) + mcosc*vector(1)*vector(0);
  rotmat(1,1) = co + mcosc*vector(1)*vector(1);
  rotmat(1,2) = -sinc*vector(0) + mcosc*vector(1)*vector(2);
  rotmat(2,0) = -sinc*vector(1) + mcosc*vector(2)*vector(0);
  rotmat(2,1) = sinc*vector(0) + mcosc*vector(2)*vector(1);
  rotmat(2,2) = co + mcosc*vector(2)*vector(2);

  sotDEBUGOUT(15) ;
  return rot;
}
