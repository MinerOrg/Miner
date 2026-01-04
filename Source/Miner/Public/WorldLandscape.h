// Copyright Schuyler Zheng. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "FastNoiseLiteTypes.h"
#include "WorldLandscape.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLandscape, Log, All);

DECLARE_MULTICAST_DELEGATE(FTerrainDataGeneratedDelegate);

/**
 * AWorldLandscape is an Actor that generates a dynamic landscape mesh based on a seed
 */

class FastNoiseLite;
class FWorldGenerationRunnable;

UCLASS(ConversionRoot, ComponentWrapperClass, ClassGroup = DynamicMesh, meta = (ChildCanTick), MinimalAPI)
class AWorldLandscape : public AActor
{
	GENERATED_BODY()

public:
	AWorldLandscape();

	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMeshComponent* GetDynamicMeshComponent() const { return DynamicMeshComponent; }

	//
	// Mesh Pool support. Meshes can be locally allocated from the Mesh Pool
	// in Blueprints, and then released back to the Pool and re-used. This
	// avoids generating temporary UDynamicMesh instances that need to be
	// garbage-collected. See UDynamicMeshPool for more details.
	//

	/** Control whether the DynamicMeshPool will be created when requested via GetComputeMeshPool() */
	UPROPERTY(Category = "DynamicMeshActor|Advanced", EditAnywhere, BlueprintReadWrite)
	bool bEnableComputeMeshPool = true;

	/** Access the compute mesh pool */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMeshPool* GetComputeMeshPool();

	/** Request a compute mesh from the Pool, which will return a previously-allocated mesh or add and return a new one. If the Pool is disabled, a new UDynamicMesh will be allocated and returned. */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMesh* AllocateComputeMesh();

	/** Release a compute mesh back to the Pool */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	bool ReleaseComputeMesh(UDynamicMesh* Mesh);

	/** Release all compute meshes that the Pool has allocated */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	void ReleaseAllComputeMeshes();

	/** Release all compute meshes that the Pool has allocated, and then release them from the Pool, so that they will be garbage-collected */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	void FreeAllComputeMeshes();

	/** The function for the world generation thread to run, and give the generated points */
	UFUNCTION(BlueprintCallable, Category = "Landscape Generation")
	void GenerateVertexLocations();

	FTerrainDataGeneratedDelegate ApplyTerrainDataDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupNoise();
	void GenerateTerrain();

	// Mesh generation steps
	void ApplyGeneratedMeshData(UE::Geometry::FDynamicMesh3& Mesh);
	void PostGeneration(UE::Geometry::FDynamicMesh3& Mesh);

	UPROPERTY(Category = DynamicMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	TObjectPtr<class UDynamicMeshComponent> DynamicMeshComponent;

	UPROPERTY(Transient)
	TObjectPtr<UDynamicMesh> DynamicMesh;

	/** The internal Mesh Pool, for use in DynamicMeshActor BPs. Use GetComputeMeshPool() to access this, as it will only be created on-demand if bEnableComputeMeshPool = true */
	UPROPERTY(Transient)
	TObjectPtr<UDynamicMeshPool> DynamicMeshPool;

	TObjectPtr<FastNoiseLite> Noise;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape", meta = (ToolTip = "How much distance to go until checking the noise again."))
	float Resolution = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape", meta = (ToolTip = "Height Scale of the Landscape"))
	float HeightScale = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape|Materials", meta = (ToolTip = "Default Material"))
	UMaterialInterface* DefaultLandscapeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape", meta = (ToolTip = "Chunk spacing/Distance to go until make chunk follow"))
	float ChunkDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape", meta = (ToolTip = "How far the chunk should go"))
	float RenderDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Noise")
	int Seed = 1337;

	UPROPERTY(EditAnywhere, Category = "Noise")
	float Frequency = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_NoiseType> NoiseType = NoiseType_Perlin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_RotationType3D> RotationType3D = RotationType3D_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_FractalType> FractalType = FractalType_FBm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	int FractalOctaves = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float FractalLacunarity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float FractalGain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float FractalWeightedStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float FractalPingPongStrength = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_CellularDistanceFunction> CellularDistanceFunction = CellularDistanceFunction_EuclideanSq;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_CellularReturnType> CellularReturnType = CellularReturnType_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float CellularJitter = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	TEnumAsByte<FastNoiseLiteTypes_DomainWarpType> DomainWarpType = DomainWarpType_OpenSimplex2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float DomainWarpAmp = 1.0f;

	UE::Geometry::EValidityCheckFailMode ValidityCheckFailMode = UE::Geometry::EValidityCheckFailMode::Ensure;

	FDynamicMesh3::FValidityOptions ValidityOptions = { false, false };

	FWorldGenerationRunnable* WorldGenerationRunnable;

	bool bCurrentlyGenerating = false;

	UPROPERTY(BlueprintReadOnly, Category = "Landscape Generation")
	TArray<FVector> GeneratedVertexLocations;

	UPROPERTY(BlueprintReadOnly, meta = (Tooltip = "The local pawn for this client. Does not need to be changed by blueprints because it is automatically set at beginplay in c++."))
	APawn* LocalClientPawn;

	UPROPERTY(BlueprintReadWrite)
	FVector LastPlayerLocation;
};
