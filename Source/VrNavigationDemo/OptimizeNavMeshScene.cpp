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

OptimizeNavMeshScene::OptimizeNavMeshScene(UWorld* NewWorld)
{
	this->World = NewWorld;

	NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);
}


OptimizeNavMeshScene::~OptimizeNavMeshScene()
{
}


void OptimizeNavMeshScene::OptimizeAllMesh()
{
	if (World != nullptr)
	{
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{		
			if (ActorItr->Tags.Contains("Wall") || ActorItr->Tags.Contains("Pillar") || ActorItr->Tags.Contains("Object")) 
			{
				TArray<UBoxComponent*> BoxComps;
				ActorItr->GetComponents(BoxComps);

				TArray<UStaticMeshComponent*> StaticComps;
				ActorItr->GetComponents(StaticComps);

				TArray<UProceduralMeshComponent*> ProceduralComps;
				ActorItr->GetComponents(ProceduralComps);

				if (BoxComps.Num() > 0)
				{
					//UE_LOG(LogTemp, Error, TEXT("ENTRO QUI."));
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
					//UE_LOG(LogTemp, Error, TEXT("No box for: %s"), *ActorItr->GetName());
				

					if (ProceduralComps.Num() > 0)
						UpdateMeshWithBox(ActorItr, ProceduralComps);

					if (StaticComps.Num() > 0)
					{
						
						TArray<UStaticMeshComponent*> NewStaticComps;

						for (UStaticMeshComponent* elem : StaticComps)
						{
							auto navCollision = Cast<UNavCollision>(elem->GetStaticMesh()->GetNavCollision());
							navCollision->AreaClass;

						
							//UE_LOG(LogTemp, Error, TEXT("Devo vedere la static mesh %i"), elem->GetStaticMesh()->GetNavCollision()->IsDynamicObstacle());

							if (elem->GetStaticMesh()->GetNavCollision()->IsDynamicObstacle() == false || navCollision->AreaClass != UNavArea_Null::StaticClass())
							{
								NewStaticComps.Add(elem);
							}
						}

						if(NewStaticComps.Num() > 0)
							UpdateMeshWithBox(ActorItr, NewStaticComps);
					}
				}

			}
		}
	}
}

template <class T>
void OptimizeNavMeshScene::UpdateMeshWithBox(TActorIterator<AActor> const& ActorItr, TArray<T*> const& Comps)
{
	UBoxComponent* CollisionMesh = NewObject<UBoxComponent>(*ActorItr, UBoxComponent::StaticClass(), TEXT("BoxCollider"));
	if (CollisionMesh)
	{
		for (T* Elem : Comps)
		{
			/*ActorItr->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			CollisionMesh->SetMobility(EComponentMobility::Movable);*/
			CollisionMesh->SetWorldLocation(ActorItr->GetActorLocation());
			ActorItr->SetRootComponent(CollisionMesh);

			UpdateMeshBounds(CollisionMesh, Elem);

			CollisionMesh->RegisterComponent();
		}
	}
}


template <class T>
void OptimizeNavMeshScene::UpdateSpecificMeshWithBox(TActorIterator<AActor> const& ActorItr, T* const& Mesh)
{
	UBoxComponent* CollisionMesh = NewObject<UBoxComponent>(*ActorItr, UBoxComponent::StaticClass(), TEXT("BoxCollider"));
	if (CollisionMesh)
	{
		/*ActorItr->GetRootComponent()->SetMobility(EComponentMobility::Movable);
		CollisionMesh->SetMobility(EComponentMobility::Movable);*/
		CollisionMesh->SetWorldLocation(ActorItr->GetActorLocation());
		ActorItr->SetRootComponent(CollisionMesh);

		UpdateMeshBounds(CollisionMesh, Mesh);

		CollisionMesh->RegisterComponent();
	}
}


template <class T>
void OptimizeNavMeshScene::UpdateMeshBounds(UBoxComponent* const &CollisionMesh, T* const &Mesh)
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
			NavigationSystemV1->UpdateComponentInNavOctree(*CollisionMesh);			
			UE_LOG(LogTemp, Error, TEXT("Update box component for: %s"), *Mesh->GetName());
		}
	}
}
