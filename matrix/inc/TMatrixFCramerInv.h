// @(#)root/base:$Name:  $:$Id: TMatrixFCramerInv.h,v 1.28 2003/12/11 13:39:22 brun Exp $
// Authors: Fons Rademakers, Eddy Offermann  Jan 2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TMatrixFCramerInv
#define ROOT_TMatrixFCramerInv

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixFCramerInv                                                    //
//                                                                      //
// Encapsulate Cramer Inversion routines.                               //
//                                                                      //
// The 4x4, 5x5 and 6x6 are adapted from routines written by            //
// Mark Fischler and Steven Haywood as part of the CLHEP package        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

#ifndef ROOT_TMatrixF
#include "TMatrixF.h"
#endif

class TMatrixFCramerInv {

public:
  static Int_t Inv2x2(TMatrixF &m,Double_t *determ);
  static Int_t Inv3x3(TMatrixF &m,Double_t *determ);
  static Int_t Inv4x4(TMatrixF &m,Double_t *determ);
  static Int_t Inv5x5(TMatrixF &m,Double_t *determ);
  static Int_t Inv6x6(TMatrixF &m,Double_t *determ);

  ClassDef(TMatrixFCramerInv,0)  //Interface to math routines
};

#endif
