//===--- LangOptions.h - C Language Family Language Options -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the clang::LangOptions interface.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_BASIC_LANGOPTIONS_H
#define LLVM_CLANG_BASIC_LANGOPTIONS_H

#include "clang/Basic/CommentOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/ObjCRuntime.h"
#include "clang/Basic/Visibility.h"
#include <string>

namespace clang {

struct SanitizerOptions {
#define SANITIZER(NAME, ID) unsigned ID : 1;
#include "clang/Basic/Sanitizers.def"
  /// \brief Controls how agressive is asan field padding (0: none, 1: least
  /// aggressive, 2: more aggressive).
  unsigned SanitizeAddressFieldPadding : 2;

  /// \brief Path to blacklist file specifying which objects
  /// (files, functions, variables) should not be instrumented.
  std::string BlacklistFile;

  SanitizerOptions();

  /// \brief Disable all sanitizers.
  void clear();
};

/// Bitfields of LangOptions, split out from LangOptions in order to ensure that
/// this large collection of bitfields is a trivial class type.
class LangOptionsBase {
public:
  // Define simple language options (with no accessors).
#define LANGOPT(Name, Bits, Default, Description) unsigned Name : Bits;
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description)
#include "clang/Basic/LangOptions.def"

  SanitizerOptions Sanitize;
protected:
  // Define language options of enumeration type. These are private, and will
  // have accessors (below).
#define LANGOPT(Name, Bits, Default, Description)
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) \
  unsigned Name : Bits;
#include "clang/Basic/LangOptions.def"
};

/// \brief Keeps track of the various options that can be
/// enabled, which controls the dialect of C or C++ that is accepted.
class LangOptions : public LangOptionsBase {
public:
  typedef clang::Visibility Visibility;
  
  enum GCMode { NonGC, GCOnly, HybridGC };
  enum StackProtectorMode { SSPOff, SSPOn, SSPStrong, SSPReq };
  
  enum SignedOverflowBehaviorTy {
    SOB_Undefined,  // Default C standard behavior.
    SOB_Defined,    // -fwrapv
    SOB_Trapping    // -ftrapv
  };

  enum PragmaMSPointersToMembersKind {
    PPTMK_BestCase,
    PPTMK_FullGeneralitySingleInheritance,
    PPTMK_FullGeneralityMultipleInheritance,
    PPTMK_FullGeneralityVirtualInheritance
  };

  enum AddrSpaceMapMangling { ASMM_Target, ASMM_On, ASMM_Off };

public:
  clang::ObjCRuntime ObjCRuntime;

  std::string ObjCConstantStringClass;
  
  /// \brief The name of the handler function to be called when -ftrapv is
  /// specified.
  ///
  /// If none is specified, abort (GCC-compatible behaviour).
  std::string OverflowHandler;

  /// \brief The name of the current module.
  std::string CurrentModule;

  /// \brief The name of the module that the translation unit is an
  /// implementation of. Prevents semantic imports, but does not otherwise
  /// treat this as the CurrentModule.
  std::string ImplementationOfModule;

  /// \brief Options for parsing comments.
  CommentOptions CommentOpts;
  
  /// \brief ID passed to the frontend that identifies the module.
  /// The same ID must be passed for all targets for a given compilation
  /// unit.
  std::string OMPModuleUniqueID;

  /// \brief Triples of the OpenMP targets that the host code
  /// codegen should take into account in order to generate
  /// accurate offloading translation tables
  std::vector<llvm::Triple> OMPTargetTriples;

  LangOptions();

  // Define accessors/mutators for language options of enumeration type.
#define LANGOPT(Name, Bits, Default, Description) 
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) \
  Type get##Name() const { return static_cast<Type>(Name); } \
  void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }  
#include "clang/Basic/LangOptions.def"
  
  bool isSignedOverflowDefined() const {
    return getSignedOverflowBehavior() == SOB_Defined;
  }
  
  bool isSubscriptPointerArithmetic() const {
    return ObjCRuntime.isSubscriptPointerArithmetic() &&
           !ObjCSubscriptingLegacyRuntime;
  }

  /// \brief Reset all of the options that are not considered when building a
  /// module.
  void resetNonModularOptions();
};

/// \brief Floating point control options
class FPOptions {
public:
  unsigned fp_contract : 1;

  FPOptions() : fp_contract(0) {}

  FPOptions(const LangOptions &LangOpts) :
    fp_contract(LangOpts.DefaultFPContract) {}
};

/// \brief OpenCL volatile options
class OpenCLOptions {
public:
#define OPENCLEXT(nm)  unsigned nm : 1;
#include "clang/Basic/OpenCLExtensions.def"

  OpenCLOptions() {
#define OPENCLEXT(nm)   nm = 0;
#include "clang/Basic/OpenCLExtensions.def"
  }
};

/// \brief Describes the kind of translation unit being processed.
enum TranslationUnitKind {
  /// \brief The translation unit is a complete translation unit.
  TU_Complete,
  /// \brief The translation unit is a prefix to a translation unit, and is
  /// not complete.
  TU_Prefix,
  /// \brief The translation unit is a module.
  TU_Module
};
  
}  // end namespace clang

#endif
