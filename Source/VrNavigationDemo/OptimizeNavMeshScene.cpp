// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimizeNavMeshScene.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "AI/Navigation/NavCollisionBase.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "Runtime\NavigationSystem\Public\NavigationSystem.h"

OptimizeNavMeshScene::OptimizeNavMeshScene(UWorld* NewWorld)
{
	this->World = NewWorld;

	navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);
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
					UE_LOG(LogTemp, Error, TEXT("No box for: %s"), *ActorItr->GetName());
				

					if (ProceduralComps.Num() > 0)
						UpdateMeshWithBox(ActorItr, ProceduralComps);

					if (StaticComps.Num() > 0)
					{
						TArray<UStaticMeshComponent*> NewStaticComps;

						for (UStaticMeshComponent* elem : StaticComps)
						{
							if (!elem->GetStaticMesh()->GetNavCollision()->IsDynamicObstacle())
							{
								NewStaticComps.Add(elem);
							}
						}

						UpdateMeshWithBox(ActorItr, NewStaticComps);
					}
				}

			}
		}
	}
}

void OptimizeNavMeshScene::UpdateStaticMesh()
{
	if (World != nullptr)
	{
		/*for (TActorIterator<UStaticMeshComponent> ActorItr(World); ActorItr; ++ActorItr) {
			UStaticMeshComponent* Mesh = *ActorItr;
			auto a = Mesh->GetStaticMesh()->GetNavCollision()->IsDynamicObstacle();
		}*/

	}
}

template <class T>
void OptimizeNavMeshScene::UpdateMeshWithBox(const TActorIterator<AActor>& ActorItr, const TArray<T*>& Comps)
{
	UBoxComponent* CollisionMesh = NewObject<UBoxComponent>(*ActorItr, UBoxComponent::StaticClass(), TEXT("BoxCollider"));
	if (CollisionMesh)
	{
		for (T* Elem : Comps)
		{
			ActorItr->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			CollisionMesh->SetMobility(EComponentMobility::Movable);
			CollisionMesh->SetWorldLocation(ActorItr->GetActorLocation());
			ActorItr->SetRootComponent(CollisionMesh);

			UpdateMeshBounds(CollisionMesh, Elem);

			CollisionMesh->RegisterComponent();
		}
	}
}

template <class T>
void OptimizeNavMeshScene::UpdateMeshBounds(UBoxComponent* const &CollisionMesh, T* const &Mesh)
{
	if (navigationSystemV1 != nullptr)
	{
		
		CollisionMesh->AreaClass = UNavArea_Null::StaticClass();
		CollisionMesh->bDynamicObstacle = true;

		Mesh->SetCanEverAffectNavigation(false);

		if (CollisionMesh->Bounds != Mesh->Bounds)
		{		
			CollisionMesh->SetBoxExtent(Mesh->Bounds.BoxExtent);
			navigationSystemV1->UpdateComponentInNavOctree(*CollisionMesh);
			UE_LOG(LogTemp, Error, TEXT("Update box component for: %s"), *Mesh->GetName());
		}
	}

}
