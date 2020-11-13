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
	TArray<AActor* > GetAllSceneActor();

	UFUNCTION()
	void GetSceneBounds(FVector Difference);

public:	
	UFUNCTION()
	FVector GetOptimalNavMeshPosition();

	UFUNCTION()
	FVector	GetNavMeshBounds(FVector NavMeshPosition); 
		
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
	float MaxX,MaxY,MaxZ;

	UPROPERTY()
	float MinY,MinX,MinZ;
};
