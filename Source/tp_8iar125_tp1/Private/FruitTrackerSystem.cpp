#include "FruitTrackerSystem.h"

UFruitTrackerSystem::UFruitTrackerSystem()
{
	this->ActiveFruits = TArray<TWeakObjectPtr<AFruit>>();
}

UFruitTrackerSystem::~UFruitTrackerSystem()
{
	this->ActiveFruits.Empty();
	this->OnFruitRegistered.Clear();
}

void UFruitTrackerSystem::RegisterFruit(AFruit* Fruit)
{
	ActiveFruits.Add(Fruit);

	if (this->OnFruitRegistered.IsBound())
		this->OnFruitRegistered.Broadcast();
}

void UFruitTrackerSystem::UnregisterFruit(AFruit* Fruit)
{
	ActiveFruits.Remove(Fruit);
}

const TArray<TWeakObjectPtr<AFruit>>& UFruitTrackerSystem::GetActiveFruits() const
{
	return ActiveFruits;
}

UFruitTrackerSystem* UFruitTrackerSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
		return nullptr;

	const auto World = WorldContextObject->GetWorld();

	if (World == nullptr)
		return nullptr;

	const auto GameInstance = World->GetGameInstance();

	if (GameInstance == nullptr)
		return nullptr;

	return GameInstance->GetSubsystem<UFruitTrackerSystem>();
}
