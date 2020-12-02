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
	NavMeshSceneBounds(UWorld* World);
	NavMeshSceneBounds(UWorld* NewWorld, class NavMeshController& );
	~NavMeshSceneBounds();

private:
	UFUNCTION()
	TArray<float > GetAllFloorActorsPosition();

	UFUNCTION()
	TArray<AActor* > GetAllSceneActors();

	UFUNCTION()
	void GetFloorBounds(FVector Difference, float MinFloorHeight, float MaxFloorHeight);

public:	
	UFUNCTION()
	FVector GetOptimalNavMeshPosition(int Floor);

	UFUNCTION()
	FVector	GetNavMeshBounds(FVector NavMeshPosition, int Floor);
	
	UFUNCTION()
	int GetNumberOfFloors();

	UFUNCTION()
	float GetMinFloorHeight();

	UFUNCTION()
	float GetMaxFloorHeight();

	UFUNCTION()
	void ResetBounds();

private:

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	FVector NavMeshBounds;

	UPROPERTY()
	TArray<AActor*> ActorsArray;

	UPROPERTY()
	TArray<float > FloorsArrayOrigin;

	UPROPERTY()
	float MaxX,MaxY,MaxZ;

	UPROPERTY()
	float MinY,MinX,MinZ;

	UPROPERTY()
	bool bSingleMode = false;

	UPROPERTY()
	float MinFloorHeight; 
		
	UPROPERTY()
	float MaxFloorHeight;
};
