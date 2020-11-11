// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMeshWithBoxCollider.h"

#include "ProceduralMeshComponent.h"
#include <Runtime\NavigationSystem\Public\NavigationSystem.h>


// Sets default values
AProceduralMeshWithBoxCollider::AProceduralMeshWithBoxCollider()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Mesh"));
	SetRootComponent(CollisionMesh);

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	ProceduralMesh->SetupAttachment(CollisionMesh);

	/*CollisionMesh->AreaClass = UNavArea_Null::StaticClass();
	CollisionMesh->bDynamicObstacle = true;
	
	ProceduralMesh->SetCanEverAffectNavigation(false);

	UpdateCollisionMesh();
	*/
	//UNavigationSystemV1::GetNavigationSystem(GetWorld())->RemoveObjectsNavOctreeId(*ProceduralMesh);
	
}

// Called when the game starts or when spawned
void AProceduralMeshWithBoxCollider::BeginPlay()
{
	Super::BeginPlay();

	CollisionMesh->AreaClass = UNavArea_Null::StaticClass();
	CollisionMesh->bDynamicObstacle = true;

	ProceduralMesh->SetCanEverAffectNavigation(false);
}

// Called every frame
void AProceduralMeshWithBoxCollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UpdateCollisionMesh();
}

void AProceduralMeshWithBoxCollider::UpdateCollisionMesh()
{
	if (CollisionMesh != nullptr && ProceduralMesh != nullptr)
	{

		UNavigationSystemV1* navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		FBoxSphereBounds NewMeshBounds = ProceduralMesh->Bounds;

		if (NewMeshBounds != MeshBounds)
		{
			CollisionMesh->SetBoxExtent(NewMeshBounds.BoxExtent);
			MeshBounds = NewMeshBounds;

			navigationSystemV1->UpdateComponentInNavOctree(*CollisionMesh);
		}
		
	}
}

