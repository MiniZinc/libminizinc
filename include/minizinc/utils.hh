/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_UTILS_H__
#define __MINIZINC_UTILS_H__

#include <string>
#include <sstream>
#include <ctime>
#include <limits>
#include <iomanip>


using namespace std;

namespace MiniZinc {
  
  inline std::string stoptime(clock_t& start) {
    std::ostringstream oss;
    clock_t now = clock();
    oss << std::setprecision(0) << std::fixed << ((static_cast<double>(now-start) / CLOCKS_PER_SEC) * 1000.0) << " ms";
    start = now;
    return oss.str();
  }

  inline std::string timeDiff(clock_t t2, clock_t t1) {
    std::ostringstream oss;
    oss << std::setprecision(2) << std::fixed << ((static_cast<double>(t2-t1) / CLOCKS_PER_SEC)) << " s";
    return oss.str();
  }

  inline bool beginswith(string s, string t) {
    return s.compare(0, t.length(), t)==0;
  }

  inline void checkIOStatus( bool fOk, string msg )
  {
    if ( !fOk ) {
      std::cerr << "\n  " << msg << strerror(errno) << "." << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  
  template <class T> inline bool assignStr(T*, const string ) { return false; }
  template<> inline bool assignStr(string* pS, const string s ) {
    *pS = s;
    return true;
  }
  
  /// A simple per-cmdline option parser
  class CLOParser {
    int& i;              // current item
    const int argc=0;
    const char* const* argv=0;
    
  public:
    CLOParser( int& ii, const int ac, const char* const* av )
      : i(ii), argc(ac), argv(av) { }
    template <class Value=int>
    inline bool getOption(  const char* names, // space-separated option list
                            Value* pResult=nullptr, // pointer to value storage
                            bool fValueOptional=false // if pResult, for non-string values
                ) {
      if( i>=argc )
        return false;
      assert( argv[i] );
      string arg( argv[i] );
      /// Separate keywords
      string keyword;
      istringstream iss( names );
      while ( iss >> keyword ) {
        if ( 0!=arg.compare( 0, keyword.size(), keyword ) )
          continue;
        /// Process it
        if ( keyword.size() < arg.size() ) {
          if ( 0==pResult )
            continue;
          arg.erase( 0, keyword.size() );
        } else {
          if ( 0==pResult )
            return true;
          i++;
          if( i>=argc )
            return false;
          arg = argv[i];
        }
        assert( pResult );
        if ( assignStr( pResult, arg ) ) 
          return true;
        istringstream iss( arg );
        if ( !( iss >> (*pResult) ) ) {
          if ( fValueOptional ) {
            --i;
            return true;
          }
          cerr << "\nBad value for " << keyword << ": " << arg << endl;
          return false;
        }
        return true;
      }
      return false;
    }
  };  // class CLOParser
  
  /// This class prints a value if non-0 and adds comma if not 1st time
  class HadOne {
    bool fHadOne=false;
  public:
    template <class N>
    string operator()(const N& val, const char* descr=0) {
      ostringstream oss;
      if ( val ) {
        if ( fHadOne )
          oss << ", ";
        fHadOne=true;
        oss << val;
        if ( descr )
          oss << descr;
      }
      return oss.str();
    }
    void reset() { fHadOne=false; }
    operator bool() const { return fHadOne; }
    bool operator!() const { return !fHadOne; }
  };

}

#endif  // __MINIZINC_FLATTENER_H__

