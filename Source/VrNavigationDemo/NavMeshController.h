// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavMeshSceneBounds.h"

/**
 * 
 */
class VRNAVIGATIONDEMO_API NavMeshController
{
public:
	NavMeshController();

	NavMeshController(UWorld*);

	~NavMeshController();

public:

	void RefreshNavMeshBounds();
	void ChangeCurrentFloor(int NewFloor);

private:
	UPROPERTY()
	class UWorld* World;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ANavMeshBoundsVolume> NavMeshBoundsVolume;
	//TSubclassOf<class ARecastNavMesh> RecastNavmesh;

	UPROPERTY()
	class ARecastNavMesh* RecastNavmesh;

	UPROPERTY()
	class UNavigationSystemV1* navigationSystemV1 = nullptr;

	UPROPERTY()
	TArray<class ANavMeshBoundsVolume*> arrayOfNavMeshBoundsVolume;

	UPROPERTY()
	int CurrentFloor;

	UPROPERTY()
	NavMeshSceneBounds SceneBounds;

private:
	void OptimizeScene();
	void SetupNavMeshSettings();
	void SpawnNavMesh();
};
