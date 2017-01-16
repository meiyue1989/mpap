
#include "HBSplineBase.h"
#include "SolverPARDISO.h"
#include "SolverPardisoEigen.h"
#include "SolverMA41Eigen.h"
#include "SolverEigen.h"




int  HBSplineBase::findCellNumber(const myPoint& geom)
{
  int  ii, ind[3]={0,0,0}, nn[3]={0,0,0}, num;

  for(ii=0;ii<DIM;ii++)
  {
    ind[ii] = int( (geom[ii]-origin[ii])/GeomData.GetGridDX(ii) );

    if(ind[ii] == nelem[ii])
      ind[ii] -= 1;

    nn[ii] = nelem[ii] + 2*degree[ii];
  }

  //printf("%5d \t %5d \t %5d \n", degree[0], degree[1], degree[2]);
  //printf("%5d \t %5d \t %5d \n", degree[0], degree[1], degree[2]);

  num = nn[0]*nn[1]*(degree[2]+ind[2]) + nn[0]*(degree[1]+ind[1]) + (degree[0]+ind[0]);

  //printf("%5d \t %5d \t %5d \t %5d \n", ind[0], ind[1], ind[2], num);
  //printf("%5d \t %5d \t %5d \t %5d \n", nn[0], nn[1], nn[2], num);

  if(elems[num]->IsLeaf())
  {
     return num;
  }
  else
  {
     double  uu, vv, ww, *tmp1, *tmp2, *tmp3;

     node *nd, *nd1, *nd2, *nd3;

     nd = elems[num];

     if( DIM == 1)
     {
       uu = (geom[0]-origin[0])/gridLEN[0];

       while(!nd->IsLeaf())
       {
         nd1  = nd->GetChild(LEFT);
         tmp1 = nd1->GetKnots(0);

         if( uu < tmp1[1] ) // LEFT
           nd2 = nd->GetChild(LEFT);
         else  //RIGHT
           nd2 = nd->GetChild(RIGHT);

         nd = nd2;
       }
     }
     else if( DIM == 2)
     {
       uu = (geom[0]-origin[0])/gridLEN[0];
       vv = (geom[1]-origin[1])/gridLEN[1];

       while(!nd->IsLeaf())
       {
         nd1 = nd->GetChild(SW);

         tmp1 = nd1->GetKnots(0);
         tmp2 = nd1->GetKnots(1);

         if( uu < tmp1[1] ) // SW or NW
         {
           if( vv < tmp2[1] ) // SW
             nd2 = nd1;
           else // NW
             nd2 = nd->GetChild(NW);
         }
         else  //SE or NE
         {
           if( vv < tmp2[1] ) // SE
             nd2 = nd->GetChild(SE);
           else // NE
             nd2 = nd->GetChild(NE);
         }
         nd = nd2;
       }
     }
     else
     {
       uu = (geom[0]-origin[0])/gridLEN[0];
       vv = (geom[1]-origin[1])/gridLEN[1];
       ww = (geom[2]-origin[2])/gridLEN[2];

       while(!nd->IsLeaf())
       {
         nd1 = nd->GetChild(SW_BACK);

         tmp1 = nd1->GetKnots(0);
         tmp2 = nd1->GetKnots(1);
         tmp3 = nd1->GetKnots(2);

         if( uu < tmp1[1] ) // SW_FRONT or NW_FRONT or SW_BACK or NW_BACK
         {
           if( vv < tmp2[1] ) // SW_FRONT or SW_BACK
           {
             if( ww < tmp3[1] )
               nd2 = nd->GetChild(SW_BACK);
             else
               nd2 = nd->GetChild(SW_FRONT);
           }
           else // NW_FRONT or NW_BACK
           {
             if( ww < tmp3[1] )
               nd2 = nd->GetChild(NW_BACK);
             else
               nd2 = nd->GetChild(NW_FRONT);
           }
         }
         else  //SE_FRONT or NE_FRONT or SE_BACK or NE_BACK
         {
           if( vv < tmp2[1] ) // SE_FRONT or SE_BACK
           {
             if( ww < tmp3[1] )
               nd2 = nd->GetChild(SE_BACK);
             else
               nd2 = nd->GetChild(SE_FRONT);
           }
           else // NE_FRONT or NE_BACK
           {
             if( ww < tmp3[1] )
               nd2 = nd->GetChild(NE_BACK);
             else
               nd2 = nd->GetChild(NE_FRONT);
           }
         }
         nd = nd2;
       }
     }

     return  (nd->GetID());
  }
}



double  HBSplineBase::ComputeGeometry(const int dir, double param)
{
  return  (origin[dir] + gridLEN[dir]*param);
}


void  HBSplineBase::ComputeGeometry(const myPoint& param, myPoint& geom)
{
  for(int ii=0;ii<DIM;ii++)
    geom[ii] = origin[ii] + gridLEN[ii]*param[ii];

  return;
}



void  HBSplineBase::geometryToParametric(const myPoint& geom, myPoint& param)
{
  for(int ii=0;ii<DIM;ii++)
    param[ii] = (geom[ii]-origin[ii])/gridLEN[ii];

  return;
}




void HBSplineBase::setInitialConditions()
{
    double* tmp;

    solverEigen->zeroMtx();

    for(int ee=0;ee<elems.size();ee++)
    {
      if( !(elems[ee]->IsGhost()) &&  elems[ee]->IsLeaf() )
      {
        //cout << " elems[ee]->GetID() " <<  elems[ee]->GetID() << '\t' <<  elems[ee]->GetLevel() << endl;

        elems[ee]->resetMatrixAndVector();
        elems[ee]->setInitialProfile();
        //elems[ee]->AssembleMatrixAndVector(1, solver->mtx, &(rhsVec(0)));
      }
    }

    solverEigen->currentStatus = ASSEMBLY_OK;

    factoriseSolveAndUpdate();

    //rhsVec = rhsVec * (1.0/rhsVec.maxCoeff());
    //

    //
    int resln1[3]; resln1[0] = resln1[1] = 5;

    //postProcess2D(1, 1, 10, 1, 0.0, 1.0, resln1);
    //postProcess1D(1, 1, 10, 1, 0.0, 1.0, resln1);
    //

    solnInit = soln;
    SolnData.var1 = solnInit;

   return;
}







void HBSplineBase::computeConditionNumber()
{
  solverEigen->computeConditionNumber();

  return;
}





void HBSplineBase::setSolver(int slv, int *parm, bool cIO)
{
    slv_type = slv;

    //Eigen::initParallel();
    Eigen::setNbThreads(0);

    //cout << " Eigen::getNbThreads() = " << Eigen::nbThreads() << endl;

    if(solverEigen != NULL)
      delete solverEigen;
    solverEigen = NULL;

    if(solverPetsc != NULL)
      delete solverPetsc;
    solverPetsc = NULL;

    //cout << " SOLVER_TYPE = " << slv << endl;

    switch(slv)
    {
        case  1: // MA41 ..........................

            SOLVER_TYPE = SOLVER_TYPE_EIGEN;

            solverEigen = (SolverEigen*) new SolverMA41Eigen;

            //solverEigen->STABILISED = (FluidSolnData.FluidProps[6] == 1);
            solverEigen->STABILISED = true;

            //printInfo();

            prepareMatrixPattern();

            //cout << " matrix pattern prepared " << endl;

            if(solverEigen->initialise(0,0,totalDOF) != 0)
              return;

            //cout << " solver prepared " << endl;
            //solver->printInfo();

        break;

        case  4: // SolverEigen ..........................

            SOLVER_TYPE = SOLVER_TYPE_EIGEN;

            solverEigen = (SolverEigen*) new SolverEigen;

            solverEigen->STABILISED = true;

            solverEigen->setAlgorithmType(parm[0]);

            //printInfo();

            prepareMatrixPattern();

            if(solverEigen->initialise(0,0,totalDOF) != 0)
              return;

            //solver->SetSolverAndParameters();

            //solver->printInfo();

        break;

        case  5: // PARDISO(sym) with Eigen
        case  6: // PARDISO(unsym) with Eigen

            SOLVER_TYPE = SOLVER_TYPE_EIGEN;

            solverEigen = (SolverEigen*) new SolverPardisoEigen;

            if (parm != NULL) numProc = parm[0]; else numProc = 1;

            numProc = min(MAX_PROCESSORS,numProc);

            //cout << " numProc " <<  numProc << endl;

            solverEigen->STABILISED = true;

            //printInfo();
            //cout << " numProc " <<  numProc << endl;
            prepareMatrixPattern();

            if(slv == 5)
            {
              if(solverEigen->initialise(numProc, PARDISO_STRUCT_SYM, totalDOF) != 0)
                return;
            }
            if(slv == 6)
            {
              if(solverEigen->initialise(numProc, PARDISO_UNSYM, totalDOF) != 0)
                return;
            }

            //solver->printInfo();

        break;

        case  8: // SolverPetsc ..........................

            SOLVER_TYPE = SOLVER_TYPE_PETSC;

            solverPetsc = (SolverPetsc*) new SolverPetsc;

            prepareMatrixPattern();

            if(solverPetsc->initialise(0, 0, totalDOF) != 0)
              return;

            solverPetsc->SetSolverAndParameters();
            //cout << " kkkkkkkkkk " << endl;
            solverPetsc->printInfo();
            //cout << " aaaaaaaaa " << endl;

        break;

        default: // invalid slv ...................

            cout << " this solver has not been implemented yet!\n\n";

        break;
    }

    solverOK = true;

    //cout << " cIO " << cIO << endl;

    if(solverEigen != NULL)
      solverEigen->checkIO = cIO;

    if(solverPetsc != NULL)
      solverPetsc->checkIO = cIO;

    //if(tis > 0)
      //setInitialConditions();

    return;
}







