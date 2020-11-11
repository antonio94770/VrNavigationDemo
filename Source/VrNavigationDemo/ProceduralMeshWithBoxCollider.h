// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "ProceduralMeshWithBoxCollider.generated.h"

class UProceduralMeshComponent;

UCLASS()
class VRNAVIGATIONDEMO_API AProceduralMeshWithBoxCollider : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AProceduralMeshWithBoxCollider();

public:
		


private:

	UPROPERTY();
	FBoxSphereBounds MeshBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* ProceduralMesh = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionMesh = nullptr;

private:
	void UpdateCollisionMesh();

};
