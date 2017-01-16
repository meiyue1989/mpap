CDOC BEGIN_SUBROUTINE SWPDSC
CDOC Initialise/switch state variables for the planar single crystal
CDOC
CDOC This initialises and switches state variables (between current and
CDOC previous values) for the planar double-slip single crystal
CDOC elasto-plastic model.
CDOC
CDOC BEGIN_PARAMETERS
CDOC INTEGER          MODE   >  Initialisation/Switching mode.
CDOC DOUBLE_PRECISION LALGVC <> Array of logical algorithmic variables
CDOC C                          at Gauss point. Current values.
CDOC DOUBLE_PRECISION LALGVL <> Array of logical algorithmic variables
CDOC C                          at Gauss point. Last converged
CDOC C                          (equilibrium) values.
CDOC DOUBLE_PRECISION RALGVC <> Array of real algorithmic variables at
CDOC C                          Gauss point. Current values.
CDOC DOUBLE_PRECISION RALGVL <> Array of real algorithmic variables at
CDOC C                          Gauss point. Last converged
CDOC C                          (equilibrium) values.
CDOC DOUBLE_PRECISION RSTAVC <> Array of real state variables at Gauss
CDOC C                          point. Current values.
CDOC DOUBLE_PRECISION RSTAVL <> Array of real state variables at Gauss
CDOC C                          point. Last converged (equilibrium)
CDOC C                          values.
CDOC DOUBLE_PRECISION STRESC <> Array of stress (Cauchy in large strain)
CDOC C                          components. Current values.
CDOC DOUBLE_PRECISION STRESL <> Array of stress (Cauchy in large strain)
CDOC C                          components. Last converged (equilibrium)
CDOC C                          values.
CDOC END_PARAMETERS
CDOC
CDOC E.de Souza Neto, July      1999: Initial coding
CDOC
      SUBROUTINE SWPDSC
     1(   MODE       ,
     2    LALGVC     ,LALGVL     ,RALGVC     ,RALGVL     ,RSTAVC     ,
     3    RSTAVL     ,STRESC     ,STRESL     )
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C Arguments
      LOGICAL
     1    LALGVC             ,LALGVL
      DIMENSION
     1    LALGVC(*)          ,LALGVL(*)          ,RALGVC(*)          ,
     2    RALGVL(*)          ,RSTAVC(*)          ,RSTAVL(*)          ,
     3    STRESC(*)          ,STRESL(*)
      DATA R0   ,R1   /
     1     0.0D0,1.0D0/
C***********************************************************************
C INITIALISE/SWITCH DATA FOR THE PLANAR DOUBLE-SLIP SINGLE CRYSTAL
C ELASTO-PLASTIC MATERIAL MODEL
C
C    MODE=0 -> Initialises the relevant data.
C
C    MODE=1 -> Assigns current values of the state variables to
C              converged solution (when the current iteration
C              satisfies the convergence criterion).
C
C    MODE=2 -> Assigns the last converged solution to current state
C              variables values (when a new iteration is required by
C              the iterative process).
C
C    MODE=3 -> Assigns the last converged solution to current state
C              variables values (when increment cutting is required).
C***********************************************************************
C
      NSTRE=4
      NRSTAV=5
      NRALGV=4
      NLALGV=6
C
      IF(MODE.EQ.0)THEN
C Initialisation mode
C ===================
        CALL RVZERO(STRESC,NSTRE)
        CALL RVZERO(RALGVC,NRALGV)
        DO 10 I=1,NLALGV
          LALGVC(I)=.FALSE.
   10   CONTINUE
C RSTAVA stores the elastic deformation gradient tensor (in-plane
C component only)
        RSTAVC(1)=R1
        RSTAVC(2)=R0
        RSTAVC(3)=R0
        RSTAVC(4)=R1
C and the accumulated plastic slip
        RSTAVC(5)=R0
      ELSE
C Switching modes
C ===============
        IF(MODE.EQ.1)THEN
          DO 20 I=1,NSTRE
            STRESL(I)=STRESC(I)
   20     CONTINUE
          DO 30 I=1,NRSTAV
            RSTAVL(I)=RSTAVC(I)
   30     CONTINUE
          DO 40 I=1,NLALGV
            LALGVL(I)=LALGVC(I)
   40     CONTINUE
C Zero plastic multipliers before starting a new increment
          CALL RVZERO(RALGVC,NRALGV)
        ELSEIF(MODE.EQ.2.OR.MODE.EQ.3)THEN
          DO 50 I=1,NSTRE
            STRESC(I)=STRESL(I)
   50     CONTINUE
          DO 60 I=1,NRSTAV
            RSTAVC(I)=RSTAVL(I)
   60     CONTINUE
          DO 70 I=1,NLALGV
            LALGVC(I)=LALGVL(I)
   70     CONTINUE
          IF(MODE.EQ.3)THEN
C Zero plastic multipliers before starting a new increment
            CALL RVZERO(RALGVC,NRALGV)
          ENDIF
        ENDIF
      ENDIF
      RETURN
      END
CDOC END_SUBROUTINE SWPDSC