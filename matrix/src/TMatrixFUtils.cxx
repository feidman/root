// @(#)root/matrix:$Name:  $:$Id: TMatrixFUtils.cxx,v 1.1 2004/01/25 20:33:32 brun Exp $
// Authors: Fons Rademakers, Eddy Offermann  Nov 2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Matrix utility classes.                                              //
//                                                                      //
// This file defines utility classes for the Linear Algebra Package.    //
// The following classes are defined here:                              //
//                                                                      //
// Different matrix views without copying data elements :               //
//   TMatrixFRow_const    TMatrixFRow                                   //
//   TMatrixFColumn_const TMatrixFColumn                                //
//   TMatrixFDiag_const   TMatrixFDiag                                  //
//   TMatrixFFlat_const   TMatrixFFlat                                  //
//                                                                      //
//   TElementActionF                                                    //
//   TElementPosActionF                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TMatrixFBase.h"

ClassImp(TMatrixFRow)
ClassImp(TMatrixFRow_const)
ClassImp(TMatrixFColumn)
ClassImp(TMatrixFColumn_const)
ClassImp(TMatrixFDiag)
ClassImp(TMatrixFDiag_const)
ClassImp(TMatrixFFlat)
ClassImp(TMatrixFFlat_const)

//______________________________________________________________________________
TMatrixFRow_const::TMatrixFRow_const(const TMatrixFBase &matrix,Int_t row)
{
  Assert(matrix.IsValid());
  fRowInd = row-matrix.GetRowLwb();
  if (fRowInd >= matrix.GetNrows() || fRowInd < 0) {
    Error("TMatrixFRow_const(const TMatrixFBase &,Int_t)","row index out of bounds");
    return;
  }

  fMatrix = &matrix;
  fPtr = matrix.GetMatrixArray()+fRowInd*matrix.GetNcols();
  fInc = 1;
}

//______________________________________________________________________________
void TMatrixFRow_const::Streamer(TBuffer &R__b)
{
  // Stream an object of class TMatrixFRow_const.

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
    TMatrixFRow_const::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
    fPtr = fMatrix->GetMatrixArray()+fRowInd*fMatrix->GetNcols();
  } else {
    TMatrixFRow_const::Class()->WriteBuffer(R__b,this);
  }
}

//______________________________________________________________________________
TMatrixFRow::TMatrixFRow(TMatrixFBase &matrix,Int_t row)
            :TMatrixFRow_const(matrix,row)
{
}

//______________________________________________________________________________
TMatrixFRow::TMatrixFRow(const TMatrixFRow &mr) : TMatrixFRow_const(mr)
{
  *this = mr;
}

//______________________________________________________________________________
void TMatrixFRow::operator=(Float_t val)
{
  // Assign val to every element of the matrix row.

  Float_t *rp = const_cast<Float_t *>(fPtr);
  for ( ; rp < fPtr+fMatrix->GetNcols(); rp += fInc)
    *rp = val;
}

//______________________________________________________________________________
void TMatrixFRow::operator+=(Float_t val)
{
  // Add val to every element of the matrix row. 

  Float_t *rp = const_cast<Float_t *>(fPtr);
  for ( ; rp < fPtr+fMatrix->GetNcols(); rp += fInc)
    *rp += val;
}

//______________________________________________________________________________
void TMatrixFRow::operator*=(Float_t val)
{
   // Multiply every element of the matrix row with val.

  Float_t *rp = const_cast<Float_t *>(fPtr);
  for ( ; rp < fPtr + fMatrix->GetNcols(); rp += fInc)
    *rp *= val;
}

//______________________________________________________________________________
void TMatrixFRow::operator=(const TMatrixFRow_const &mr)
{
  const TMatrixFBase *mt = mr.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFRow_const &)","matrices not compatible");
    return;
  }

  Float_t *rp1 = const_cast<Float_t *>(fPtr);
  const Float_t *rp2 = mr.GetPtr();
  for ( ; rp1 < fPtr+fMatrix->GetNcols(); rp1 += fInc,rp2 += fInc)
    *rp1 = *rp2;
}

//______________________________________________________________________________
void TMatrixFRow::operator=(const TMatrixFRow &mr)
{
  const TMatrixFBase *mt = mr.GetMatrix();
  if (fMatrix == mt) return;
 
  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFRow &)","matrices not compatible");
    return;
  }

  Float_t *rp1 = const_cast<Float_t *>(fPtr);
  const Float_t *rp2 = mr.GetPtr();
  for ( ; rp1 < fPtr+fMatrix->GetNcols(); rp1 += fInc,rp2 += fInc)
    *rp1 = *rp2;
}

//______________________________________________________________________________
void TMatrixFRow::operator=(const TVectorF &vec)
{
   // Assign a vector to a matrix row. The vector is considered row-vector
   // to allow the assignment in the strict sense.

  Assert(vec.IsValid());

  if (fMatrix->GetColLwb() != vec.GetLwb() || fMatrix->GetNcols() != vec.GetNrows()) {
    Error("operator=(const TVectorF &)","vector length != matrix-row length");
    return;
  }

  Float_t *rp = const_cast<Float_t *>(fPtr);
  const Float_t *vp = vec.GetMatrixArray();
  for ( ; rp < fPtr+fMatrix->GetNcols(); rp += fInc)
    *rp = *vp++;
}

//______________________________________________________________________________
void TMatrixFRow::operator+=(const TMatrixFRow_const &r)
{
  // Add to every element of the matrix row the corresponding element of row r.

  const TMatrixFBase *mt = r.GetMatrix();

  if (fMatrix->GetColLwb() != mt->GetColLwb() || fMatrix->GetNcols() != mt->GetNcols()) {
    Error("operator+=(const TMatrixFRow_const &)","different row lengths");
    return;
  }

  Float_t *rp1 = const_cast<Float_t *>(fPtr);
  const Float_t *rp2 = r.GetPtr();
  for ( ; rp1 < fPtr+fMatrix->GetNcols(); rp1 += fInc,rp2 += r.GetInc())
   *rp1 += *rp2;
}

//______________________________________________________________________________
void TMatrixFRow::operator*=(const TMatrixFRow_const &r)
{
  // Multiply every element of the matrix row with the
  // corresponding element of row r.

  const TMatrixFBase *mt = r.GetMatrix();

  if (fMatrix->GetColLwb() != mt->GetColLwb() || fMatrix->GetNcols() != mt->GetNcols()) {
    Error("operator*=(const TMatrixFRow_const &)","different row lengths");
    return;
  }

  Float_t *rp1 = const_cast<Float_t *>(fPtr);
  const Float_t *rp2 = r.GetPtr();
  for ( ; rp1 < fPtr+fMatrix->GetNcols(); rp1 += fInc,rp2 += r.GetInc())
    *rp1 *= *rp2;
}

//______________________________________________________________________________
TMatrixFColumn_const::TMatrixFColumn_const(const TMatrixFBase &matrix,Int_t col)
{
  Assert(matrix.IsValid());
  fColInd = col-matrix.GetColLwb();
  if (fColInd >= matrix.GetNcols() || fColInd < 0) {
    Error("TMatrixFColumn_const(const TMatrixFBase &,Int_t)","column index out of bounds");
    return;
  }

  fMatrix = &matrix;
  fPtr = matrix.GetMatrixArray()+fColInd;
  fInc = matrix.GetNcols();
}

//______________________________________________________________________________
void TMatrixFColumn_const::Streamer(TBuffer &R__b)
{
  // Stream an object of class TMatrixFColumn.
   
  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
    TMatrixFColumn_const::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
    fPtr = fMatrix->GetMatrixArray()+fColInd;
  } else {
    TMatrixFColumn_const::Class()->WriteBuffer(R__b,this);
  }
}

//______________________________________________________________________________
TMatrixFColumn::TMatrixFColumn(TMatrixFBase &matrix,Int_t col)
               :TMatrixFColumn_const(matrix,col)
{
}

//______________________________________________________________________________
TMatrixFColumn::TMatrixFColumn(const TMatrixFColumn &mc) : TMatrixFColumn_const(mc)
{
  *this = mc;
}

//______________________________________________________________________________
void TMatrixFColumn::operator=(Float_t val)
{
  // Assign val to every element of the matrix column.

  Float_t *cp = const_cast<Float_t *>(fPtr);
  for ( ; cp < fPtr+fMatrix->GetNoElements(); cp += fInc)
    *cp = val;
}

//______________________________________________________________________________
void TMatrixFColumn::operator+=(Float_t val)
{
  // Add val to every element of the matrix column.

  Float_t *cp = const_cast<Float_t *>(fPtr);
  for ( ; cp < fPtr+fMatrix->GetNoElements(); cp += fInc)
    *cp += val;
}

//______________________________________________________________________________
void TMatrixFColumn::operator*=(Float_t val)
{
   // Multiply every element of the matrix column with val.

  Float_t *cp = const_cast<Float_t *>(fPtr);
  for ( ; cp < fPtr+fMatrix->GetNoElements(); cp += fInc)
    *cp *= val;
}

//______________________________________________________________________________
void TMatrixFColumn::operator=(const TMatrixFColumn_const &mc) 
{   
  const TMatrixFBase *mt = mc.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFColumn_const &)","matrices not compatible");
    return;
  }

  Float_t *cp1 = const_cast<Float_t *>(fPtr);
  const Float_t *cp2 = mc.GetPtr();
  for ( ; cp1 < fPtr+fMatrix->GetNoElements(); cp1 += fInc,cp2 += fInc)
    *cp1 = *cp2;
}

//______________________________________________________________________________
void TMatrixFColumn::operator=(const TMatrixFColumn &mc)
{  
  const TMatrixFBase *mt = mc.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFColumn &)","matrices not compatible");
    return;
  }

  Float_t *cp1 = const_cast<Float_t *>(fPtr);
  const Float_t *cp2 = mc.GetPtr();
  for ( ; cp1 < fPtr+fMatrix->GetNoElements(); cp1 += fInc,cp2 += fInc)
    *cp1 = *cp2;
}

//______________________________________________________________________________
void TMatrixFColumn::operator=(const TVectorF &vec)
{
  // Assign a vector to a matrix column.

  Assert(vec.IsValid());

  if (fMatrix->GetRowLwb() != vec.GetLwb() || fMatrix->GetNrows() != vec.GetNrows()) {
    Error("operator=(const TVectorF &)","vector length != matrix-column length");
    Assert(0);
    return;
  }

  Float_t *cp = const_cast<Float_t *>(fPtr);
  const Float_t *vp = vec.GetMatrixArray();
  for ( ; cp < fPtr+fMatrix->GetNoElements(); cp += fInc)
    *cp = *vp++;

  Assert(vp == vec.GetMatrixArray()+vec.GetNrows());
}

//______________________________________________________________________________
void TMatrixFColumn::operator+=(const TMatrixFColumn_const &mc)
{
  const TMatrixFBase *mt = mc.GetMatrix();

  if (fMatrix->GetRowLwb() != mt->GetRowLwb() || fMatrix->GetNrows() != mt->GetNrows()) {
    Error("operator+=(const TMatrixFColumn_const &)","different row lengths");
    return;
  }

  Float_t *cp1 = const_cast<Float_t *>(fPtr);
  const Float_t *cp2 = mc.GetPtr();
  for ( ; cp1 < fPtr+fMatrix->GetNoElements(); cp1 += fInc,cp2 += fInc)
    *cp1 += *cp2;
}

//______________________________________________________________________________
void TMatrixFColumn::operator*=(const TMatrixFColumn_const &mc)
{
  // Multiply every element of the matrix column with the
  // corresponding element of column mc.

  const TMatrixFBase *mt = mc.GetMatrix();

  if (fMatrix->GetRowLwb() != mt->GetRowLwb() || fMatrix->GetNrows() != mt->GetNrows()) {
    Error("operator*=(const TMatrixFColumn_const &)","different row lengths");
    return;
  }

  Float_t *cp1 = const_cast<Float_t *>(fPtr);
  const Float_t *cp2 = mc.GetPtr();
  for ( ; cp1 < fPtr+fMatrix->GetNoElements(); cp1 += fInc,cp2 += fInc)
    *cp1 *= *cp2;
}     

//______________________________________________________________________________
TMatrixFDiag_const::TMatrixFDiag_const(const TMatrixFBase &matrix,Int_t /*dummy*/)
{
  Assert(matrix.IsValid());
  fMatrix = &matrix;
  fNdiag  = TMath::Min(matrix.GetNrows(),matrix.GetNcols());
  fPtr    = matrix.GetMatrixArray();
  fInc    = matrix.GetNcols()+1;
}

//______________________________________________________________________________
void TMatrixFDiag_const::Streamer(TBuffer &R__b)
{
  // Stream an object of class TMatrixFDiag.

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
    TMatrixFDiag_const::Class()->ReadBuffer(R__b, this,R__v,R__s,R__c);
    fPtr = fMatrix->GetMatrixArray();
  } else {
    TMatrixFDiag_const::Class()->WriteBuffer(R__b,this);
  }
}

//______________________________________________________________________________
TMatrixFDiag::TMatrixFDiag(TMatrixFBase &matrix,Int_t dummy)
             :TMatrixFDiag_const(matrix,dummy)
{
}

//______________________________________________________________________________
TMatrixFDiag::TMatrixFDiag(const TMatrixFDiag &md) : TMatrixFDiag_const(md)
{
  *this = md;
}

//______________________________________________________________________________
void TMatrixFDiag::operator=(Float_t val)
{
  // Assign val to every element of the matrix diagonal.

  Float_t *dp = const_cast<Float_t *>(fPtr);
  for (Int_t i = 0; i < fNdiag; i++, dp += fInc)
    *dp = val;
}

//______________________________________________________________________________
void TMatrixFDiag::operator+=(Float_t val)
{
  // Assign val to every element of the matrix diagonal.

  Float_t *dp = const_cast<Float_t *>(fPtr);
  for (Int_t i = 0; i < fNdiag; i++, dp += fInc)
    *dp += val;
}

//______________________________________________________________________________
void TMatrixFDiag::operator*=(Float_t val)
{
  // Assign val to every element of the matrix diagonal.

  Float_t *dp = const_cast<Float_t *>(fPtr);
  for (Int_t i = 0; i < fNdiag; i++, dp += fInc)
    *dp *= val;
}

//______________________________________________________________________________
void TMatrixFDiag::operator=(const TMatrixFDiag_const &md)
{
  const TMatrixFBase *mt = md.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFDiag_const &)","matrices not compatible");
    return;
  }

  Float_t *dp1 = const_cast<Float_t *>(fPtr);
  const Float_t *dp2 = md.GetPtr();
  for (Int_t i = 0; i < fNdiag; i++, dp1 += fInc, dp2 += fInc)
    *dp1 = *dp2;
}

//______________________________________________________________________________
void TMatrixFDiag::operator=(const TMatrixFDiag &md)
{
  const TMatrixFBase *mt = md.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFDiag &)","matrices not compatible");
    return;
  }

  Float_t *dp1 = const_cast<Float_t *>(fPtr);
  const Float_t *dp2 = md.GetPtr();
  for (Int_t i = 0; i < fNdiag; i++, dp1 += fInc, dp2 += fInc)
    *dp1 = *dp2;
}

//______________________________________________________________________________
void TMatrixFDiag::operator=(const TVectorF &vec)
{
  // Assign a vector to the matrix diagonal.

  Assert(vec.IsValid());

  if (fNdiag != vec.GetNrows()) {
    Error("operator=(const TVectorF &)","vector length != matrix-diagonal length");
    return;
  }

  Float_t *dp = const_cast<Float_t *>(fPtr);
  const Float_t *vp = vec.GetMatrixArray();
  for ( ; vp < vec.GetMatrixArray()+vec.GetNrows(); dp += fInc)
    *dp = *vp++;
}

//______________________________________________________________________________
void TMatrixFDiag::operator+=(const TMatrixFDiag_const &d)
{
  // Add to every element of the matrix diagonal the
  // corresponding element of diagonal d.

  if (fNdiag != d.GetNdiags()) {
    Error("operator=(const TMatrixFDiag_const &)","matrix-diagonal's different length");
    return;
  }

  Float_t *dp1 = const_cast<Float_t *>(fPtr);
  const Float_t *dp2 = d.GetPtr();
  for (Int_t i = 0; i < fNdiag; i++, dp1 += fInc, dp2 += d.GetInc())
    *dp1 += *dp2;
}

//______________________________________________________________________________
void TMatrixFDiag::operator*=(const TMatrixFDiag_const &d)
{
  // Add to every element of the matrix diagonal the
  // corresponding element of diagonal d.

  if (fNdiag != d.GetNdiags()) {
    Error("operator*=(const TMatrixFDiag_const &)","matrix-diagonal's different length");
    return;
  }

  Float_t *dp1 = const_cast<Float_t *>(fPtr);
  const Float_t *dp2 = d.GetPtr();
  for (Int_t i = 0; i < fNdiag; i++, dp1 += fInc, dp2 += d.GetInc())
    *dp1 *= *dp2;
}

//______________________________________________________________________________
TMatrixFFlat_const::TMatrixFFlat_const(const TMatrixFBase &matrix,Int_t /*dummy*/)
{
  Assert(matrix.IsValid());
  fMatrix = &matrix;
  fPtr    = matrix.GetMatrixArray();
  fNelems = matrix.GetNoElements();
}

//______________________________________________________________________________
void TMatrixFFlat_const::Streamer(TBuffer &R__b)
{
  // Stream an object of class TMatrixFFlat.

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s,&R__c);
    TMatrixFFlat_const::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
    fPtr = fMatrix->GetMatrixArray();
  } else {
    TMatrixFFlat_const::Class()->WriteBuffer(R__b,this);
  }
}

//______________________________________________________________________________
TMatrixFFlat::TMatrixFFlat(TMatrixFBase &matrix,Int_t dummy)
             :TMatrixFFlat_const(matrix,dummy)
{
}

//______________________________________________________________________________
TMatrixFFlat::TMatrixFFlat(const TMatrixFFlat &mf) : TMatrixFFlat_const(mf)
{
  *this = mf;
}

//______________________________________________________________________________
void TMatrixFFlat::operator=(Float_t val)
{
  // Assign val to every element of the matrix.

  Float_t *fp = const_cast<Float_t *>(fPtr);
  while (fp < fPtr+fMatrix->GetNoElements())
    *fp++ = val;
}

//______________________________________________________________________________
void TMatrixFFlat::operator+=(Float_t val)
{
  // Add val to every element of the matrix.

  Float_t *fp = const_cast<Float_t *>(fPtr);
  while (fp < fPtr+fMatrix->GetNoElements())
    *fp++ += val;
}

//______________________________________________________________________________
void TMatrixFFlat::operator*=(Float_t val)
{
  // Multiply every element of the matrix with val.

  Float_t *fp = const_cast<Float_t *>(fPtr);
  while (fp < fPtr+fMatrix->GetNoElements())
    *fp++ *= val;
}

//______________________________________________________________________________
void TMatrixFFlat::operator=(const TMatrixFFlat_const &mf)
{
  const TMatrixFBase *mt = mf.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFFlat_const &)","matrices not compatible");
    return;
  }

  Float_t *fp1 = const_cast<Float_t *>(fPtr);
  const Float_t *fp2 = mf.GetPtr();
  while (fp1 < fPtr+fMatrix->GetNoElements())
    *fp1++ = *fp2++;
}

//______________________________________________________________________________
void TMatrixFFlat::operator=(const TMatrixFFlat &mf)
{
  const TMatrixFBase *mt = mf.GetMatrix();
  if (fMatrix == mt) return;

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator=(const TMatrixFFlat &)","matrices not compatible");
    return;
  }

  Float_t *fp1 = const_cast<Float_t *>(fPtr);
  const Float_t *fp2 = mf.GetPtr();
  while (fp1 < fPtr+fMatrix->GetNoElements())
    *fp1++ = *fp2++;
}

//______________________________________________________________________________
void TMatrixFFlat::operator=(const TVectorF &vec)
{
  // Assign a vector to the matrix. The matrix is traversed row-wise

  Assert(vec.IsValid());
  Assert(fMatrix->GetNoElements() == vec.GetNrows());

  if (fMatrix->GetNoElements() != vec.GetNrows()) {
    Error("operator*=(const TVectorF &)","vector length != # matrix-elements");
    return;
  }

  Float_t *fp = const_cast<Float_t *>(fPtr);
  const Float_t *vp = vec.GetMatrixArray();
  while (fp < fPtr+fMatrix->GetNoElements())
     *fp++ = *vp++;
}

//______________________________________________________________________________
void TMatrixFFlat::operator+=(const TMatrixFFlat_const &mf)
{
  // Add to every element of the matrix the corresponding element of matrix mf.

  const TMatrixFBase *mt = mf.GetMatrix();

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator+=(const TMatrixFFlat &)","matrices not compatible");
    return;
  }

  Float_t *fp1 = const_cast<Float_t *>(fPtr);
  const Float_t *fp2 = mf.GetPtr();
  while (fp1 < fPtr + fMatrix->GetNoElements())
    *fp1++ += *fp2++;
}

//______________________________________________________________________________
void TMatrixFFlat::operator*=(const TMatrixFFlat_const &mf)
{
  // Multiply every element of the matrix with the corresponding element of diagonal mf.

  const TMatrixFBase *mt = mf.GetMatrix();

  if (!AreCompatible(*fMatrix,*mt)) {
    Error("operator*=(const TMatrixFFlat_const &)","matrices not compatible");
    return;
  }

  Float_t *fp1 = const_cast<Float_t *>(fPtr);
  const Float_t *fp2 = mf.GetPtr();
  while (fp1 < fPtr + fMatrix->GetNoElements())
    *fp1++ *= *fp2++;
}
