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
#include <iostream>

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

    /// Size of the string
    unsigned int size(void) const;
    /// Underlying C string object
    const char* c_str(void) const;
    /// Conversion to STL string
    std::string str(void) const;
    /// Underlying string implementation
    ASTStringO* aststr(void) const { return _s; }

    /// Return if string is equal to \a s
    bool operator== (const ASTString& s) const;
    /// Return if string is not equal to \a s
    bool operator!= (const ASTString& s) const;

    /// Return if string is equal to \a s
    bool operator== (const std::string& s) const;
    /// Return if string is not equal to \a s
    bool operator!= (const std::string& s) const;

    /// Return if string ends with \a s
    bool endsWith(const std::string& s) const;

    /// Return if string begins with \a s
    bool beginsWith(const std::string& s) const;

    /// Compute hash value of string
    size_t hash(void) const;
    
    /// Mark string during garbage collection
    void mark(void) const;
  };

  /// Hash map from strings to \a T
  template<typename T>
  struct ASTStringMap {
    /// The map type specialised for ASTString
    typedef std::unordered_map<ASTString,T> t;
  };

  /**
   * \brief Print integer set \a s
   * \relates Gecode::IntSet
   */
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const ASTString& s) {
    return s.size()==0 ? os : (os << s.c_str());
  }

}

namespace std {
  template<>
  struct hash<MiniZinc::ASTString> {
  public:
    size_t operator()(const MiniZinc::ASTString& s) const;
  };
}

namespace std {
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
    /// Constructor
    ASTStringO(const std::string& s);
  public:
    /// Allocate and initialise as \a s
    static ASTStringO* a(const std::string& s);
    /// Return underlying C-style string
    const char* c_str(void) const { return _data+sizeof(size_t); }
    /// Conversion to STL string
    std::string str(void) const { return std::string(c_str()); }
    /// Return size of string
    unsigned int size(void) const { return static_cast<unsigned int>(_size)-static_cast<unsigned int>(sizeof(size_t))-1; }
    /// Access character at position \a i
    char operator[](unsigned int i) {
      assert(i<size()); return _data[sizeof(size_t)+i];
    }
    /// Return hash value of string
    size_t hash(void) const {
      return reinterpret_cast<const size_t*>(_data)[0];
    }
    /// Mark for garbage collection
    void mark(void) const {
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
  ASTString::mark(void) const { if (_s) _s->mark(); }

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
  inline bool
  ASTString::endsWith(const std::string &s) const {
    return size() >= s.size() &&
      (size() == 0 || strncmp(_s->c_str()+size()-s.size(), s.c_str(), s.size())==0);
  }
  inline bool
  ASTString::beginsWith(const std::string &s) const {
    return size() >= s.size() &&
    (size() == 0 || strncmp(_s->c_str(), s.c_str(), s.size())==0);
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
}

namespace std {
  inline bool
  equal_to<MiniZinc::ASTString>::operator()(const MiniZinc::ASTString& s0,
                                            const MiniZinc::ASTString& s1) 
                                            const {
      return s0==s1;
  }
}

#endif
