// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

// #include <stdio.h>
// #include "hello.h"

// int main()
// {
//     hello_world();
//     return 0;
// }

// // Note:
// // ---only on Windows---
// // Every function needs to be exported to be able to access the functions by dart.
// // Refer: https://stackoverflow.com/q/225432/8608146
// void hello_world()
// {
//     printf("Hello World\n");
// }

#include <iostream>

// int add(int a, int b) {
//     return a + b;
// }
extern "C" {
    void hello_world() {
        std::cout << "Hello from myFunction!" << std::endl;
    }
    
}
// extern "C" {
//     __attribute__((visibilitys("default"))) int add(int a, int b) {
//         return a + b;
//     }
// }
