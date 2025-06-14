////////////////////////////////////////////////////////////////////////////////
// Filename: ModelManager.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <d3d11.h>
#include <vector>
#include <string>
#include "modelclass.h"

enum class ModelType {
    ROAD = 0,
    TREE = 1,
    PERSON = 2,
    BUILDING = 3,
    CAR = 4,
    BIKE = 5,
    WALL = 6,
    TIRESTACK = 7,
    FENCE = 8,
    SIGN = 9,
    MODEL_COUNT = 10
};

class ModelManager {
private:
    std::vector<ModelClass*> m_Models;
    std::vector<std::wstring> m_ModelPaths;
    std::vector<std::wstring> m_TexturePaths;

public:
    ModelManager();
    ~ModelManager();

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    ModelClass* GetModel(ModelType type);
    ModelClass* GetModel(int index);
    int GetModelCount() const;

private:
    void InitializeModelPaths();
};

#endif
