/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ASTSTRING_HH__
#define __MINIZINC_ASTSTRING_HH__

#include <minizinc/gc.hh>
#include <string>
#include <cstring>
#include <functional>
#include <unordered_map>

namespace MiniZinc {
  
  class ASTStringO;
  
  /**
   * \brief Handler for ASTStringO objects
   */
  class ASTString {
  protected:
    /// String
    ASTStringO* _s;
  public:
    /// Default constructor
    ASTString(void) : _s(NULL) {}
    /// Constructor
    ASTString(ASTStringO* s) : _s(s) {}
    /// Constructor
    ASTString(const std::string& s);
    /// Copy constructor
    ASTString(const ASTString& s);
    /// Assignment operator
    ASTString& operator= (const ASTString& s);

    unsigned int size(void) const;
    const char* c_str(void) const;
    std::string str(void) const;
    ASTStringO* aststr(void) const { return _s; }

    bool operator== (const ASTString& s) const;
    bool operator!= (const ASTString& s) const;

    bool operator== (const std::string& s) const;
    bool operator!= (const std::string& s) const;
    
    size_t hash(void) const;
    
    void mark(void);
  };

  template<typename T>
  struct ASTStringMap {
    typedef std::unordered_map<ASTString,T> t;
  };

}

namespace std {
  template<>
  struct hash<MiniZinc::ASTString> {
  public:
    size_t operator()(const MiniZinc::ASTString& s) const;
  };
  template<>
  struct equal_to<MiniZinc::ASTString> {
  public:
    bool operator()(const MiniZinc::ASTString& s0,
                    const MiniZinc::ASTString& s1) const;
  };
}

namespace MiniZinc {

  /**
   * \brief Garbage collected string
   */
  class ASTStringO : public ASTChunk {
  protected:
    ASTStringO(const std::string& s);
  public:
    static ASTStringO* a(const std::string& s);
    const char* c_str(void) const { return _data+sizeof(size_t); }
    std::string str(void) const { return std::string(c_str()); }
    unsigned int size(void) const { return _size-sizeof(size_t); }
    char operator[](unsigned int i) {
      assert(i<size()); return _data[sizeof(size_t)+i];
    }
    size_t hash(void) const {
      return reinterpret_cast<const size_t*>(_data)[0];
    }
    void mark(void) {
      _gc_mark = 1;
    }
  };

  inline
  ASTString::ASTString(const std::string& s) : _s(ASTStringO::a(s)) {}
  inline
  ASTString::ASTString(const ASTString& s) : _s(s._s) {}
  inline ASTString&
  ASTString::operator= (const ASTString& s) {
    _s = s._s;
    return *this;
  }
  inline unsigned int
  ASTString::size(void) const {
    return _s ? _s->size() : 0;
  }
  inline const char*
  ASTString::c_str(void) const { return _s ? _s->c_str() : NULL; }
  inline std::string
  ASTString::str(void) const { return _s ? _s->str() : std::string(""); }
  inline void
  ASTString::mark(void) { if (_s) _s->mark(); }

  inline bool
  ASTString::operator== (const ASTString& s) const {
    return size()==s.size() &&
      (size()==0 || strncmp(_s->c_str(),s._s->c_str(),size())==0);
  }
  inline bool
  ASTString::operator!= (const ASTString& s) const {
    return !(*this == s);
  }
  inline bool
  ASTString::operator== (const std::string& s) const {
    return size()==s.size() &&
      (size()==0 || strncmp(_s->c_str(),s.c_str(),size())==0);
  }
  inline bool
  ASTString::operator!= (const std::string& s) const {
    return !(*this == s);
  }
  
  inline size_t
  ASTString::hash(void) const {
    return _s ? _s->hash() : 0;
  }

}

namespace std {
  inline size_t
  hash<MiniZinc::ASTString>::operator()(
      const MiniZinc::ASTString& s) const {
      return s.hash();
  }
  inline bool
  equal_to<MiniZinc::ASTString>::operator()(const MiniZinc::ASTString& s0,
                                            const MiniZinc::ASTString& s1) 
                                            const {
      return s0==s1;
  }
}

#endif
