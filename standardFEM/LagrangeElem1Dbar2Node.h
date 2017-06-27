
#ifndef incl_LagrangeElem1Dbar2Node_h
#define incl_LagrangeElem1Dbar2Node_h


#include "LagrangeElement.h"


class  LagrangeElem1Dbar2Node : public LagrangeElement
{
  public:

    LagrangeElem1Dbar2Node();

    virtual ~LagrangeElem1Dbar2Node();

    virtual int getElmTypeNameNum()
    {  return 0;     }

    virtual void prepareElemData();

    void prepareElemData2();

    virtual int  calcStiffnessAndResidual(MatrixXd& Klocal, VectorXd& Flocal);

    virtual int calcInternalForces();

    virtual int calcLoadVector();


    virtual int calcOutput(double u1, double v1);

    virtual void discreteContourplot(int, int, int, int, double, double);

    virtual void projectToKnots(bool, int, int, int);

    virtual void projectStrain(int, int, double*);

    virtual void projectStress(int, double*);

    virtual void projectIntVar(int, double*);

    virtual void toPostprocess(int, int, int,  SparseMatrixXd&, VectorXd&);

};

#endif

