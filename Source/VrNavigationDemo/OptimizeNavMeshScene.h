// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VRNAVIGATIONDEMO_API OptimizeNavMeshScene
{

private:

	//Pointer for GetWorld()
	//UPROPERTY()
	UWorld* World;

	//Pointer to the current NavigationSystem
	//UPROPERTY()
	class UNavigationSystemV1* NavigationSystemV1;

public:
	OptimizeNavMeshScene(UWorld* World);
	~OptimizeNavMeshScene();

	//UFUNCTION()
	void OptimizeAllMesh(float MinFloorHeight, float MaxFloorHeight) const;

	//UFUNCTION()
	void OptimizeSingleNavMeshActor(AActor* const Actor) const;

	template <class T>
	//UFUNCTION()
	void UpdateSpecificMeshWithBox(TActorIterator<AActor> const&, T* const&) const;

	template <class T>
	//UFUNCTION()
	void UpdateMeshBounds(class UBoxComponent* const&, T* const&) const;
		
	template <class T>
	//UFUNCTION()
	void UpdateMeshWithBox(AActor* const , const TArray<T*> & ) const;

	//UFUNCTION()
	void ClearNavMesh() const;

};