#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum FastNoiseLiteTypes_NoiseType
{
    NoiseType_OpenSimplex2,
    NoiseType_OpenSimplex2S,
    NoiseType_Cellular,
    NoiseType_Perlin,
    NoiseType_ValueCubic,
    NoiseType_Value
};

UENUM(BlueprintType)
enum FastNoiseLiteTypes_RotationType3D
{
    RotationType3D_None,
    RotationType3D_ImproveXYPlanes,
    RotationType3D_ImproveXZPlanes
};

UENUM(BlueprintType)
enum FastNoiseLiteTypes_FractalType
{
    FractalType_None,
    FractalType_FBm,
    FractalType_Ridged,
    FractalType_PingPong,
    FractalType_DomainWarpProgressive,
    FractalType_DomainWarpIndependent
};

UENUM(BlueprintType)
enum FastNoiseLiteTypes_CellularDistanceFunction
{
    CellularDistanceFunction_Euclidean,
    CellularDistanceFunction_EuclideanSq,
    CellularDistanceFunction_Manhattan,
    CellularDistanceFunction_Hybrid
};

UENUM(BlueprintType)
enum FastNoiseLiteTypes_CellularReturnType
{
    CellularReturnType_CellValue,
    CellularReturnType_Distance,
    CellularReturnType_Distance2,
    CellularReturnType_Distance2Add,
    CellularReturnType_Distance2Sub,
    CellularReturnType_Distance2Mul,
    CellularReturnType_Distance2Div
};

UENUM(BlueprintType)
enum FastNoiseLiteTypes_DomainWarpType
{
    DomainWarpType_OpenSimplex2,
    DomainWarpType_OpenSimplex2Reduced,
    DomainWarpType_BasicGrid
};
