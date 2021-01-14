// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

/**
 * 
 */

UENUM()
enum class ENavMeshTypeForSceneBounds : uint8 {
	SINGLEMODE,
	ONEFLOOR,
	MULTIPLEFLOOR,
};

class NavMeshController;

class VRNAVIGATIONDEMO_API NavMeshSceneBounds
{


public:
	NavMeshSceneBounds();
	NavMeshSceneBounds(UWorld* NewWorld, const class NavMeshController& );
	~NavMeshSceneBounds();

private:
	//UFUNCTION()
	TArray<float > GetAllFloorActorsPosition() const;

	//UFUNCTION()
	TArray<AActor* > GetAllSceneActors() const;

	//UFUNCTION()
	void GetFloorBounds(FVector Difference, float MinFloorHeight, float MaxFloorHeight);

public:	
	//UFUNCTION()
	FVector GetOptimalNavMeshPosition(int Floor);

	//UFUNCTION()
	FVector	GetNavMeshBounds(FVector NavMeshPosition, int Floor);
	
	//UFUNCTION()
	int GetNumberOfFloors() const;

	//UFUNCTION()
	float GetMinFloorHeight() const;

	//UFUNCTION()
	float GetMaxFloorHeight() const;

	//UFUNCTION()
	void ResetBounds();

private:

	//Pointer for GetWorld()
	//UPROPERTY()
	UWorld* World;

	//FVector for the bounds for the navmesh
	//UPROPERTY()
	FVector NavMeshBounds;

	//List of actors in the scene that have a particular flag
	//UPROPERTY()
	TArray<AActor*> ActorsArray;

	//Actor's origin for every floors in the scene
	//UPROPERTY()
	TArray<float > FloorsArrayOrigin;

	//Max X,Y,Z, var to calculate the bounds
	//UPROPERTY()
	float MaxX,MaxY,MaxZ;

	//Min X,Y,Z, var to calculate the bounds
	//UPROPERTY()
	float MinY,MinX,MinZ;

	//Bool to remove, used for Single Floor mode
	//UPROPERTY()
	bool bSingleMode = false;

	//Min X for the current Floor
	//UPROPERTY()
	float MinFloorHeight; 
	
	//Max X for the current Floor
	//UPROPERTY()
	float MaxFloorHeight;
};
