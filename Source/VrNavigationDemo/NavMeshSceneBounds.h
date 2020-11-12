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

public:
	UFUNCTION()
	FVector GetNavMeshBounds();

private:

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	FVector NavMeshBounds;

	UPROPERTY()
	TArray<AActor*> ActorsArray;
};
