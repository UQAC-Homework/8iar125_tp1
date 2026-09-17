#include "FruitTrackerSystem.h"

UFruitTrackerSystem::UFruitTrackerSystem()
{
	this->ActiveFruits = TArray<TWeakObjectPtr<AFruit>>();
}

void UFruitTrackerSystem::RegisterFruit(AFruit* Fruit)
{
	ActiveFruits.Add(Fruit);
}

void UFruitTrackerSystem::UnregisterFruit(AFruit* Fruit)
{
	ActiveFruits.Remove(Fruit);
}

const TArray<TWeakObjectPtr<AFruit>>& UFruitTrackerSystem::GetActiveFruits() const
{
	return ActiveFruits;
}
