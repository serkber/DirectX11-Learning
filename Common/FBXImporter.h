#pragma once
#include <fstream>
#include <xstring>

#include "Typedefs.h"

class FBXImporter
{    
public:    
    struct FBXModel
    {
        float3* vertices;
        int* indices;
        int vertexCount;
        int indexCount;
    };
    
    void EvaluateWord(const std::string& text);
    FBXModel GetModel(std::string file);
    ~FBXImporter();

private:
    std::ifstream m_file;

    uint32_t m_numVertices;
    uint32_t m_numIndices;

    float3* m_vertices;
    int* m_indices;
};