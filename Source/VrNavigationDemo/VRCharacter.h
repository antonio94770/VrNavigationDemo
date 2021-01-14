// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NavMeshController.h"
#include "VRCharacter.generated.h"


UCLASS()
class VRNAVIGATIONDEMO_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	//Reference to the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	//For testing i can use the mouse input to look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCanUseMouseInputForCamera = false;

	//Check it if you want to optimize the actor's floor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bOptimizeNavMesh;

	//Root of our character's components
	UPROPERTY()
	class USceneComponent* RootVR;

	//Controllers
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* LeftController;
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* RightController;

	//Teleport Path
	UPROPERTY(VisibleAnywhere)
	class USplineComponent* TeleportPath;

	//Mesh component of destination marker
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;

	//Trajectory Mesh
	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* TeleportTrajectoryMesh;

	//Trajectory Material
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* TeleportTrajectoryMaterial;

	//Pool of UsplineMeshComponent for the teleport
	UPROPERTY()
	TArray<class USplineMeshComponent*> TeleportMeshPool;

	//ONLY FOR TESTING
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ENavMeshTypeController NavMeshType;

private:
	//Curve element's radius
	UPROPERTY(EditAnywhere)
	float TeleportProjectileRadius = 10.f;

	//Curve Length
	UPROPERTY(EditAnywhere)
	float TeleportProjectileSpeed = 1000.f;
	
	//Curve Height
	UPROPERTY(EditAnywhere)
	float TeleportSimulationTime = 1.5f;

	//Teleport of the fade
	UPROPERTY(EditAnywhere)
	float TeleportFadeTime = 1.f;

	//Extent for the teleport 
	UPROPERTY(EditAnywhere)
	FVector TeleportExtent = FVector(10.f, 10.f, 10.f);

	//Used to save a position, otherwise we can teleport on last view position (also outside a map)
	UPROPERTY()
	FVector LastUsefullPositionForTeleport;

	//Bool to check if we can teleport
	UPROPERTY()
	bool bCanTeleport;

	//Bool to check if already teleported and then we can't still teleport
	UPROPERTY()
	bool bAlreadyTeleported;

	/* These 3 are for actor spawning test*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshActor> BP_ProceduralMeshNavModifier;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshDefault> BP_ProceduralMeshDefault;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshDefault> BP_ProceduralMeshOptimized;

	//ONLY FOR TESTING
	UPROPERTY()
	int CurrentFloorForTesting;

	//ONLY FOR TESTING, REMOVE IT!
	NavMeshController NavController;

	/* These 3 are for actor spawning test*/
	UPROPERTY()
	bool bSpawnedMeshNavModifier = false;

	UPROPERTY()
	bool bSpawnedDefaultMesh = false;
	
	UPROPERTY()
	bool bSpawnedOptimizedMesh = false;

private:
	void ForwardMovement(float moveSpeed);
	void RightMovement(float moveSpeed);
	void StartTeleport();
	void StartCameraFade(float From, float To);
	void EndTeleport();

	void UpdateDestinationMarker();
	bool FindTeleportDestination(TArray<FVector>&, FVector&);
	void UpdateSpline(const TArray<FVector> &Path);
	void DrawTeleportPath(const TArray<FVector> &Path);
	
	void SpawnProceduralMeshNavModifier();
	void SpawnDefaultProceduralMesh();
	void SpawnOptimizedProceduralMesh();
	void SpawnNavMesh();
	void SaveToFileForStudyingPerformance();
};
