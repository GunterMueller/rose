#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include <string>
#include "CastTransformer.h"
#include "TFTransformation.h"

class TFTypeTransformer {
 public:
  typedef std::tuple<SgType*,SgFunctionDefinition*,std::string,bool,SgType*> VarTypeVarNameTuple;
  typedef std::list<VarTypeVarNameTuple> VarTypeVarNameTupleList;
  void addToTransformationList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varnames);
  void addToTransformationList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, SgType* fromType);
  // searches for variable in the given subtree 'root'
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type);
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type, bool base, SgType* fromType);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type, bool base);
  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNameTupleList& list);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);
  int getTotalNumChanges();
  int getTotalTypeNameChanges();
  void generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);

 private:
  CastTransformer _castTransformer;
  static bool _traceFlag;
  int _totalNumChanges=0;
  float _totalTypeNameChanges=0;
};

#endif
