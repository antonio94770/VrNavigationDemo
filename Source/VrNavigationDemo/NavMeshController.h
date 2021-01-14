// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OptimizeNavMeshScene.h"
#include "NavMeshSceneBounds.h"


/**
 * ENUM for navmesh type utilized for floor decision
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

	NavMeshController(UWorld* NewWorld);
	
	NavMeshController(UWorld* NewWorld, ENavMeshTypeController bDifferentNavMeshForDifferentFloor, bool bOptimize);

	~NavMeshController();

public:

	//UFUNCTION()
	void RefreshNavMeshBounds();

	//UFUNCTION()
	void ChangeCurrentFloor(int NewFloor);

public:

	//Number of floors get from NavMeshSceneBounds.cpp, used from VRCharacter.cpp to check if can increase the current Floor Count
	//UPROPERTY()
	int FloorsNumber;

	//NavMeshMode to edit the behavior of the different modes
	//UPROPERTY()
	ENavMeshTypeController NavMeshMode;

private:
	//Pointer to the current GetWorld()
	//UPROPERTY()
	class UWorld* World;

	//Var to edit the parameters of recast, STILL NOT USED
	//UPROPERTY()
	class ARecastNavMesh* RecastNavmesh;

	//Var to host the current NavigationSystem
	//UPROPERTY()
	class UNavigationSystemV1* NavigationSystemV1 = nullptr;

	//Array of NavMeshBoundsVolume
	//UPROPERTY()
	TArray<class ANavMeshBoundsVolume*> ArrayOfNavMeshBoundsVolume;

	//Current floor counter
	//UPROPERTY()
	int CurrentFloor;

	//Array of initial navmesh shape parameters, so we can calculate the difference for the bounds, based on this values
	//UPROPERTY()
	TArray<FVector> InitialNavMeshBound;

	//Reference to NavMeshSceneBounds
	//UPROPERTY()
	NavMeshSceneBounds SceneBounds;

	//Reference to OptimizeNavMeshScene
	//UPROPERTY()
	OptimizeNavMeshScene Optimizer;
	
	//FOR TESTING: switch on-off from VRCharacter
	//UPROPERTY()
	bool bOptimize;

private:

	//UFUNCTION()
	void SetupNavMeshSettings();

	//UFUNCTION()
	void SpawnNavMesh();
};
