/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/library_bundle.hh>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace MiniZinc {

namespace {

const char* const VERSION_PRAGMA = "/*** @mzn_lib_version ";
const char* const REPLACE_PRAGMA = "/*** @replace_file ";
const char* const OVERRIDE_PRAGMA = "/*** @override_file ";
const char* const PRAGMA_END_TAIL = " ***/";
const int SUPPORTED_VERSION = 1;

bool starts_with(const std::string& s, size_t pos, const char* prefix) {
  size_t n = std::strlen(prefix);
  return s.size() - pos >= n && s.compare(pos, n, prefix) == 0;
}

/// Return the end of the line starting at \a pos (i.e. the position of the
/// newline character, or the end of \a s)
size_t line_end(const std::string& s, size_t pos) {
  size_t nl = s.find('\n', pos);
  return nl == std::string::npos ? s.size() : nl;
}

bool is_blank(const std::string& s, size_t from, size_t to) {
  for (size_t i = from; i < to; i++) {
    if (s[i] != ' ' && s[i] != '\t' && s[i] != '\r') {
      return false;
    }
  }
  return true;
}

/// Read the quoted string starting at \a pos (which must be the opening
/// quote), leaving \a pos just past the closing quote
bool read_quoted(const std::string& s, size_t& pos, size_t end, std::string& out) {
  if (pos >= end || s[pos] != '"') {
    return false;
  }
  size_t stop = s.find('"', pos + 1);
  if (stop == std::string::npos || stop >= end) {
    return false;
  }
  out = s.substr(pos + 1, stop - pos - 1);
  pos = stop + 1;
  return true;
}

/// If the line [\a pos, \a end) is a file pragma, set \a key, \a isOverride
/// and \a origin (empty unless the pragma gives one) and return true
bool parse_file_pragma(const std::string& s, size_t pos, size_t end, std::string& key,
                       bool& isOverride, std::string& origin) {
  if (starts_with(s, pos, REPLACE_PRAGMA)) {
    isOverride = false;
    pos += std::strlen(REPLACE_PRAGMA);
  } else if (starts_with(s, pos, OVERRIDE_PRAGMA)) {
    isOverride = true;
    pos += std::strlen(OVERRIDE_PRAGMA);
  } else {
    return false;
  }
  std::string parsedKey;
  if (!read_quoted(s, pos, end, parsedKey)) {
    return false;
  }
  // A layered bundle records where each file came from, relative to the
  // directory holding the bundle. Only a second quoted name is one: the space
  // before the closing "***/" looks the same up to this point.
  std::string parsedOrigin;
  if (pos + 1 < end && s[pos] == ' ' && s[pos + 1] == '"') {
    pos++;
    if (!read_quoted(s, pos, end, parsedOrigin)) {
      return false;
    }
  }
  if (!starts_with(s, pos, PRAGMA_END_TAIL)) {
    return false;
  }
  pos += std::strlen(PRAGMA_END_TAIL);
  if (pos > end || !is_blank(s, pos, end)) {
    return false;
  }
  key = parsedKey;
  origin = parsedOrigin;
  return true;
}

}  // namespace

LibraryBundle::LibraryBundle(std::string path, std::string contents)
    : _path(std::move(path)), _contents(std::move(contents)) {
  std::string root = _path.substr(0, _path.size() - std::strlen(SUFFIX));

  size_t pos = 0;
  size_t end = line_end(_contents, pos);
  if (!starts_with(_contents, pos, VERSION_PRAGMA)) {
    throw Error("File '" + _path + "' is not a MiniZinc library bundle.");
  }
  int version = std::atoi(_contents.c_str() + std::strlen(VERSION_PRAGMA));
  if (version != SUPPORTED_VERSION) {
    std::ostringstream oss;
    oss << "Unsupported MiniZinc library bundle version " << version << " in file '" << _path
        << "' (this version of MiniZinc supports version " << SUPPORTED_VERSION << ").";
    throw Error(oss.str());
  }

  // Accumulator for the entry currently being read
  bool haveEntry = false;
  std::string key;
  std::string origin;
  bool isOverride = false;
  size_t contentStart = 0;
  unsigned int contentLine = 0;

  unsigned int lineNo = 1;
  auto finish = [&](size_t contentEnd) {
    if (!haveEntry) {
      return;
    }
    Entry e;
    // Normalised, so that a location reported for a bundled file is spelled the
    // same as one reported for the file on disk (native separators on Windows).
    // A layered bundle names each file's own library, others sit under the
    // directory the bundle was built from.
    if (isOverride) {
      e.location = _path;
    } else if (origin.empty()) {
      e.location = FileUtils::file_path(root + "/" + key);
    } else {
      e.location = FileUtils::file_path(FileUtils::dir_name(_path) + "/" + origin);
    }
    e.lineOffset = isOverride ? contentLine - 1 : 0;
    e.data = _contents.data() + contentStart;
    e.size = contentEnd - contentStart;
    _entries.emplace(key, std::move(e));
  };

  while (end < _contents.size()) {
    pos = end + 1;
    lineNo++;
    end = line_end(_contents, pos);
    std::string nextKey;
    std::string nextOrigin;
    bool nextIsOverride = false;
    if (parse_file_pragma(_contents, pos, end, nextKey, nextIsOverride, nextOrigin)) {
      finish(pos);
      haveEntry = true;
      key = nextKey;
      origin = nextOrigin;
      isOverride = nextIsOverride;
      contentStart = std::min(end + 1, _contents.size());
      contentLine = lineNo + 1;
    }
  }
  finish(_contents.size());
}

const LibraryBundle::Entry* LibraryBundle::entry(const std::string& key) const {
  auto it = _entries.find(key);
  return it == _entries.end() ? nullptr : &it->second;
}

std::string LibraryBundle::qualifiedName(const std::string& key) const { return _path + "/" + key; }

namespace {
/// \a path without any trailing separators, or the empty string if it does not
/// end in the bundle suffix
std::string bundle_key(const std::string& path) {
  size_t len = path.size();
  while (len > 0 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
    len--;
  }
  size_t suffixLen = std::strlen(LibraryBundle::SUFFIX);
  if (len <= suffixLen || path.compare(len - suffixLen, suffixLen, LibraryBundle::SUFFIX) != 0) {
    return {};
  }
  return path.substr(0, len);
}
}  // namespace

bool LibraryBundle::exists(const std::string& path) {
  std::string key = bundle_key(path);
  return !key.empty() && FileUtils::file_exists(key);
}

const LibraryBundle* LibraryBundleCache::get(const std::string& path) {
  std::string key = bundle_key(path);
  if (key.empty()) {
    return nullptr;
  }
  auto it = _bundles.find(key);
  if (it != _bundles.end()) {
    return it->second.get();
  }
  if (!FileUtils::file_exists(key)) {
    return nullptr;
  }
  std::unique_ptr<LibraryBundle> b(new LibraryBundle(key, FileUtils::read_file_contents(key)));
  auto* ret = b.get();
  _bundles.emplace(key, std::move(b));
  return ret;
}

const LibraryBundle::Entry* LibraryBundleCache::lookup(const std::string& qualifiedName,
                                                       const LibraryBundle** bundle) const {
  for (const auto& [p, b] : _bundles) {
    if (qualifiedName.size() > p.size() + 1 && qualifiedName[p.size()] == '/' &&
        qualifiedName.compare(0, p.size(), p) == 0) {
      const LibraryBundle::Entry* e = b->entry(qualifiedName.substr(p.size() + 1));
      if (e != nullptr && bundle != nullptr) {
        *bundle = b.get();
      }
      return e;
    }
  }
  return nullptr;
}

}  // namespace MiniZinc
