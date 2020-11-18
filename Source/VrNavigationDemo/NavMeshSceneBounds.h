// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

/**
 * 
 */
class VRNAVIGATIONDEMO_API NavMeshSceneBounds
{
public:
	NavMeshSceneBounds();
	NavMeshSceneBounds(UWorld* World);
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bMoveAndRestrictToFloor = false;
};
