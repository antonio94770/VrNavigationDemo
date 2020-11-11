// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NavMeshManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRNAVIGATIONDEMO_API UNavMeshManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNavMeshManager();

	UNavMeshManager(UWorld*);

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
	void SetUWorld(UWorld*);
	void SetupNavMeshSettings();
	void RefreshNavMesh();
	void SpawnNavMesh();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
