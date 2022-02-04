
#pragma warning(disable : 4267 4244 4146 4141 4291 4624 4996 4819)

#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/Support/raw_ostream.h"
#if LLVM_VERSION_MAJOR >= 11
#include "clang/Basic/LangStandard.h"
#else
#include "clang/Frontend/LangStandard.h"
#endif
#include "clang/Basic/Builtins.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Sema/Sema.h"

using namespace std;

struct ParseOptions {
  string target_triple;
  string sysroot;
  vector<string> definitions;
  vector<string> include_dirs;
  vector<string> system_include_dirs;
};

class HeaderParser : public clang::ASTConsumer {
public:
  explicit HeaderParser(ParseOptions options);
  ~HeaderParser();

  bool ParseAST(const char *filename);

  clang::ASTContext &GetASTContext() { return compiler_.getASTContext(); }

  void print_ast();

private:
  llvm::raw_fd_ostream output_stream_;
  clang::DiagnosticOptions diagnostic_options_;
  clang::CompilerInstance compiler_;
  std::unique_ptr<clang::TargetInfo> target_info_;
};

int main(int argc, const char *argv[]) { return 0; }

HeaderParser::HeaderParser(ParseOptions options) : output_stream_(1, false) {
  std::shared_ptr<clang::CompilerInvocation> compiler_invocation =
      std::make_shared<clang::CompilerInvocation>();
  clang::PreprocessorOptions &preprocessor_options =
      compiler_invocation->getPreprocessorOpts();
  preprocessor_options.addMacroDef("__iris_cpp_parse__");
  for (auto &macro : options.definitions) {
    preprocessor_options.addMacroDef(macro.c_str());
  }
  // Setup the language parsing options for C++
  clang::LangOptions &lang_options = *compiler_invocation->getLangOpts();
  // m_CompilerInvocation->setLangDefaults(lang_options, clang::IK_CXX,
  // clang::LangStandard::lang_cxx11);
  lang_options.CPlusPlus = 1;
  lang_options.Bool = 1;
  lang_options.RTTI = 0;

#if defined(CLCPP_USING_MSVC)
  lang_options.MicrosoftExt = 1;
  lang_options.MicrosoftMode = 1;
  lang_options.MSBitfields = 1;

  //
  // This is MSVC specific to get STL compiling with clang. MSVC doesn't do
  // semantic analysis of templates until instantiation, whereas clang will try
  // to resolve non type-based function calls. In MSVC STL land, this causes
  // hundreds of errors referencing
  // '_invalid_parameter_noinfo'.
  //
  // The problem in a nutshell:
  //
  //    template <typename TYPE> void A()
  //    {
  //       // Causes an error in clang because B() is not defined yet, MSVC is
  //       fine B();
  //    }
  //    void B() { }
  //
  lang_options.DelayedTemplateParsing = 1;
#endif // CLCPP_USING_MSVC

  clang::HeaderSearchOptions &header_search_options =
      compiler_invocation->getHeaderSearchOpts();
  for (auto &include : options.include_dirs) {
    header_search_options.AddPath(include.c_str(), clang::frontend::Angled,
                                  false, false);
  }
  for (auto &include : options.system_include_dirs) {
    header_search_options.AddPath(include.c_str(), clang::frontend::System,
                                  false, false);
  }
  llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts =
      new clang::DiagnosticOptions();
  // Setup diagnostics output; MSVC line-clicking and suppress warnings from
  // system headers
#if defined(CLCPP_USING_MSVC)
  diagnostic_options_.Format = diagnostic_options_.Msvc;
#else
  // m_DiagnosticOptions.Format = m_DiagnosticOptions.Clang;
#endif // CLCPP_USING_MSVC
  clang::TextDiagnosticPrinter *client =
      new clang::TextDiagnosticPrinter(output_stream_, &*DiagOpts);
  compiler_.createDiagnostics(client, true);
  compiler_.getDiagnostics().setSuppressSystemWarnings(true);
  // Setup target options - ensure record layout calculations use the MSVC C++
  // ABI
  clang::TargetOptions &target_options = compiler_invocation->getTargetOpts();
  target_options.Triple = options.target_triple;
  auto new_target_opts = std::make_shared<clang::TargetOptions>();
  *new_target_opts = target_options;
#if defined(CLCPP_USING_MSVC)
  target_options.CXXABI = "microsoft";
#else
  // target_options.CXXABI = "itanium";
#endif // CLCPP_USING_MSVC
  target_info_.reset(clang::TargetInfo::CreateTargetInfo(
      compiler_.getDiagnostics(), new_target_opts));
  compiler_.setTarget(target_info_.get());
  // Set the invokation on the instance
  compiler_.createFileManager();
  compiler_.createSourceManager(compiler_.getFileManager());
  compiler_.setInvocation(compiler_invocation);
}

HeaderParser::~HeaderParser() {}

bool HeaderParser::ParseAST(const char *filename) {

  compiler_.createPreprocessor(clang::TranslationUnitKind::TU_Complete);
  compiler_.createASTContext();

  // Initialize builtins
  if (compiler_.hasPreprocessor()) {
    clang::Preprocessor &preprocessor = compiler_.getPreprocessor();
    preprocessor.getBuiltinInfo().initializeBuiltins(
        preprocessor.getIdentifierTable(), preprocessor.getLangOpts());
  }

  const auto file = compiler_.getFileManager().getFile(filename);
  if (file) {
    auto fileID = compiler_.getSourceManager().getOrCreateFileID(
#if LLVM_VERSION_MAJOR >= 11
        file.get(),
#else
        file,
#endif
        clang::SrcMgr::C_User);
    compiler_.getSourceManager().setMainFileID(fileID);
  }
  // Parse the AST
  //EmptyASTConsumer ast_consumer;
  clang::DiagnosticConsumer *client = compiler_.getDiagnostics().getClient();
  client->BeginSourceFile(compiler_.getLangOpts(),
                          &compiler_.getPreprocessor());
  clang::ParseAST(compiler_.getPreprocessor(), this,
                  compiler_.getASTContext());
  client->EndSourceFile();
  return client->getNumErrors() == 0;
}

void HeaderParser::print_ast() {
  auto tu_decl = GetASTContext().getTranslationUnitDecl();
  for (clang::DeclContext::decl_iterator i = tu_decl->decls_begin();
       i != tu_decl->decls_end(); ++i) {
    clang::NamedDecl *named_decl = llvm::dyn_cast<clang::NamedDecl>(*i);
    if (named_decl == nullptr)
      continue;
    // Filter out unsupported decls at the global namespace level
    clang::Decl::Kind kind = named_decl->getKind();
    switch (kind) {
    case (clang::Decl::Namespace):
    case (clang::Decl::CXXRecord):
    case (clang::Decl::Function):
    case (clang::Decl::Enum):
      // AddDecl(named_decl, "", 0);
      break;
    default:
      break;
    }
  }
}
