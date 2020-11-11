// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

	void RefreshNavMesh();

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

private:
	void OptimizeScene();
	void SetupNavMeshSettings();
	void SpawnNavMesh();
};
