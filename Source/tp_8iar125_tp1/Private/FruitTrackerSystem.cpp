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
