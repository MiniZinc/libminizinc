/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <minizinc/file_utils.hh>
#include <minizinc/exception.hh>

/// Convenience macro for loading symbols
#define load_symbol(name) *(void**)(&name) = symbol(#name)

namespace MiniZinc {
  /// Base class for plugins loaded from DLLs
  class Plugin {
  public:
    class PluginError : public Exception {
    public:
      /// Construct with message \a msg
      PluginError(const std::string& msg) : Exception(msg) {}
      /// Destructor
      ~PluginError(void) throw() {}
      /// Return description
      virtual const char* what(void) const throw() {
        return "MiniZinc: plugin loading error";
      }
    };

    /// Load a plugin with given DLL path
    Plugin(const std::string& file) {
      if (MiniZinc::FileUtils::is_absolute(file)) {
        open(file);
      } else {
        // TODO: this should probably check that there is no current file extension
#ifdef _WIN32
        open(file + ".dll");
#elif __APPLE__
        open(file + ".dylib");
#else
        open(file + ".so");
#endif
      }
      if (dll == nullptr)
        throw PluginError("Failed to load plugin " + file);
    }
    /// Load a plugin by trying the given DLL file paths
    Plugin(const std::vector<std::string>& files) {
      for (auto file : files) {
        if (MiniZinc::FileUtils::is_absolute(file)) {
          open(file);
        } else {
          // TODO: this should probably check that there is no current file extension
#ifdef _WIN32
          open(file + ".dll");
#else
          open(file + ".so");
#endif
        }
        if (dll != nullptr)
          return;
      }
      bool first = true;
      std::stringstream ss;
      ss << "Failed to load plugin. Tried ";
      for (auto file : files) {
        if (first)
          first = false;
        else
          ss << ", ";
        ss << file;
      }
      throw PluginError(ss.str());
    }

    ~Plugin() {
      close();
    }
  protected:
    /// Load a symbol from this DLL
    void* symbol(const char* name) {
      void* ret;
#ifdef _WIN32
      ret = (void*)GetProcAddress((HMODULE)dll, (LPCSTR)name);
#else
      ret = dlsym(dll, name);
#endif
      if (ret == nullptr)
        throw PluginError(std::string("Failed to load symbol ") + name);
      return ret;
    }
  private:
    void* dll;
    void open(const std::string& file) {
#ifdef _WIN32
      dll = (void*)LoadLibrary((LPCSTR)file.c_str());
#else
      dll = dlopen(file.c_str(), RTLD_NOW);
#endif
    }
    void close() {
#ifdef _WIN32
      FreeLibrary((HMODULE)dll);
#else
      dlclose(dll);
#endif
      dll = nullptr;
    }
  };
}
