//===- PrivilegeDataTrack.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "privilegeDataTrack"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // PrivilegeDataTrack - The first implementation, without getAnalysisUsage.
  struct PrivilegeDataTrack : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    PrivilegeDataTrack() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "PrivilegeDataTrack: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char PrivilegeDataTrack::ID = 0;
static RegisterPass<PrivilegeDataTrack> X("privilegeDataTrack", "PrivilegeDataTrack World Pass");

namespace {
  // PrivilegeDataTrack2 - The second implementation with getAnalysisUsage implemented.
  struct PrivilegeDataTrack2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    PrivilegeDataTrack2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "PrivilegeDataTrack: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char PrivilegeDataTrack2::ID = 0;
static RegisterPass<PrivilegeDataTrack2>
Y("privilegeDataTrack2", "PrivilegeDataTrack World Pass (with getAnalysisUsage implemented)");
