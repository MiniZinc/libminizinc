
function filePackagerPatch_getPreloadedPackageNode(filename) {
  var fs = require('fs');
  var path = require('path');
  filename = path.normalize(path.resolve(__dirname, filename));
  const buf = fs.readFileSync(filename);
  // convert to ArrayBuffer
  return buf.buffer.slice(buf.byteOffset, buf.byteOffset + buf.byteLength);
}

function filePackagerPatch_isNodeOrShell() {
  var ENVIRONMENT_IS_WEB = typeof window === 'object';
  var ENVIRONMENT_IS_WORKER = typeof importScripts === 'function';
  var ENVIRONMENT_IS_NODE = typeof process === 'object' && typeof require === 'function' && !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_WORKER;
  var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;
  return ENVIRONMENT_IS_NODE || ENVIRONMENT_IS_SHELL;
}

if (typeof location === 'undefined') {
  // create a fake location to overrule the file_packager
  var location = {
    pathname: '/'
  };
}

if (filePackagerPatch_isNodeOrShell()) {
  Module.getPreloadedPackage = Module.getPreloadedPackage || filePackagerPatch_getPreloadedPackageNode;
} else {
  // need a hack to locate relative file in browser settings for the file packager
  var wrappee = Module.locateFile || function (path, prefix) {return prefix + path};
  Module.locateFile = function (path, prefix) {
    if (prefix || !path.endsWith('.data')) {
      return wrappee(path, prefix);
    }
    // file packager is called before a proper script location decection
    var base = _scriptDir ? _scriptDir : (typeof importScripts === 'function' ? self.location.href : (typeof document !== 'undefined' && document.currentScript ? document.currentScript.src : ''));
    if (base.indexOf('blob:') !== 0) {
      base = base.substr(0, base.lastIndexOf('/') + 1);
    } else {
      base = '';
    }
    return wrappee(path, base);
  };
}
