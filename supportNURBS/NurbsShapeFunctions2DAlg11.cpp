#include <iostream>
#include <math.h>
#include "Debug.h"
#include "NurbsShapeFunctions.h"

using namespace std;


void NurbsShapeFunctions2DAlg11(NurbsSURFACE *surf1, int ni, int nj, double u, double v, double* N, double* dN_dx, double* dN_dy, double& J)
{
  short p = surf1->p, q = surf1->q, ii, jj, loc_num = 0;

  int ind1 = ni+p, ind2 = ind1+1, ind3 = nj+q, ind4 = ind3+1;

  double du_dtildeu, dv_dtildev;

  // Gradient of mapping from parent element to parameter space
  du_dtildeu = (surf1->U[ind2]-surf1->U[ind1])/2.0;
  dv_dtildev = (surf1->V[ind4]-surf1->V[ind3])/2.0;

  int ROWS = 2;

  double** ders1 = new double*[ROWS];
  double** ders2 = new double*[ROWS];

  for(ii=0;ii<ROWS;ii++)
  {
     ders1[ii] = new double[p+1];
     ders2[ii] = new double[q+1];
  }

  DersBasisFuns(&(surf1->U[0]), surf1->U.n, p, u, 1, ders1 );
  DersBasisFuns(&(surf1->V[0]), surf1->V.n, q, v, 1, ders2 );

  vector<double>  dR_du1(surf1->nlbf), dR_du2(surf1->nlbf);
  // Calculate Bivariate B-Spline basis functions and their derivatives w.r.t parametetric coordinates

  loc_num = 0;

  for(jj=0; jj<=q; jj++)
  {
     for(ii=0; ii<=p; ii++)
     {
        N[loc_num] = ders1[0][ii] * ders2[0][jj];
        dR_du1[loc_num] = ders1[1][ii] * ders2[0][jj];
        dR_du2[loc_num] = ders1[0][ii] * ders2[1][jj];
        loc_num++;
     }
  }


  double dx_du[2][2];
  dx_du[0][0] = dx_du[1][0] = dx_du[0][1] = dx_du[1][1] = 0.0;


  // Gradient of mapping from parameter space to physical space
  loc_num = 0;
  EPOINT *EP;

  int temp;
  for(jj=0; jj<=q; jj++)
  {
    temp = nj+jj;
    for(ii=0; ii<=p; ii++)
    {
      EP = &(surf1->PP[ni+ii][temp] );

      dx_du[0][0] +=  (EP->x * dR_du1[loc_num]) ;
      dx_du[1][0] +=  (EP->x * dR_du2[loc_num]) ;
      dx_du[0][1] +=  (EP->y * dR_du1[loc_num]) ;
      dx_du[1][1] +=  (EP->y * dR_du2[loc_num]) ;

      loc_num++;
    }
  }

  double du_dx[2][2];

  // Compute inverse of gradient(dx_du)
  double det;
  det = dx_du[0][0]*dx_du[1][1] - dx_du[0][1]*dx_du[1][0];

  du_dx[0][0] = dx_du[1][1]/det;
  du_dx[0][1] = -dx_du[0][1]/det;
  du_dx[1][0] = -dx_du[1][0]/det;
  du_dx[1][1] = dx_du[0][0]/det;


  // Compute derivatives of basis functions w.r.t physical coordinates
  for(loc_num = 0;loc_num<(surf1->nlbf); loc_num++)
  {
    dN_dx[loc_num] = dR_du1[loc_num] * du_dx[0][0] + dR_du2[loc_num] * du_dx[0][1];
    dN_dy[loc_num] = dR_du1[loc_num] * du_dx[1][0] + dR_du2[loc_num] * du_dx[1][1];
  }


  // Determinant of the Jacobian matrix

  J = det * du_dtildeu * dv_dtildev ;

  EP = NULL;

  for(ii=0;ii<ROWS;ii++)
  {
    delete [] ders1[ii];
    delete [] ders2[ii];
  }
  delete [] ders1;
  delete [] ders2;

  return;
}
