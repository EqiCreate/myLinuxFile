import 'dart:ffi' as ffi;
import 'dart:ffi';
import 'dart:io' show Platform, Directory;
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import 'package:path/path.dart' as path;

// FFI signature of the hello_world C function
typedef HelloWorldFunc = ffi.Void Function();
// Dart type definition for calling the C foreign function
typedef HelloWorld = void Function();

typedef SumCFunction = ffi.Int32 Function(ffi.Int32 a, ffi.Int32 b);
typedef SumDartFunction = int Function(int a, int b);

typedef TransCFunction = ffi.Int8 Function(
    ffi.Pointer<ffi.Uint8> array, ffi.Int32 len);
typedef TransDartFunction = int Function(Pointer<ffi.Uint8> array, int len);

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
  final sum = dylib.lookupFunction<SumCFunction, SumDartFunction>('Sum');
  final trans =
      dylib.lookupFunction<TransCFunction, TransDartFunction>('trans');

  // Call the function
  hello();
  print(sum(1, 2));

  final data = Uint8List.fromList([1, 2, 3, 4, 6]);

  // Allocate memory for the data
  // final dataPtr = data.buffer.asUint8List().elementAt(0);
  // dataPtr.asTypedList(data.length).setAll(0, data);
  final dataPtr = calloc<Uint8>(data.length);
  dataPtr.asTypedList(data.length).setAll(0, data);
  final result = trans(dataPtr, data.length);
  calloc.free(dataPtr);
}
