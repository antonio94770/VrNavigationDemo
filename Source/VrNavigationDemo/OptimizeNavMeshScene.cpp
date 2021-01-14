// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimizeNavMeshScene.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "AI/Navigation/NavCollisionBase.h"
#include "NavCollision.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "Runtime\NavigationSystem\Public\NavigationSystem.h"


//In this constructor we inizialize the first two variables
OptimizeNavMeshScene::OptimizeNavMeshScene(UWorld* NewWorld)
{
	this->World = NewWorld;

	NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);
}


OptimizeNavMeshScene::~OptimizeNavMeshScene()
{
}


//TO EDIT IN DC: Given all actors in the range of the two heights we call the function to optimize them.
void OptimizeNavMeshScene::OptimizeAllMesh(float MinFloorHeight, float MaxFloorHeight) const
{
	if (World != nullptr)
	{
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			FVector Origin;
			FVector BoxExtent;
			ActorItr->GetActorBounds(false, Origin, BoxExtent);

			Origin.X = Origin.X;
			Origin.Y = Origin.Y;
			Origin.Z = Origin.Z;

			if ((Origin.Z < MaxFloorHeight && Origin.Z > MinFloorHeight) || (MaxFloorHeight < MinFloorHeight && Origin.Z > MinFloorHeight))
			{
				if (ActorItr->Tags.Contains("Wall") || ActorItr->Tags.Contains("Pillar") || ActorItr->Tags.Contains("Object"))
				{
					OptimizeSingleNavMeshActor(*ActorItr);
				}
			}
		}
	}
}


//We check if the actor has procedural o static mesh component and then, if not already optimized, we optimize it
void OptimizeNavMeshScene::OptimizeSingleNavMeshActor(AActor* const Actor) const
{	
	TArray<UBoxComponent*> BoxComps;
	Actor->GetComponents(BoxComps);

	TArray<UStaticMeshComponent*> StaticComps;
	Actor->GetComponents(StaticComps);

	TArray<UProceduralMeshComponent*> ProceduralComps;
	Actor->GetComponents(ProceduralComps);

	if (BoxComps.Num() > 0)
	{
		for (UBoxComponent* BoxElem : BoxComps)
		{
			for (UStaticMeshComponent* StaticElem : StaticComps)
			{
				UpdateMeshBounds(BoxElem, StaticElem);
			}

			for (UProceduralMeshComponent* ProcElem : ProceduralComps)
			{
				UpdateMeshBounds(BoxElem, ProcElem);
			}
		}
	}
	else
	{
		if (ProceduralComps.Num() > 0)
			UpdateMeshWithBox(Actor, ProceduralComps);

		if (StaticComps.Num() > 0)
		{

			TArray<UStaticMeshComponent*> NewStaticComps;

			for (UStaticMeshComponent* elem : StaticComps)
			{
				const UNavCollision* navCollision = Cast<UNavCollision>(elem->GetStaticMesh()->GetNavCollision());

				if (elem->GetStaticMesh()->GetNavCollision()->IsDynamicObstacle() == false || navCollision->AreaClass != UNavArea_Null::StaticClass())
				{
					NewStaticComps.Add(elem);
				}
				else
					NavigationSystemV1->UpdateComponentInNavOctree(*elem);

			}

			if (NewStaticComps.Num() > 0)
				UpdateMeshWithBox(Actor, NewStaticComps);
		}
	}
}


//Given an actor with his mesh, we instantiate a new UBoxComponent
template <class T>
void OptimizeNavMeshScene::UpdateMeshWithBox(AActor* const ActorItr, const TArray<T*>& Comps) const
{
	UBoxComponent* CollisionMesh = NewObject<UBoxComponent>(ActorItr, UBoxComponent::StaticClass(), TEXT("BoxCollider"));
	if (CollisionMesh)
	{
		for (T* Elem : Comps)
		{
			CollisionMesh->SetWorldLocation(ActorItr->GetActorLocation());
			ActorItr->SetRootComponent(CollisionMesh);

			UpdateMeshBounds(CollisionMesh, Elem);

			CollisionMesh->RegisterComponent();
		}
	}
}


//We clear the current NavOctree from the actor the we pass in the function
void OptimizeNavMeshScene::ClearNavMesh() const
{
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr) {
		if (!ActorItr->Tags.Contains("Floor") && NavigationSystemV1 != nullptr)
			NavigationSystemV1->ClearNavOctreeAll(*ActorItr);
	}
}


//Function never used that update a mesh with a UBoxComponent
template <class T>
void OptimizeNavMeshScene::UpdateSpecificMeshWithBox(TActorIterator<AActor> const& ActorItr, T* const& Mesh) const
{
	UBoxComponent* CollisionMesh = NewObject<UBoxComponent>(*ActorItr, UBoxComponent::StaticClass(), TEXT("BoxCollider"));
	if (CollisionMesh)
	{
		CollisionMesh->SetWorldLocation(ActorItr->GetActorLocation());
		ActorItr->SetRootComponent(CollisionMesh);

		UpdateMeshBounds(CollisionMesh, Mesh);

		CollisionMesh->RegisterComponent();
	}
}


//If the mesh extent changed, we update it, also we put the right value for the optimization and we update the mesh in NavOctree
template <class T>
void OptimizeNavMeshScene::UpdateMeshBounds(UBoxComponent* const &CollisionMesh, T* const &Mesh) const
{
	if (NavigationSystemV1 != nullptr)
	{	
		CollisionMesh->AreaClass = UNavArea_Null::StaticClass();
		CollisionMesh->bDynamicObstacle = true;

		Mesh->SetCanEverAffectNavigation(false);
		//NavigationSystemV1->RemoveObjectsNavOctreeId(*Mesh);

		if (CollisionMesh->Bounds.BoxExtent != Mesh->Bounds.BoxExtent)
		{	
			CollisionMesh->SetBoxExtent(Mesh->Bounds.BoxExtent);			
		}

		NavigationSystemV1->UpdateComponentInNavOctree(*CollisionMesh);
		UE_LOG(LogTemp, Error, TEXT("Update box component for: %s"), *Mesh->GetName());
	}
}
