/**
 * File : parser.h
 * Author : Tuan Anh Nguyen
 * Description : parser header declaration
 * Creation date : -
 *
 * Modifications :
 * Authors      Date            Comment
 * clementval   March 2012  Add Enum support
 * clementval   April 2012  Add namespace support to the parser
 */

#ifndef PARSER_H
#define PARSER_H

#include "parser_common.h"
#include "type.h"
#include <string>

#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
typedef long YYSTYPE;   // We force the declaration as long to avoid compilation problems in 64bits
#define yystype YYSTYPE /* obsolescent; will be withdrawn */
#define YYSTYPE_IS_DECLARED 1
#define YYSTYPE_IS_TRIVIAL 1
#endif

#define PARAM_IN 1
#define PARAM_OUT 2
#define SYNC 4
#define ASYNC 8
#define CONC 16
#define USERPROC 32
#define PARAMSIZE 64
#define OBJ_POSTFIX "_popcobject"
#define TMP_CODE_PREFIX "_paroc_"

// Invocation semantics codes...
#define INVOKE_SYNC 1
//#define INVOKE_ASYNC 2
#define INVOKE_CONSTRUCTOR 4
#define INVOKE_CONC 8
#define INVOKE_MUTEX 16

// bit FLAGS values customized marshalling/demarshalling procedure....should be the same as defined in marshal.h
#define FLAG_MARSHAL 2
#define FLAG_INPUT 1

#define TYPE_OTHERCODE 1
#define TYPE_PACKOBJECT 2
#define TYPE_CLASS 3

#define TYPE_ATTRIBUTE 4
#define TYPE_METHOD 5
#define TYPE_DIRECTIVE 6

#define METHOD_NORMAL 0
#define METHOD_CONSTRUCTOR 1
#define METHOD_DESTRUCTOR 2

enum InvokeType { autoselect = 0, invokesync = 1, invokeasync = 2 };
enum AccessType { PUBLIC = 0, PROTECTED = 1, PRIVATE = 2 };

class Param;
class Method;
class Class;
class BaseClass;
class ClassMember;
class CodeData;
class CodeFile;
class PackObject;

typedef std::vector<Param*> CArrayParam;
typedef std::vector<Method*> CArrayMethod;
typedef std::vector<Class*> CArrayClass;
typedef std::vector<BaseClass*> CArrayBaseClass;
typedef std::vector<ClassMember*> CArrayClassMember;
typedef std::vector<CodeData*> CArrayCodeData;
typedef std::vector<CodeFile*> CArrayCodeFile;
typedef std::vector<DataType*> CArrayDataType;

/**
 * @class CodeData
 * @brief A C++ class representing code to generate, used by the parser.
 *
 *
 */
class CodeData {
public:
    CodeData(CodeFile* file);
    virtual ~CodeData();

    virtual int Type() {
        return -1;
    };

    virtual void GenerateCode(std::string& output);
    CodeFile* GetCodeFile() {
        return codeFile;
    }

protected:
    CodeFile* codeFile;
};

/**
 * @class OtherCode
 * @brief A C++ class representing other code lines, used by the parser.
 *
 *
 */
class OtherCode : public CodeData {
public:
    OtherCode(CodeFile* file);

    virtual int Type() {
        return TYPE_OTHERCODE;
    };

    virtual void GenerateCode(std::string& output);

    void AddCode(const char* newcode);
    void AddCode(const char*, int n);
    void AddCode(const std::string& newcode);

protected:
    std::string code;
};

/**
 * @class PackObject
 * @brief A C++ class representing code for the object packing, used by the parser.
 *
 *
 */
class PackObject : public CodeData {
public:
    PackObject(CodeFile* file);
    virtual int Type() {
        return TYPE_PACKOBJECT;
    };
    virtual void GenerateCode(std::string& output);
    void AddObject(const std::string& objname);
    int GetNumObject();

    void SetStartLine(int l);
    void SetEndLine(int l);

protected:
    std::vector<std::string> objects;
    int startline, endline;
};

/**
 * @class CodeFile
 * @brief A C++ class representing a code file, used by the parser.
 *
 *
 */
class CodeFile {
public:
    CodeFile(const std::string& fname);
    ~CodeFile();
    void AddCodeData(CodeData* code);
    void EmptyCodeData();
    void GenerateCode(std::string& output, bool client = true, bool broker = true /*, bool isPOPCPPCompilation=false*/);
    CArrayCodeData* GetCodes();
    bool HasClass();

    const std::string& GetFileName() const {
        return filename;
    }
    void SetFileName(const std::string& name) {
        filename = name;
    }

    const std::string& GetOutputName() const {
        return outfile;
    }
    void SetOutputName(const std::string& name) {
        outfile = name;
    }

    Class* FindClass(char* clname);
    /**
         * @brief Returns the class list
         * @param classlist Output
     */
    void FindAllClass(CArrayClass& classlist);
    /**
     * @brief Finds out if a class is in bases and adds it if not found.
     * @param t Class to search
     * @param bases Base
     * @param virtualBaseOnly
     */
    void FindAllBaseClass(Class& t, CArrayClass& bases, bool virtualBaseOnly);

    DataType* FindDataType(const char* name);
    void AddDataType(DataType* type);

    void SetAsCoreCompilation();
    bool IsCoreCompilation();

    void EnableAsyncAllocation();
    bool IsAsyncAllocationEnabled();

    static bool SameFile(const char* file1, const char* file2);

protected:
    std::string filename;
    std::string outfile;
    bool isCoreCompilation;
    bool asyncAllocationEnabled;
    CArrayCodeData codes;
    CArrayClass classlist;

    CArrayDataType datatypes;
    CArrayDataType temptypes;
};

/**
 * @class Param
 * @brief A C++ class representing a parameter, used by the parser.
 *
 * It seems that this class is also used to represent a class attribute.
 */
class Param {
public:
    Param(DataType* ptype);
    Param();
    virtual ~Param();
    char* Parse(char* str);
    bool Match(Param* p);

    bool IsPointer();
    bool IsRef();
    bool IsConst();
    bool InParam();
    bool IsArray();
    bool IsVoid();
    bool OutParam();

    void SetType(DataType* type);

    DataType* GetType();

    char* GetName();
    char* GetDefaultValue();

    int GetAttr();

    bool CanMarshal();

    bool DeclareParam(char* output, bool header);
    bool DeclareVariable(char* output, bool& reformat, bool allocmem);
    bool DeclareVariable(char* output);

    virtual bool Marshal(char* bufname, bool reformat, bool inf_side, std::string& output);
    virtual bool UnMarshal(char* bufname, bool reformat, bool alloc_mem, bool inf_side, std::string& output);
    virtual bool UserProc(char* code);

    bool isConst;
    bool isRef;
    bool isArray;
    bool isVoid;
    char name[64];
    DataType* mytype;
    //  char *cardinal;
    char* paramSize;
    char* marshalProc;
    char* defaultVal;

    bool isInput;
    bool isOutput;

    int attr;
};

/**
 * @class ObjDesc
 * @brief (to be written)
 *
 *
 */
class ObjDesc {
public:
    ObjDesc();
    ~ObjDesc();

    void Generate(char* code);
    void SetCode(char* code);

protected:
    char* odstr;
};

/**
 * @class ClassMember
 * @brief A C++ class representing a member of a class, used by the parser.
 *
 *
 */
class ClassMember {
public:
    ClassMember(Class* cl, AccessType myaccess);
    virtual ~ClassMember();

    virtual int Type() {
        return -1;
    };

    virtual void GenerateClient(std::string& output);
    virtual void GenerateHeader(std::string& output, bool interface);
    virtual void generate_header_pog(std::string& output, bool interface);

    void SetLineInfo(int linenum);

    AccessType GetMyAccess();
    Class* GetClass();

protected:
    AccessType accessType;
    Class* myclass;
    int line;
};

/**
 * @class Attribute
 * @brief  A C++ class representing a class attribute, used by the parser.
 *
 *
 */
class Attribute : public ClassMember {
public:
    Attribute(Class* cl, AccessType myaccess);
    virtual int Type() {
        return TYPE_ATTRIBUTE;
    };

    virtual void GenerateHeader(std::string& output, bool interface);

    Param* NewAttribute();

protected:
    CArrayParam attributes;
};

/**
 * @class Enumeration
 * @brief This class is holding enum type information
 */
class Enumeration : public ClassMember {
public:
    Enumeration(Class* cl, AccessType myaccess);
    ~Enumeration();
    virtual void GenerateHeader(std::string& output, bool interface);

    void setName(std::string value);
    void setArgs(std::string value);

private:
    std::string name;
    std::string args;
};

/**
 * @class Structure
 * @brief This class is holding struct type information
 */
class Structure : public ClassMember {
public:
    Structure(Class* cl, AccessType myaccess);
    ~Structure();
    virtual void GenerateHeader(std::string& output, bool interface);

    void setName(std::string value);
    void setObjects(std::string value);
    void setInnerDecl(std::string value);

private:
    std::string name;
    std::string objects;
    std::string innerdecl;
};

/**
 * @class TypeDefinition
 * @Brief This class is holding typedef information
 */
class TypeDefinition : public ClassMember {
public:
    TypeDefinition(Class* cl, AccessType myaccess);
    ~TypeDefinition();
    virtual void GenerateHeader(std::string& output, bool interface);

    void setName(std::string name);
    void setBaseType(std::string basetype);
    void setAsPtr();
    bool isPtr();
    void setAsArray();
    bool isArray();

private:
    bool ptr;
    bool array;
    std::string name;
    std::string basetype;
};

/**
 * @class Directive
 * @brief (to be written)
 *
 */
class Directive : public ClassMember {
public:
    Directive(Class* cl, char* directive);
    ~Directive();

    virtual int Type() {
        return TYPE_DIRECTIVE;
    };
    virtual void GenerateHeader(std::string& output, bool interface);

private:
    char* code;
};

/**
 * @class Method
 * @brief  A C++ class representing a method, used by the parser.
 *
 */
class Method : public ClassMember {
public:
    Method(Class* cl, AccessType myaccess);
    ~Method();

    static const int POPC_METHOD_NON_COLLECTIVE_SIGNAL_ID;
    static const int POPC_METHOD_NON_COLLECTIVE_SIGNAL_INVOKE_MODE;
    static const char* POPC_METHOD_NON_COLLECTIVE_SIGNAL_NAME;

    virtual int Type() {
        return TYPE_METHOD;
    };

    int CheckMarshal();

    virtual void GenerateClient(std::string& output);
    virtual void GenerateHeader(std::string& output, bool interface);
    virtual void generate_header_pog(std::string& output, bool interface);
    virtual void GenerateBrokerHeader(std::string& output);
    virtual void generate_broker_header_pog(std::string& output);
    virtual void GenerateBroker(std::string& output);

    virtual int MethodType() {
        return METHOD_NORMAL;
    };

    bool operator==(Method& other);

    char* Parse(char* str);

    Param* AddNewParam();

    bool hasInput();
    bool hasOutput();

    bool isVirtual;
    bool isPureVirtual;
    bool isGlobalConst;  // add by david

    enum CollectiveType {
        POPC_COLLECTIVE_BROADCAST,
        POPC_COLLECTIVE_GATHER,
        POPC_COLLECTIVE_SCATTER,
        POPC_COLLECTIVE_REDUCE
    };
    void set_collective(CollectiveType type);
    bool is_collective();

    bool isConcurrent;
    bool isMutex;
    bool isHidden;

    InvokeType invoketype;

    char name[64];
    int id;
    CArrayParam params;
    Param returnparam;

protected:
    virtual void GenerateReturn(std::string& output, bool header, bool interface);
    virtual void GenerateReturn(std::string& output, bool header);
    virtual void GeneratePostfix(std::string& output, bool header);
    virtual void GenerateName(std::string& output, bool header);
    virtual void GenerateArguments(std::string& output, bool header);

    virtual void GenerateClientPrefixBody(std::string& output);

private:
    bool _is_collective;
    bool _is_broadcast;
    bool _is_scatter;
    bool _is_gather;
    bool _is_reduce;
};
/**
 * @class Constructor
 * @brief  A C++ class representing constructor method, used by the parser.
 *
 *
 */
class Constructor : public Method {
public:
    Constructor(Class* cl, AccessType myaccess);
    virtual int MethodType() {
        return METHOD_CONSTRUCTOR;
    };

    virtual void GenerateHeader(std::string& output, bool interface);
    virtual void generate_header_pog(std::string& output, bool interface);

    bool isDefault();

    ObjDesc& GetOD();

    void set_id(int value);
    int get_id();

protected:
    virtual void GenerateReturn(std::string& output, bool header, bool interface) {
        Method::GenerateReturn(output, header, interface);
    };
    virtual void GenerateReturn(std::string& output, bool header);
    virtual void GeneratePostfix(std::string& output, bool header);
    virtual void GenerateClientPrefixBody(std::string& output);

    ObjDesc od;  // Only used for constructor method

private:
    int identifier;
};
/**
 * @class Destructor
 * @brief  A C++ class representing destructor method, used by the parser.
 *
 */
class Destructor : public Method {
public:
    Destructor(Class* cl, AccessType myaccess);

    virtual int MethodType() {
        return METHOD_DESTRUCTOR;
    };

    virtual void GenerateClient(std::string& output);

protected:
    virtual void GenerateReturn(std::string& output, bool header, bool interface) {
        Method::GenerateReturn(output, header, interface);
    };
    virtual void GenerateReturn(std::string& output, bool header);
};

/**
 * @class BaseClass
 * @brief A C++ class representing a parent class, used by the parser
 *
 *
 */
class BaseClass {
public:
    BaseClass(Class* name, AccessType basemode, bool onlyVirtual);
    bool baseVirtual;
    AccessType type;
    Class* base;
};

/**
 * @class Class
 * @brief A C++ class representing a parallel class, used by the parser.
 *
 *
 */
class Class : public CodeData, public DataType {
public:
    Class(char* clname, CodeFile* file);
    ~Class();

    virtual bool IsParClass();
    virtual int CanMarshal();
    virtual void Marshal(char* varname, char* bufname, char* sizehelper, std::string& output);
    virtual void DeMarshal(char* varname, char* bufname, char* sizehelper, std::string& output);

    virtual int Type() {
        return TYPE_CLASS;
    };
    virtual void GenerateCode(std::string& output /*, bool isPOPCPPCompilation*/);

    void SetFileInfo(char* file);
    char* GetFileInfo();
    void SetStartLine(int num);
    void SetEndLine(int num);

    char* Parse(char* str);
    void AddMember(ClassMember* t);

    void SetClassID(char* id);
    void SetPureVirtual(bool val);
    bool IsPureVirtual();
    void SetBasePureVirtual(bool val);
    bool IsBasePureVirtual();

    static unsigned IDFromString(char* str);

    bool hasMethod(Method& x);
    bool methodInBaseClass(Method& x);
    bool findPureVirtual(CArrayMethod& lst);
    // void findPureVirtual(CArrayMethod &lst);

    bool GenerateClient(std::string& code);
    bool GenerateHeader(std::string& code, bool interface);
    bool GenerateBrokerHeader(std::string& code);
    bool GenerateBroker(std::string& code);

    bool generate_header_pog(std::string& code, bool interface);
    bool generate_broker_header_pog(std::string& code);

    char classid[64];

    CArrayBaseClass baseClass;
    CArrayClassMember memberList;

    void SetNamespace(char* value);
    std::string GetNamespace();

    void SetAsCoreCompilation();
    bool IsCoreCompilation();
    void EnableWarning();
    bool IsWarningEnable();
    void set_as_collective();
    bool is_collective();

    void EnableAsyncAllocation();
    bool IsAsyncAllocationEnabled();

    static const char* POG_BASE_INTERFACE_NAME;
    static const char* POG_BASE_OBJECT_NAME;
    static const char* POG_OBJECT_POSTFIX;
    static const char* POG_BASE_BROKER_NAME;
    static const char* POG_BROKER_POSTFIX;

public:
    static char interface_base[1024];
    static char broker_base[1024];
    static char object_base[1024];

protected:
    Constructor constructor;

    char* myFile;
    bool initDone;
    int endid;
    int startline, endline;

    bool noConstructor;
    bool pureVirtual;
    bool basePureVirtual;
    bool isCoreCompilation;
    bool hasWarningEnable;
    bool asyncAllocationEnabled;
    bool _is_collective;

    char* my_interface_base;
    char* my_object_base;
    char* my_broker_base;
    std::string strnamespace;

private:
    int constrcutor_id;
};

// Implementation of template

#endif
