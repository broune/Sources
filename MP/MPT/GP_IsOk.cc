/******************************************************************
 *
 * File:    GP_IsOK.h
 * Purpose: General checks of poly classes
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 10/98
 *
 ******************************************************************/

#include "GP.h"

/////////////////////////////////////////////////////////////////////
///
/// Examinig Specs and data
///
/////////////////////////////////////////////////////////////////////

bool GP_t::IsSpecOk()
{
  switch(Type())
  {
      case GP_AtomType:
        return Atom() != NULL && Atom()->IsAtomSpecOk();
        
      case GP_PolyType:
        return Poly() != NULL && Poly()->IsPolySpecOk();
        
      case GP_CompType:
        return Comp() != NULL && Comp()->IsCompSpecOk();
        
      default:
        return false;
  }
}
bool GP_t::IsDataOk(const void* data)
{
  switch(Type())
  {
      case GP_AtomType:
        return Atom() != NULL && Atom()->IsAtomDataOk(data);
        
      case GP_PolyType:
        return Poly() != NULL && Poly()->IsPolyDataOk(data);
        
      case GP_CompType:
        return Comp() != NULL && Comp()->IsCompDataOk(data);
        
      default:
        return false;
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Atoms
///
/////////////////////////////////////////////////////////////////////
bool GP_Atom_t::IsAtomSpecOk()
{
  GP_AtomType_t type = AtomType();
  
  if (type == GP_UnknownAtomType || 
      AtomEncoding() == GP_UnknownAtomEncoding)
    return false;
  
  if ((type == GP_ModuloAtomType || type == GP_CharPAtomType) &&
      GP_AtomModulus() == NULL) 
    return false;

  return true;
}
bool GP_Atom_t::IsAtomDataOk(const void* data)
{
  GP_AtomType_t type = AtomType();
  
  if (AtomEncoding() == GP_DynamicAtomEncoding &&
      (AtomEncoding(data) == GP_UnknownAtomEncoding || 
       AtomEncoding(data) == GP_UnknownAtomEncoding))
    return false;
  
  return true;
}

/////////////////////////////////////////////////////////////////////
///
/// Composites
///
/////////////////////////////////////////////////////////////////////
bool GP_Comp_t::IsCompSpecOk()
{
  GP_CompType_t ctype = CompType();
  GP_pt elements = Elements();
  
  if (ctype == GP_UnknownCompType || elements == NULL)
    return false;
  
  if (elements->IsSpecOk() == false) return false;

  switch (ctype)
  {
      case GP_IdealCompType:
        if (elements->Type() != GP_PolyType ||
            elements->Poly()->IsFreeModuleVector())
          return false;
        return true;

      case GP_ModuleCompType:
        if (elements->Type() != GP_PolyType ||
            ! (elements->Poly()->IsFreeModuleVector()))
          return false;
        return true;

      case GP_RationalCompType:
        if (elements->Type() != GP_AtomType ||
            (elements->Atom()->AtomType() != GP_IntegerAtomType))
          return false;
        return true;

      case GP_ComplexCompType:
        if (elements->Type() != GP_AtomType ||
            (elements->Atom()->AtomType() != GP_RealAtomType))
          return false;
        return true;

      case GP_MatrixCompType:
      {
        long dx = -1, dy = -1;
        MatrixDimension(dx, dy);
        if (dx <= 0 || dy <= 0) return false;
        return true;
      }

      default:
        return true;
  }
}
bool GP_Comp_t::IsCompDataOk(const void* data)
{
  GP_Iterator_pt it = ElementDataIterator(data);
  GP_pt elements = Elements();
  GP_CompType_t ctype = CompType();
  long i, n;
  
  
  if (it == NULL || elements == NULL) return false;
  n = it->N();
  if (n < 0) return false;
  
  switch (CompType())
  {
      case GP_MatrixCompType:
      {
        long dx = -1, dy = -1;
        MatrixDimension(dx, dy);
        if (dx < 0 || dy < 0) return false;
        if (dx*dy != n) return false;
        break;
      }
      
      case GP_RationalCompType:
      case GP_QuotientCompType:
      case GP_ComplexCompType:
        if (n == 0 || n > 2) return false;
        break;

      case GP_FreeModuleCompType:
      {
        for (i=0; i<n; i++)
        {
          void* fmel = it->Next();
          long comp = FreeModuleComponent(fmel);
          if (comp < 0) return false;
          if (elements->IsDataOk(FreeModuleElement(fmel)) == false)
            return false;
        }
        return true;
      }
      
      case GP_UnknownCompType:
        return false;
        
      default:
        break;
  }

  for (i=0; i<n; i++)
    if (elements->IsDataOk(it->Next()) == false) return false;
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_Poly_t::IsPolySpecOk()
{
  void* mpoly;

  switch(PolyType())
  {
      case GP_UvPolyType:
        if (UvPoly() != NULL && UvPoly()->IsUvPolySpecOk()) break;
        return false;

      case GP_MvPolyType:
        if (MvPoly() != NULL && MvPoly()->IsMvPolySpecOk()) break;
        return false;

      default:
        return false;
  }
  if (Coeffs() == NULL || Coeffs()->IsSpecOk() == false) return false;
  
  mpoly = MinPoly();
  if (mpoly != NULL) return IsPolyDataOk(mpoly);
  return true;
}
bool GP_Poly_t::IsPolyDataOk(const void* data)
{
  
  switch(PolyType())
  {
      case GP_UvPolyType:
        return UvPoly() != NULL && UvPoly()->IsUvPolyDataOk(data);

      case GP_MvPolyType:
        return MvPoly() != NULL && MvPoly()->IsMvPolyDataOk(data);

      default:
        return false;
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Univariate Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_UvPoly_t::IsUvPolySpecOk()
{
  if (UvPolyType() == GP_UnknownUvPolyType) return false;
  return true;
}

bool GP_UvPoly_t::IsUvPolyDataOk(const void* data)
{
  GP_Iterator_pt it = TermIterator(data);
  bool isSparse = (UvPolyType() == GP_SparseUvPolyType);
  GP_pt coeff = Coeffs();

  long i, n;
  void* term;
  
  
  if (it == NULL) return false;
  n = it->N();
  
  if (n < 0) return false;
  
  for (i = 0; i<n; i++)
  {
    term = it->Next();
    if (isSparse) 
    {
      if (coeff->IsDataOk(ExpCoeff(term)) == false ||
          ExpValue(term) < 0) return false;
    }
    else
    {
      if (coeff->IsDataOk(term) == false) return false;
    }
  }
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// Multivariate Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_MvPoly_t::IsMvPolySpecOk()
{
  if (NumberOfVars() <= 0) 
    return false;
  
  switch(MvPolyType())
  {
      case GP_DistMvPolyType:
        return DistMvPoly() != NULL && DistMvPoly()->IsDistMvPolySpecOk();
        
      case GP_RecMvPolyType:
        return RecMvPoly() != NULL && RecMvPoly()->IsRecMvPolySpecOk();

      default:
        return false;
  }
}
bool GP_MvPoly_t::IsMvPolyDataOk(const void* data)
{
  switch(MvPolyType())
  {
      case GP_DistMvPolyType:
        return DistMvPoly() != NULL && DistMvPoly()->IsDistMvPolyDataOk(data);
        
      case GP_RecMvPolyType:
        return RecMvPoly() != NULL && RecMvPoly()->IsRecMvPolyDataOk(data);

      default:
        return false;
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Distributed multivariate polys
///
/////////////////////////////////////////////////////////////////////
bool GP_DistMvPoly_t::IsDistMvPolySpecOk()
{

  if (DistMvPolyType() == GP_UnknownDistMvPolyType) return false;

  GP_Ordering_pt has_ordering = HasOrdering();
  GP_Ordering_pt should_ordering = ShouldHaveOrdering();
  
  return 
    (has_ordering == NULL || (has_ordering->IsOk(NumberOfVars()))) &&
    (should_ordering == NULL || (should_ordering->IsOk(NumberOfVars())));
}
bool GP_DistMvPoly_t::IsDistMvPolyDataOk(const void* data)
{
  GP_DistMvPolyType_t type = DistMvPolyType();
  GP_Iterator_pt    monoms = MonomIterator(data);
  GP_pt             coeffs = Coeffs();
  void*             monom;
  long i, n,j,       nvars  = NumberOfVars();
  GP_Iterator_pt    expvector = NULL;

  if (type ==  GP_UnknownDistMvPolyType ||
      monoms == NULL ||
      coeffs == NULL ||
      nvars <= 0 ||
      (n = monoms->N()) < 0)
    return false;
  
  for (i=0; i<n; i++)
  {
    monom = monoms->Next();
    if (coeffs->IsDataOk(Coeff(monom)) == false) return false;
    
    if (type == GP_SparseDistMvPolyType)
    {
      long m;
      void* exp;

      if (expvector == NULL)  
      {
        expvector = ExpVectorIterator(monom);
        if (expvector == NULL) return false;
      }
      else expvector->Reset(monom);
      
      m = expvector->N();
      if (m < 0) return false;
      
      for (j = 0; j < m; j++)
      {
        exp = expvector->Next();
        if (ExpValue(exp) < 0 || ExpNumber(exp) < 0 || ExpNumber(exp) >= nvars)
          return false;
      }
    }
    else
    {
      long* evector = NULL;
      ExpVector(monom, evector);
      
      if (evector == NULL) return NULL;
      for (j = 0; j<nvars; j++)
      {
        if (evector[j] < 0) return false;
      }
    }
  }
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// recursive multivariate polys
///
/////////////////////////////////////////////////////////////////////
bool GP_RecMvPoly_t::IsRecMvPolySpecOk()
{
  return true;
}
bool GP_RecMvPoly_t::IsRecMvPolyDataOk(const void* data)
{
  if (IsNull(data)) return true;
  if (IsCoeff(data)) return Coeffs()->IsDataOk(data);

  return (
    Variable(data) >= 0 && Variable(data) < NumberOfVars() &&
    Exponent(data) > 0 &&
    IsRecMvPolyDataOk(AddSubPoly(data)) &&
    IsRecMvPolyDataOk(MultSubPoly(data))
    );
}


/////////////////////////////////////////////////////////////////////
///
/// Orderings
///
/////////////////////////////////////////////////////////////////////
bool GP_Ordering_t::IsBlockOrderingOk(const void* block_ordering)
{
  long length = BlockLength(block_ordering);
  
  if (length <= 0) return false;
  
  switch (OrderingType(block_ordering))
  {
      case GP_UnknownOrdering:
      case GP_ProductOrdering: 
        return false;

      case GP_IncrCompOrdering:
      case GP_DecrCompOrdering:
        if (length != 1) return false;
        return true;
        
      case GP_MatrixOrdering:
      {
        GP_Iterator_pt iter = WeightsIterator(block_ordering);
        if (iter == NULL) return false;
        if (iter->N() != length*length) return false;
        return true;
      }
        
      default:
      {
        GP_Iterator_pt iter = WeightsIterator(block_ordering);
        if (iter == NULL || iter->N() > 0 && iter->N() <= length)
          return true;
        return false;
      }
  }
}
    
bool GP_Ordering_t::IsOk(const long nvars)
{
  if (nvars <= 0) return false;
  
  switch(OrderingType())
  {
      case GP_UnknownOrdering:
        return false;
        
      case GP_IncrCompOrdering:
      case GP_DecrCompOrdering:
        // incomplete orderings are no good
        return false;
        
      case GP_MatrixOrdering:
      {
        GP_Iterator_pt iter = WeightsIterator();
        if (iter == NULL) return false;
        if (iter->N() != nvars*nvars) return false;
        return true;
      }
      
      case GP_ProductOrdering:
      {
        GP_Iterator_pt iter = BlockOrderingIterator();
        long i, n = 0, length = 0;
        void* block_ordering;
        
        if (iter !=NULL) 
        {
          for (i=0, n = iter->N(); i<n; i++)
          {
            block_ordering = iter->Next();
            if (! IsBlockOrderingOk(block_ordering)) return false;
            length += BlockLength(block_ordering);
          }
        }
        return (length == nvars);
      }
      
      default:
      {
        GP_Iterator_pt it = WeightsIterator();
        if (it == NULL || (it->N() > 0 && it->N() <= nvars)) return true;
        return false;
      }
  }
}


#if 0
//
//OBJECTS = mpGP.o
//
//olaf.mdm:	$(OBJECTS) olaf.cc
//		mmg -g -v olaf.cc $(OBJECTS)
//

MFUNC( test, MCnop )
{ MFnargsCheck(1);

  GP_pt GPobj = mpGetGP( MFarg(1) );

  if( GPobj == NULL ) 
      MFreturn( MFcopy(MVunknown) );
  if( mpCheckGP(GPobj) ) 
      MFreturn( MFcopy(MVtrue) );
  else
      MFreturn( MFcopy(MVfalse) );
} MFEND

#endif
