import 'dart:ffi' as ffi;
import 'dart:io' show Platform, Directory;

import 'package:path/path.dart' as path;

// FFI signature of the hello_world C function
typedef HelloWorldFunc = ffi.Void Function();
// Dart type definition for calling the C foreign function
typedef HelloWorld = void Function();

void test() {
  // Open the dynamic library
  var libraryPath =
      path.join(Directory.current.path, 'hello_library', 'my_dll.dll');

  if (Platform.isMacOS) {
    libraryPath =
        path.join(Directory.current.path, 'hello_library', 'libhello.dylib');
  }

  if (Platform.isWindows) {
    libraryPath = path.join(
        Directory.current.path, 'hello_library', 'Debug', 'hello.dll');
  }

  final dylib = ffi.DynamicLibrary.open(libraryPath);

  final hello = dylib.lookupFunction<HelloWorldFunc, HelloWorld>('hello_world');
  // Call the function
  hello();
}
