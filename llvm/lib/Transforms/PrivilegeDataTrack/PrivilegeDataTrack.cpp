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

// LLM: for invoking pass using clang option
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


// LLM: for instructino visitors
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/IntrinsicInst.h"

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
      errs() << "PrivilegeDataTrack00: ";
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


STATISTIC(HelloCounter2, "Counts number of functions greeted");

namespace {
  // PrivilegeDataTrack2 - The second implementation with getAnalysisUsage implemented, and with attributes 'privileged_data'
 struct PrivilegeDataTrack2 : public FunctionPass, 
       public InstVisitor<PrivilegeDataTrack2>	{

    const DataLayout *DLayout;

   public:
    static char ID; // Pass identification, replacement for typeid
    std::string sourceFileName;
    PrivilegeDataTrack2() : FunctionPass(ID){}

    virtual bool doInitialization(Module &Mod) override;
    virtual bool runOnFunction(Function &F) override;

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }

    virtual StringRef getPassName() const override{
       return "PrivilegeDataTrack";
    } 

    // visitor methods
    void visitLoadInst(LoadInst &LI);
#if 0
    void visitStoreInst(StoreInst &SI);
    void visitAllocaInst(AllocaInst &I);
    void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I);
    void visitAtomicRMWInst(AtomicRMWInst &I);
    void visitMemIntrinsic(MemIntrinsic &MI);
    void visitGetElementPtrInst(GetElementPtrInst &I);
    void visitPHINode(PHINode &I);
    void visitCastInst(CastInst &I);
    void visitUnaryOperator(UnaryOperator &I);
    void visitBinaryOperator(BinaryOperator &I);
    void visitUnaryInstruction(UnaryInstruction &I);
    void visitInstruction(Instruction &I);
#endif 

  };
} /* namespace */


FunctionPass *createPrivilegeDataTrackPass(){
   return new PrivilegeDataTrack2();
}
 

bool PrivilegeDataTrack2::doInitialization(Module &Mod) {
    sourceFileName = Mod.getSourceFileName();
    DLayout = &Mod.getDataLayout();
    return true;
}

bool PrivilegeDataTrack2::runOnFunction(Function &F) {

      ++HelloCounter2;
      errs() << "PrivilegeDataTrack2: ";
      errs() << "in file: " << sourceFileName << "\n";
      errs().write_escaped(F.getName()) << "\t return type: ";

      F.getReturnType()->dump();
      errs() << "\t attribute: ";

      // the following is almost the same as F.getAttributes().dump();
      AttributeList alist = F.getAttributes();
      errs() << "\t PAL[\n";
      for (unsigned i = alist.index_begin(), e = alist.index_end(); i != e; ++i){
         if (alist.getAttributes(i).hasAttributes())
           errs() << "\t  { " << i << " ==> " << alist.getAsString(i) << " }\n" ;
      }
      errs() << "\t ]\n";

      // get the parameters and print the attributes
      int arg_num = F.arg_size();
      for (int i =0 ; i < arg_num; i++){
          errs() << "\t AttributeSet of parameter " << i << "\n";
          F.getAttributes().getParamAttributes(i).dump();
      }

      for (BasicBlock & B: F){
	      errs() << "basic block size: " << B.size() << "\n";
         for (Instruction & inst: B){
		 errs()<< "\tinstruction: " << inst.getOpcodeName() << "\n";
	 }
      }
      errs() << '\n';

      visit(F);

      return false;
}

// visit load instruction; propagate the attribute from source mem to register operands
void PrivilegeDataTrack2::visitLoadInst(LoadInst & LI){
  // todo
  errs() << "LLM: TODO: visit load instruction here \n";
  errs() << "Opcode: " << LI.getOpcodeName() << "\n";
   
}



char PrivilegeDataTrack2::ID = 0;
static RegisterPass<PrivilegeDataTrack2>
Y("privilegeDataTrack2", "PrivilegeDataTrack2 for privilege data tracking",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
/**
 *  register the pass as a step of an existing pipeline, some extension points are provided, e.g. PassManagerBuilder::EP_EarlyAsPossible to apply our pass before any optimization, or PassManagerBuilder::EP_FullLinkTimeOptimizationLast to apply it after Link Time Optimizations.
 *
 * cite: http://llvm.org/docs/WritingAnLLVMPass.html

*/

static RegisterStandardPasses Z2(
//    PassManagerBuilder::EP_EarlyAsPossible,
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new PrivilegeDataTrack2()); });

 

