// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGameMode.h"
#include "PlayerCharacter.h"
#include "WorldLandscape.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

AWorldGameMode::AWorldGameMode()
{

}

void AWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get the world landscape immediately, making the landscape giive us the their reference is too slow and inconsistent
	for (TActorIterator<AWorldLandscape> It(GetWorld()); It; ++It)
	{
		AWorldLandscape* GivenLandscape = *It;

		if (!IsValid(Landscape)) {
			// There is no landscape right now, so there is just 1 for now
			Landscape = GivenLandscape;
			check(IsValid(Landscape));
		}
		else {
			// I didn't implement multiple landscapes, so just crash
			unimplemented();
		}
	}

	LandscapeGeneratedDelegate.AddUObject(this, &AWorldGameMode::SetPlayerSpawns);
}

void AWorldGameMode::SetPlayerSpawns()
{
	// Tmp: Just set all players to spawn in the same location
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* Player = *It;
		
		check(IsValid(Player));

		FVector LineTraceHitLocation = FindPlayerSpawnLocation();

		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::Printf(TEXT("THing is: %s"), *FVector(LineTraceHitLocation.X, LineTraceHitLocation.Y, LineTraceHitLocation.Z + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()).ToString()));
		Player->SetActorLocation(FVector(LineTraceHitLocation.X, LineTraceHitLocation.Y, (LineTraceHitLocation.Z + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

FVector AWorldGameMode::FindPlayerSpawnLocation() const
{
	FVector FinalSpawnLocation = FVector(0, 0, SpawnCheckRaycastDistance);

	// Settings for the line trace
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	bool Finished = false;
	
	// Could use multilinetrace, but this is easier
	if (GetWorld()->LineTraceSingleByChannel(Hit, FinalSpawnLocation, FVector(0, 0, -SpawnCheckRaycastDistance), LandscapeChannel, Params)) {
		// This means that the spawn location is above 
		return FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);
	}

	ensure(!"Couldn't find a landscape to put the player on, so giving world origin.");
	return FVector::ZeroVector;
}
