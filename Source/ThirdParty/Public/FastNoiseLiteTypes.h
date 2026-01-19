#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "FastNoiseLiteTypes.generated.h"

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

USTRUCT(BlueprintType)
struct FNoiseSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Frequency = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_NoiseType> NoiseType = NoiseType_Perlin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_RotationType3D> RotationType3D = RotationType3D_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_FractalType> FractalType = FractalType_FBm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int FractalOctaves = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FractalLacunarity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FractalGain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FractalWeightedStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FractalPingPongStrength = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_CellularDistanceFunction> CellularDistanceFunction = CellularDistanceFunction_EuclideanSq;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_CellularReturnType> CellularReturnType = CellularReturnType_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellularJitter = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FastNoiseLiteTypes_DomainWarpType> DomainWarpType = DomainWarpType_OpenSimplex2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DomainWarpAmp = 1.0f;
};
