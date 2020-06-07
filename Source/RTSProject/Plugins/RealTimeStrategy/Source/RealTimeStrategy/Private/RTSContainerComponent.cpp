#include "RTSContainerComponent.h"

#include "GameFramework/Actor.h"

#include "RTSContainableComponent.h"
#include "RTSLog.h"


URTSContainerComponent::URTSContainerComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	// Set reasonable default values.
	Capacity = 1;
}

bool URTSContainerComponent::ContainsActor(const AActor* Actor) const
{
    return ContainedActors.Contains(Actor);
}

bool URTSContainerComponent::CanLoadActor(AActor* Actor) const
{
	return Capacity < 0 || ContainedActors.Num() < Capacity;
}

void URTSContainerComponent::LoadActor(AActor* Actor)
{
	if (!CanLoadActor(Actor))
	{
		return;
	}

	if (ContainedActors.Contains(Actor))
	{
		return;
	}

	// Add to container.
	ContainedActors.Add(Actor);

    URTSContainableComponent* ContainableComponent = Actor->FindComponentByClass<URTSContainableComponent>();

    if (IsValid(ContainableComponent))
    {
        ContainableComponent->SetContainer(GetOwner());
    }

	// Hide actor.
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);

	// Notify listeners.
	OnActorEntered.Broadcast(GetOwner(), Actor);

	UE_LOG(LogRTS, Log, TEXT("Actor %s has entered %s."), *Actor->GetName(), *GetOwner()->GetName());
}

void URTSContainerComponent::UnloadActor(AActor* Actor)
{
	if (!ContainedActors.Contains(Actor))
	{
		return;
	}

	// Remove from container.
	ContainedActors.Remove(Actor);

    URTSContainableComponent* ContainableComponent = Actor->FindComponentByClass<URTSContainableComponent>();

    if (IsValid(ContainableComponent))
    {
        ContainableComponent->SetContainer(nullptr);
    }

	// Show actor.
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);

	// Notify listeners.
	OnActorLeft.Broadcast(GetOwner(), Actor);

	UE_LOG(LogRTS, Log, TEXT("Actor %s has left %s."), *Actor->GetName(), *GetOwner()->GetName());
}

int32 URTSContainerComponent::GetCapacity() const
{
    return Capacity;
}

void URTSContainerComponent::SetCapacity(int32 InCapacity)
{
    Capacity = InCapacity;
}

TArray<AActor*> URTSContainerComponent::GetContainedActors() const
{
    return ContainedActors;
}
