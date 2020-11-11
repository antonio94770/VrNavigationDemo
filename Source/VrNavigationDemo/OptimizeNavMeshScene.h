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
	class UNavigationSystemV1* navigationSystemV1;

public:
	OptimizeNavMeshScene(UWorld* );
	~OptimizeNavMeshScene();

	UFUNCTION()
	void OptimizeAllMesh();

private:

	UFUNCTION()
	void UpdateStaticMesh();
			
	template <class T>
	UFUNCTION()
	void UpdateMeshWithBox(const TActorIterator<class AActor>& , const TArray<T*>& );

	template <class T>
	UFUNCTION()
	void UpdateMeshBounds(class UBoxComponent* const &CollisionMesh, T* const& Mesh);

};