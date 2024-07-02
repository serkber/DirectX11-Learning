#include "FBXImporter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Typedefs.h"
#include "Utils.h"

void FBXImporter::EvaluateWord(const std::string& text)
{    
    if(text._Equal("Vertices:"))
    {
        std::string numVert;
        m_file >> numVert;
        numVert.erase(0, 1);
        m_numVertices = std::stoi(numVert) / 3;

        m_vertices = new float3[m_numVertices];

        std::string vertsRaw;
        while (!vertsRaw._Equal("a:"))
        {
            m_file >> vertsRaw;
        }

        m_file >> vertsRaw;
        const auto verts = Utils::Split(vertsRaw, ',');

        for (int i = 0, j = 0; i < m_numVertices; ++i, j += 3)
        {
            m_vertices[i].x = std::stof(verts[j]);
            m_vertices[i].y = std::stof(verts[j + 1]);
            m_vertices[i].z = std::stof(verts[j + 2]);
        }
    }

    if (text._Equal("PolygonVertexIndex:"))
    {
        std::string numIndices;
        m_file >> numIndices;
        numIndices.erase(0, 1);
        m_numIndices = std::stoi(numIndices);

        m_indices = new int[m_numIndices];
        
        std::string indicesRaw;
        while (!indicesRaw._Equal("a:"))
        {
            m_file >> indicesRaw;
        }
        
        m_file >> indicesRaw;        
        const auto inds = Utils::Split(indicesRaw, ',');

        for (int i = 0; i < inds.size(); ++i)
        {
            auto index = std::stoi(inds[i]);

            if(index < 0)
            {
                index = abs(index) - 1;
            }
            
            m_indices[i] = index;
        }

        for (int i = 0; i < m_numIndices; ++i)
        {
            std::cout << m_indices[i] << std::endl;
        }
    }
}

FBXImporter::FBXModel FBXImporter::GetModel(std::string file)
{
    delete m_vertices;
    delete m_indices;
    
    m_file.open(file);
    
    FBXModel model;

    if(m_file)
    {
        
        std::string text;
        while (m_file >> text)
        {
            EvaluateWord(text);
        }
        
        m_file.close();
        model.vertices = m_vertices;
        model.indices = m_indices;
        model.indexCount = m_numIndices;
        model.vertexCount = m_numVertices;

    }
    
    return model;
}

FBXImporter::~FBXImporter()
{
    delete m_vertices;
    delete m_indices;
}