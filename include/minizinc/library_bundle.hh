/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace MiniZinc {

/// A MiniZinc library directory packaged as a single file.
///
/// A bundle is a concatenation of the library's files, each preceded by a
/// pragma naming it:
///
///     /*** @mzn_lib_version 1 ***/
///     /*** @replace_file "globals.mzn" ***/
///     ...
///
/// A bundle can replace a library directory on the include search path.
/// \c \@replace_file entries report locations in the original file, as needed
/// by the standard library. \c \@override_file entries report locations in the
/// bundle, as needed by solver libraries.
class LibraryBundle {
public:
  /// One bundled file. \a data points into the bundle contents, which outlive
  /// every entry, avoiding a copy for each file.
  struct Entry {
    /// File name to report in locations
    std::string location;
    /// Number to add to line numbers reported by the parser
    unsigned int lineOffset;
    /// Contents of the file (not NUL terminated)
    const char* data;
    std::size_t size;
  };

  /// The suffix that identifies a bundle
  static constexpr const char* SUFFIX = ".lib.mzn";

  /// Whether \a path names a library bundle that exists, without loading it
  static bool exists(const std::string& path);

  /// Return the entry for the library file \a key, or nullptr
  const Entry* entry(const std::string& key) const;
  /// Return the unique name identifying the library file \a key
  std::string qualifiedName(const std::string& key) const;
  /// Path of the bundle file
  const std::string& path() const { return _path; }

private:
  friend class LibraryBundleCache;
  LibraryBundle(std::string path, std::string contents);

  std::string _path;
  /// Contents of the whole bundle; entries point into this
  std::string _contents;
  std::unordered_map<std::string, Entry> _entries;
};

/// The library bundles read during one parse.
///
/// A bundle is held only as long as the parse that reads it, so nothing stays
/// resident between compilations. Not thread safe, like the rest of the parser
/// state it belongs to.
class LibraryBundleCache {
public:
  /// Return the bundle at \a path, reading it the first time it is asked for,
  /// or nullptr if \a path is not a bundle
  const LibraryBundle* get(const std::string& path);
  /// Return the entry for \a qualifiedName (as returned by
  /// LibraryBundle::qualifiedName()), or nullptr if it does not name a file in
  /// a bundle read so far. When it does and \a bundle is given, it is set to
  /// the bundle holding the entry.
  const LibraryBundle::Entry* lookup(const std::string& qualifiedName,
                                     const LibraryBundle** bundle = nullptr) const;

private:
  std::map<std::string, std::unique_ptr<LibraryBundle>> _bundles;
};

}  // namespace MiniZinc
