// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCanUseMouseInputForCamera = false;

	UPROPERTY()
	class USceneComponent* RootVR;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* LeftController;
	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* RightController;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* TeleportPath;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* TeleportTrajectoryMesh;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* TeleportTrajectoryMaterial;

	UPROPERTY()
	TArray<class USplineMeshComponent*> TeleportMeshPool;

private:
	/*UPROPERTY(EditAnywhere)
	float MaxTeleportDistance = 100.f;*/

	//Curve element's radius
	UPROPERTY(EditAnywhere)
	float TeleportProjectileRadius = 10.f;

	//Curve Lenght
	UPROPERTY(EditAnywhere)
	float TeleportProjectileSpeed = 1000.f;
	
	//Curve Height
	UPROPERTY(EditAnywhere)
	float TeleportSimulationTime = 1.5f;

	UPROPERTY(EditAnywhere)
	float TeleportFadeTime = 1.f;

	UPROPERTY(EditAnywhere)
	FVector TeleportExtent = FVector(10.f, 10.f, 10.f);

	UPROPERTY()
	FVector LastUsefullPositionForTeleport;

	UPROPERTY()
	bool bCanTeleport;

	UPROPERTY()
	bool bAlreadyTeleported;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshWithBoxCollider> BP_ProceduralMeshWithBoxCollider;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshWithBoxCollider> BP_ProceduralMeshWithBoxColliderWithCollision;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProceduralMeshDefault> BP_ProceduralMeshDefault;

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
	
	void SpawnProceduralMesh();
	void SpawnProceduralMeshWithCollision();
	void SpawnDefaultProceduralMesh();
	void SpawnNavMesh();
	void CallOptimizeNavMesh();
};
