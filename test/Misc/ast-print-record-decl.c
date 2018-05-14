// Check struct:
//
//   First check compiling and printing of this file.
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -DKW=struct -DBASES= -o - %s \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print -DKW=struct -DBASES= %s > %t.c
//   RUN: FileCheck --check-prefixes=CHECK,PRINT -DKW=struct -DBASES= %s \
//   RUN:           --input-file %t.c
//
//   Now check compiling and printing of the printed file.
//
//   RUN: echo "// expected""-warning@* 10 {{'T' is deprecated}}" >> %t.c
//   RUN: echo "// expected""-note@* 10 {{'T' has been explicitly marked deprecated here}}" >> %t.c
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -o - %t.c \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print %t.c \
//   RUN: | FileCheck --check-prefixes=CHECK,PRINT -DKW=struct -DBASES= %s

// Repeat for union:
//
//   First check compiling and printing of this file.
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -DKW=union -DBASES= -o - %s \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print -DKW=union -DBASES= %s > %t.c
//   RUN: FileCheck --check-prefixes=CHECK,PRINT -DKW=union -DBASES= %s \
//   RUN:           --input-file %t.c
//
//   Now check compiling and printing of the printed file.
//
//   RUN: echo "// expected""-warning@* 10 {{'T' is deprecated}}" >> %t.c
//   RUN: echo "// expected""-note@* 10 {{'T' has been explicitly marked deprecated here}}" >> %t.c
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -o - %t.c \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print %t.c \
//   RUN: | FileCheck --check-prefixes=CHECK,PRINT -DKW=union -DBASES= %s

// Repeat for C++ (BASES helps ensure we're printing as C++ not as C):
//
//   First check compiling and printing of this file.
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -DKW=struct -DBASES=' : B' -o - \
//   RUN:        -xc++ %s \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print -DKW=struct -DBASES=' : B' -xc++ %s \
//   RUN: > %t.cpp
//   RUN: FileCheck --check-prefixes=CHECK,PRINT,CXX -DKW=struct \
//   RUN:           -DBASES=' : B' %s --input-file %t.cpp
//
//   Now check compiling and printing of the printed file.
//
//   RUN: echo "// expected""-warning@* 10 {{'T' is deprecated}}" >> %t.cpp
//   RUN: echo "// expected""-note@* 10 {{'T' has been explicitly marked deprecated here}}" >> %t.cpp
//
//   RUN: %clang -Xclang -verify -S -emit-llvm -o - %t.cpp \
//   RUN: | FileCheck --check-prefixes=CHECK,LLVM %s
//
//   RUN: %clang_cc1 -verify -ast-print %t.cpp \
//   RUN: | FileCheck --check-prefixes=CHECK,PRINT,CXX -DKW=struct \
//   RUN:             -DBASES=' : B' %s

// END.

#ifndef KW
# error KW undefined
# define KW struct // help syntax checkers
#endif

#ifndef BASES
# error BASES undefined
# define BASES // help syntax checkers
#endif

struct B {};

// CHECK-LABEL: defFirst
void defFirst() {
  // PRINT-NEXT: [[KW]]
  // PRINT-DAG:  __attribute__((aligned(16)))
  // PRINT-DAG:  __attribute__((deprecated("")))
  // PRINT-NOT:  __attribute__
  // PRINT-SAME: T[[BASES]] {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT: } *p0;
  // expected-warning@+2 {{'T' is deprecated}}
  // expected-note@+1 2 {{'T' has been explicitly marked deprecated here}}
  KW __attribute__((aligned(16))) __attribute__((deprecated(""))) T BASES {
    int i;
  } *p0;

  // PRINT-NEXT: [[KW]] T *p1;
  KW T *p1; // expected-warning {{'T' is deprecated}}

  // LLVM: store i64 16
  long s0 = sizeof *p0;
  // LLVM-NEXT: store i64 16
  long s1 = sizeof *p1;
}

// CHECK-LABEL: defLast
void defLast() {
  // PRINT-NEXT: [[KW]] __attribute__((aligned(16))) T *p0;
  KW __attribute__((aligned(16))) T *p0;

  // PRINT-NEXT: [[KW]] __attribute__((deprecated(""))) T[[BASES]] {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT: } *p1;
  // expected-warning@+2 {{'T' is deprecated}}
  // expected-note@+1 {{'T' has been explicitly marked deprecated here}}
  KW __attribute__((deprecated(""))) T BASES { int i; } *p1;

  // LLVM: store i64 16
  long s0 = sizeof *p0;
  // LLVM-NEXT: store i64 16
  long s1 = sizeof *p1;
}

// CHECK-LABEL: defMiddle
void defMiddle() {
  // PRINT-NEXT: [[KW]] __attribute__((deprecated(""))) T *p0;
  // expected-warning@+2 {{'T' is deprecated}}
  // expected-note@+1 3 {{'T' has been explicitly marked deprecated here}}
  KW __attribute__((deprecated(""))) T *p0;

  // PRINT-NEXT: [[KW]] __attribute__((aligned(16))) T[[BASES]] {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT: } *p1;
  KW __attribute__((aligned(16))) T BASES { int i; } *p1; // expected-warning {{'T' is deprecated}}

  // PRINT-NEXT: [[KW]] T *p2;
  KW T *p2; // expected-warning {{'T' is deprecated}}

  // LLVM: store i64 16
  long s0 = sizeof *p0;
  // LLVM-NEXT: store i64 16
  long s1 = sizeof *p1;
  // LLVM-NEXT: store i64 16
  long s2 = sizeof *p2;
}

// CHECK-LABEL: defSelfRef
void defSelfRef() {
  // PRINT-NEXT: [[KW]] __attribute__((deprecated(""))) T *p0;
  // expected-warning@+2 {{'T' is deprecated}}
  // expected-note@+1 2 {{'T' has been explicitly marked deprecated here}}
  KW __attribute__((deprecated(""))) T *p0;

  // PRINT-NEXT: [[KW]] __attribute__((aligned(16))) T[[BASES]] {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT:   [[KW]] T *p2;
  // PRINT-NEXT: } *p1;
  KW __attribute__((aligned(16))) T BASES { // expected-warning {{'T' is deprecated}}
    int i;
    KW T *p2;
  } *p1;

  // LLVM: store i64 16
  long s0 = sizeof *p0;
  // LLVM-NEXT: store i64 16
  long s1 = sizeof *p1;
  // LLVM-NEXT: store i64 16
  long s2 = sizeof *p0->p2;
  // LLVM-NEXT: store i64 16
  long s3 = sizeof *p1->p2;
  // LLVM-NEXT: store i64 16
  long s4 = sizeof *p1->p2->p2;
}

// CHECK-LABEL: declsOnly
void declsOnly() {
  // PRINT-NEXT: [[KW]] T *p0;
  KW T *p0;

  // PRINT-NEXT: [[KW]] __attribute__((may_alias)) T *p1;
  KW __attribute__((may_alias)) T *p1;

  // PRINT-NEXT: [[KW]] T *p2;
  KW T *p2;

  // PRINT-NEXT: [[KW]] __attribute__((deprecated(""))) T *p3;
  // expected-warning@+2 {{'T' is deprecated}}
  // expected-note@+1 2 {{'T' has been explicitly marked deprecated here}}
  KW __attribute__((deprecated(""))) T *p3;

  // PRINT-NEXT: [[KW]] T *p4;
  KW T *p4; // expected-warning {{'T' is deprecated}}
}

// Make sure expanded printing of tag types is turned back off in other parts
// of a tag declaration.  The base class list is checked above.

// CHECK-LABEL: inMembers
void inMembers() {
  // PRINT-NEXT: [[KW]] T1 {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT: };
  KW T1 { int i; };
  // PRINT-NEXT: [[KW]] T2 {
  // PRINT-NEXT:   [[KW]] T1 i;
  // PRINT-NEXT: };
  KW T2 { KW T1 i; };
}

// CHECK-LABEL: inInit
void inInit() {
  // PRINT-NEXT: [[KW]] T1 {
  // PRINT-NEXT:   int i;
  // PRINT-NEXT: };
  KW T1 { int i; };
  // PRINT-NEXT: [[KW]] T2 {
  // PRINT-NEXT:   long i;
  // PRINT-NEXT: } t2 = {sizeof([[KW]] T1)};
  KW T2 { long i; } t2 = {sizeof(KW T1)};
}

#ifdef __cplusplus
// CXX-LABEL: inMemberPtr
void inMemberPtr() {
  // CXX-NEXT: [[KW]] T1 {
  // CXX-NEXT:   int i;
  // CXX-NEXT: };
  KW T1 { int i; };
  // CXX-NEXT: [[KW]] T2 {
  // CXX-NEXT: } T1::*p;
  KW T2 {} T1::*p;
}
#endif