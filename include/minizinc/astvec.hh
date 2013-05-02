/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ASTVEC_HH__
#define __MINIZINC_ASTVEC_HH__

#include <minizinc/gc.hh>

#include <vector>

namespace MiniZinc {

  class ASTIntVecO;
  
  /**
   * \brief Handler for ASTIntVecO objects
   */
  class ASTIntVec {
  protected:
    /// Vector
    ASTIntVecO* _v;
  public:
    /// Default constructor
    ASTIntVec(void) : _v(NULL) {}
    /// Constructor
    ASTIntVec(ASTIntVecO* v) : _v(v) {}
    /// Constructor
    ASTIntVec(const std::vector<int>& v);
    /// Copy constructor
    ASTIntVec(const ASTIntVec& s);
    /// Assignment operator
    ASTIntVec& operator= (const ASTIntVec& s);

    /// Size of vector
    unsigned int size(void) const;
    /// Element access
    int& operator[](unsigned int i);
    /// Element access
    const int operator[](unsigned int i) const;
    /// Iterator begin
    int* begin(void);
    /// Iterator end
    int* end(void);
  };
  
  template<class> class ASTNodeVecO;

  /**
   * \brief Handler for ASTIntVecO objects
   */
  template<class T>
  class ASTNodeVec {
  protected:
    /// Vector
    ASTNodeVecO<T*>* _v;
  public:
    /// Default constructor
    ASTNodeVec(void) : _v(NULL) {}
    /// Constructor
    ASTNodeVec(ASTNodeVecO<T*>* v) : _v(v) {}
    /// Constructor
    ASTNodeVec(const std::vector<T*>& v);
    /// Copy constructor
    ASTNodeVec(const ASTNodeVec& v);
    /// Assignment operator
    ASTNodeVec& operator= (const ASTNodeVec& v);

    /// Size of vector
    unsigned int size(void) const;
    /// Element access
    T*& operator[](unsigned int i);
    /// Element access
    const T* operator[](unsigned int i) const;
    /// Iterator begin
    T** begin(void);
    /// Iterator end
    T** end(void);
    
    ASTNodeVecO<T*>* vec(void);
  };
  
  class ASTIntVecO : public ASTChunk {
  protected:
    ASTIntVecO(const std::vector<int>& v);
  public:
    static ASTIntVecO* a(const std::vector<int>& v);
    unsigned int size(void) const { return _size/sizeof(int); }
    int& operator[](unsigned int i) {
      assert(i<size());
      return reinterpret_cast<int*>(_data)[i];
    }
    const int operator[](unsigned int i) const {
      assert(i<size());
      return reinterpret_cast<const int*>(_data)[i];
    }
    /// Iterator begin
    int* begin(void) { return reinterpret_cast<int*>(_data); }
    /// Iterator end
    int* end(void) { return begin()+size(); }
  };

  template<class T>
  class ASTNodeVecO : public ASTVec {
  protected:
    ASTNodeVecO(const std::vector<T>& v);
  public:
    static ASTNodeVecO* a(const std::vector<T>& v);
    unsigned int size(void) const { return _size; }
    bool empty(void) const { return size()==0; }
    T& operator[] (int i) {
      assert(i<size()); return reinterpret_cast<T&>(_data[i]);
    }
    const T operator[] (int i) const {
      assert(i<size()); return reinterpret_cast<T>(_data[i]);
    }
    /// Iterator begin
    T* begin(void) { return reinterpret_cast<T*>(_data); }
    /// Iterator end
    T* end(void) { return begin()+size(); }
  };

  template<class T>
  ASTNodeVecO<T>::ASTNodeVecO(const std::vector<T>& v)
    : ASTVec(v.size()) {
    for (unsigned int i=v.size(); i--;)
      (*this)[i] = v[i];
  }
  template<class T>
  ASTNodeVecO<T>*
  ASTNodeVecO<T>::a(const std::vector<T>& v) {
    ASTNodeVecO<T>* ao =
      static_cast<ASTNodeVecO<T>*>(
        alloc(sizeof(ASTNodeVecO<T>)+sizeof(T)*v.size()));
    new (ao) ASTNodeVecO<T>(v);
    return ao;
  }

  inline
  ASTIntVec::ASTIntVec(const std::vector<int>& v) : _v(ASTIntVecO::a(v)) {}
  inline
  ASTIntVec::ASTIntVec(const ASTIntVec& v) : _v(v._v) {}
  inline ASTIntVec&
  ASTIntVec::operator= (const ASTIntVec& v) {
    _v = v._v;
    return *this;
  }

  inline unsigned int
  ASTIntVec::size(void) const {
    return _v ? _v->size() : 0;
  }
  inline int&
  ASTIntVec::operator[](unsigned int i) {
    return (*_v)[i];
  }
  inline const int
  ASTIntVec::operator[](unsigned int i) const {
    return (*_v)[i];
  }
  inline int*
  ASTIntVec::begin(void) { return _v->begin(); }
  inline int*
  ASTIntVec::end(void) { return _v->end(); }

  template<class T>
  ASTNodeVec<T>::ASTNodeVec(const std::vector<T*>& v)
    : _v(ASTNodeVecO<T*>::a(v)) {}
  template<class T>
  inline
  ASTNodeVec<T>::ASTNodeVec(const ASTNodeVec<T>& v)
    : _v(v._v) {}
  template<class T>
  inline ASTNodeVec<T>&
  ASTNodeVec<T>::operator =(const ASTNodeVec<T>& v) {
    _v = v._v;
    return *this;
  }
  template<class T>
  inline unsigned int
  ASTNodeVec<T>::size(void) const {
    return _v ? _v->size() : 0;
  }
  template<class T>
  inline T*&
  ASTNodeVec<T>::operator[](unsigned int i) {
    return (*_v)[i];
  }
  template<class T>
  inline const T*
  ASTNodeVec<T>::operator[](unsigned int i) const {
    return (*_v)[i];
  }
  template<class T>
  inline T**
  ASTNodeVec<T>::begin(void) {
    return _v ? _v->begin() : NULL; }
  template<class T>
  inline T**
  ASTNodeVec<T>::end(void) {
    return _v ? _v->end() : NULL;
  }
  template<class T>
  inline ASTNodeVecO<T*>*
  ASTNodeVec<T>::vec(void) {
    return _v;
  }

}

#endif
