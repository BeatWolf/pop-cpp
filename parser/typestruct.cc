#include "popc_intface.h"

#include "type.h"

// TODO LW: Why do we handle class and struct differently ??? This file should not exist

TypeStruct::TypeStruct(char* name) : DataType(name) {
}

TypeStruct::~TypeStruct() {
    for (auto& attr : attributes) {
        free(attr.second);
    }
}

void TypeStruct::Add(DataType* elem, char* ename) {
    assert(ename != nullptr);
    ename = popc_strdup(ename);
    attributes.emplace_back(elem, ename);
}

int TypeStruct::CanMarshal() {
    if (IsMarked()) {
        Mark(false);
        return 0;
    }
    Mark(true);

    for (auto& attr : attributes) {
        if (attr.first->CanMarshal() != 1) {
            Mark(false);
            return 0;
        }
    }

    Mark(false);
    return 1;
}

void TypeStruct::Marshal(char* varname, char* bufname, char* /*sizehelper*/, std::string& output) {
    char paramname[256], tmpcode[1024];

    if (!FindVarName(varname, paramname)) {
        strcpy(paramname, "unknown");
    }
    sprintf(tmpcode, "%s.Push(\"%s\",\"%s\", 1);\n", bufname, paramname, GetName());
    output += tmpcode;

    for (auto& attr : attributes) {
        sprintf(tmpcode, "%s.%s", varname, attr.second);
        attr.first->Marshal(tmpcode, bufname, nullptr, output);
    }

    sprintf(tmpcode, "%s.Pop();\n", bufname);
    output += tmpcode;
}

void TypeStruct::DeMarshal(char* varname, char* bufname, char* /*sizehelper*/, std::string& output) {
    char paramname[256], tmpcode[1024];

    if (!FindVarName(varname, paramname)) {
        strcpy(paramname, "unknown");
    }
    sprintf(tmpcode, "%s.Push(\"%s\",\"%s\",1);\n", bufname, paramname, GetName());
    output += tmpcode;

    for (auto& attr : attributes) {
        sprintf(tmpcode, "%s.%s", varname, attr.second);
        attr.first->DeMarshal(tmpcode, bufname, nullptr, output);
    }

    sprintf(tmpcode, "%s.Pop();\n", bufname);
    output += tmpcode;
}

bool TypeStruct::IsPrototype() {
    return attributes.empty();
}
