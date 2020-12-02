// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OptimizeNavMeshScene.h"
#include "NavMeshSceneBounds.h"


/**
 * 
 */

UENUM()
enum class ENavMeshTypeController : uint8 {
	SINGLEMODE,
	ONEFLOOR,
	MULTIPLEFLOOR,
};


class VRNAVIGATIONDEMO_API NavMeshController
{
public:
	NavMeshController();
	
	NavMeshController(UWorld*, ENavMeshTypeController bDifferentNavMeshForDifferentFloor, bool bOptimize);

	~NavMeshController();

public:

	void RefreshNavMeshBounds();
	void ChangeCurrentFloor(int NewFloor);

public:
	UPROPERTY()
	int FloorsNumber;

	UPROPERTY()
	ENavMeshTypeController NavMeshMode;

private:
	UPROPERTY()
	class UWorld* World;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ANavMeshBoundsVolume> NavMeshBoundsVolume;
	//TSubclassOf<class ARecastNavMesh> RecastNavmesh;

	UPROPERTY()
	class ARecastNavMesh* RecastNavmesh;

	UPROPERTY()
	class UNavigationSystemV1* NavigationSystemV1 = nullptr;

	UPROPERTY()
	TArray<class ANavMeshBoundsVolume*> ArrayOfNavMeshBoundsVolume;

	UPROPERTY()
	int CurrentFloor;

	UPROPERTY()
	TArray<FVector> InitialNavMeshBound;

	UPROPERTY()
	NavMeshSceneBounds SceneBounds;

	UPROPERTY()
	OptimizeNavMeshScene Optimizer;
		
	UPROPERTY()
	bool bOptimize;

private:
	void SetupNavMeshSettings();
	void SpawnNavMesh();
};
