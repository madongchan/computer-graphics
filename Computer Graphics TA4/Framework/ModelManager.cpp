////////////////////////////////////////////////////////////////////////////////
// Filename: ModelManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ModelManager.h"

ModelManager::ModelManager() {
    m_Models.clear();
    m_ModelPaths.clear();
    m_TexturePaths.clear();
}

ModelManager::~ModelManager() {
}

bool ModelManager::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;

    // 모델 경로들 초기화
    InitializeModelPaths();

    // 모델 벡터 크기 설정
    m_Models.resize(static_cast<int>(ModelType::MODEL_COUNT));

    // 각 모델 로드
    for (int i = 0; i < static_cast<int>(ModelType::MODEL_COUNT); i++) {
        m_Models[i] = new ModelClass;
        if (!m_Models[i]) {
            return false;
        }

        result = m_Models[i]->Initialize(device, m_ModelPaths[i].c_str(), m_TexturePaths[i].c_str());
        if (!result) {
            MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
            return false;
        }
    }

    return true;
}

void ModelManager::Shutdown() {
    for (auto& model : m_Models) {
        if (model) {
            model->Shutdown();
            delete model;
            model = nullptr;
        }
    }
    m_Models.clear();
}

ModelClass* ModelManager::GetModel(ModelType type) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < m_Models.size()) {
        return m_Models[index];
    }
    return nullptr;
}

ModelClass* ModelManager::GetModel(int index) {
    if (index >= 0 && index < m_Models.size()) {
        return m_Models[index];
    }
    return nullptr;
}

int ModelManager::GetModelCount() const {
    return static_cast<int>(ModelType::MODEL_COUNT);
}

void ModelManager::InitializeModelPaths() {
    // 모델 파일 경로들
    m_ModelPaths = {
        L"./data/SM_Env_Road_Cross_01.obj",      // ROAD
        L"./data/SM_Generic_Tree_02.obj",      // TREE
        L"./data/Character_MilitaryMale_01.obj",    // PERSON
        L"./data/SM_Bld_Tent_02.obj",  // BUILDING
        L"./data/SM_Veh_Car_Clean_01.obj",       // CAR
        L"./data/SM_Veh_Bike_01.obj",      // BIKE
        L"./data/SM_Prop_Range_Wall_Window_01.obj",      // WALL
        L"./data/SM_Prop_TireStack_01.obj",      // TireStack
        L"./data/SM_Prop_Barricade_01.obj",     // FENCE
        L"./data/SM_Prop_Spotlight_01.obj"       // SIGN
    };

    // 텍스처 파일 경로들
    m_TexturePaths = {
        L"./data/PolygonBattleRoyale_Road_01.dds",      // ROAD
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",      // TREE
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",    // PERSON
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",  // BUILDING
        L"./data/PolygonBattleRoyale_Vehicles_01.dds",       // CAR
        L"./data/PolygonBattleRoyale_Vehicles_01.dds",      // BIKE
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",      // WALL
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",      // TireStack
        L"./data/PolygonBattleRoyale_Texture_01_A.dds",     // FENCE
        L"./data/PolygonBattleRoyale_Texture_01_A.dds"       // SIGN
    };
}
