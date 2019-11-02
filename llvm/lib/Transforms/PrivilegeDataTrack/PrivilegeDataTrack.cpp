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

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

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
static RegisterPass<PrivilegeDataTrack> X("privilegeDataTrack", "PrivilegeDataTrack World Pass", 
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);


/**
 *  register the pass as a step of an existing pipeline, some extension points are provided, e.g. PassManagerBuilder::EP_EarlyAsPossible to apply our pass before any optimization, or PassManagerBuilder::EP_FullLinkTimeOptimizationLast to apply it after Link Time Optimizations.
 * 
 * cite: http://llvm.org/docs/WritingAnLLVMPass.html

*/

static RegisterStandardPasses Z(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new PrivilegeDataTrack()); });

namespace {
  // PrivilegeDataTrack2 - The second implementation with getAnalysisUsage implemented, and with attributes 'privileged_data'
  struct PrivilegeDataTrack2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    PrivilegeDataTrack2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "PrivilegeDataTrack: ";
      errs().write_escaped(F.getName()) << "\t return type: ";
      errs().write_escaped(F.getFunctionType()->getStructName());
      errs() << '\n';
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
Y("privilegeDataTrack2", "PrivilegeDataTrack World Pass (with getAnalysisUsage implemented)", 
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);



/**
 *  register the pass as a step of an existing pipeline, some extension points are provided, e.g. PassManagerBuilder::EP_EarlyAsPossible to apply our pass before any optimization, or PassManagerBuilder::EP_FullLinkTimeOptimizationLast to apply it after Link Time Optimizations.
 * 
 * cite: http://llvm.org/docs/WritingAnLLVMPass.html

*/

static RegisterStandardPasses Z2(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new PrivilegeDataTrack2()); });

