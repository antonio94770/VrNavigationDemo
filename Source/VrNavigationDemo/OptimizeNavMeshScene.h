// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VRNAVIGATIONDEMO_API OptimizeNavMeshScene
{

private:

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	class UNavigationSystemV1* NavigationSystemV1;

public:
	OptimizeNavMeshScene(UWorld* World);
	~OptimizeNavMeshScene();

	UFUNCTION()
	void OptimizeAllMesh(float MinFloorHeight, float MaxFloorHeight);

	UFUNCTION()
	void OptimizeSingleNavMeshActor(AActor* Actor);

	template <class T>
	UFUNCTION()
	void UpdateSpecificMeshWithBox(TActorIterator<AActor> const&, T* const&);

	template <class T>
	UFUNCTION()
	void UpdateMeshBounds(class UBoxComponent* const&, T* const&);
		
	template <class T>
	UFUNCTION()
	void UpdateMeshWithBox(AActor* , const TArray<T*>& );

	UFUNCTION()
	void ClearNavMesh();

};