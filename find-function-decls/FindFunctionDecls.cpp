#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;

std::vector<std::string> vec_class;
std::vector<std::string> vec_struct;
std::vector<std::string> vec_function;
std::vector<std::string> vec_enum;
std::vector<std::string> vec_namespace;
class FindNamedFunctionVisitor : public RecursiveASTVisitor <FindNamedFunctionVisitor>{
  public:
  explicit FindNamedFunctionVisitor(ASTContext * Context):context(Context){}

  bool VisitCXXRecordDecl(CXXRecordDecl * Declaration){
    //Declaration->dumpDeclContext();

    //std::cout<<std::endl<<std::endl;
    //std::cout<<"get Decl kind: "<<Declaration->getDeclKind();
    
    if(Declaration->isClass()){
      vec_class.push_back(Declaration->getQualifiedNameAsString());
    }
    
    else if(Declaration->isStruct()){
      vec_struct.push_back(Declaration->getQualifiedNameAsString());
    }
    
    return true;
  }

  bool VisitNamespaceDecl(NamespaceDecl *D){
    vec_namespace.push_back(D->getQualifiedNameAsString());
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl  *FD){
    std::cout<<"------------------------------"<<std::endl;
    std::cout<<"function name:"<<FD->getNameAsString()<<std::endl;
    std::cout<<"number of param:"<<FD->getNumParams()<<std::endl;
    std::cout<<"getReturnType:"
              <<FD->getReturnType().getAsString()<<std::endl;
    for(BlockDecl::param_iterator PI= FD->param_begin();
        PI != FD->param_end();PI++){
      std::cout<<"param:"<<(*PI)->getQualifiedNameAsString()
                <<", type is "<<(*PI)->getType().getAsString()<<std::endl;
    }
    std::cout<<std::endl;
    return true;
  }

  bool VisitEnumDecl(EnumDecl *ED){
    vec_enum.push_back(ED->getQualifiedNameAsString());
    return true;
  }

private:
  ASTContext *context;
};

class FindNamedFunctionConsumer : public clang::ASTConsumer{
public:
  explicit FindNamedFunctionConsumer(ASTContext* Context) : Visitor(Context){}

  virtual void HandleTranslationUnit(clang::ASTContext & Context){
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindNamedFunctionVisitor Visitor;
};

class FindNamedFunctionActiion:public clang::ASTFrontendAction{
  public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance & Compiler,llvm::StringRef InFile){
      return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedFunctionConsumer(&Compiler.getASTContext())
      );
    }
};

void print_vec(std::vector<std::string> vec,std::string kind){
  std::cout<<"-------------"<<kind<<"-------------"<<std::endl;
  for(std::string & name : vec){
    std::cout<<name<<std::endl;
  }
}

int main(int argc, char **argv) {
  if(argc <= 1){
    std::cout<<"need file name"<<std::endl;
  }
  std::string file_path(argv[1]);
  std::ifstream ifs(file_path);
  std::string content;
  content.assign( (std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>())
                );
  //std::cout<<content;
  clang::tooling::runToolOnCode(new FindNamedFunctionActiion,content);
  print_vec(vec_class,"class");
  print_vec(vec_struct,"struct");
  //print_vec(vec_function,"function");
  print_vec(vec_enum,"enum");
}
