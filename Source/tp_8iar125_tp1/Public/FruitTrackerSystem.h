#pragma once

#include "CoreMinimal.h"
#include "Fruit.h"
#include "FruitTrackerSystem.generated.h"

// GameMode
UCLASS()
class TP_8IAR125_TP1_API UFruitTrackerSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly)
	TArray<TWeakObjectPtr<AFruit>> ActiveFruits;

public:
	UFruitTrackerSystem();
	virtual ~UFruitTrackerSystem() override;

	/// Called when a fruit is registered
	DECLARE_MULTICAST_DELEGATE(FOnFruitRegistered);
	FOnFruitRegistered OnFruitRegistered;

	/// Registers the given fruit as active
	void RegisterFruit(AFruit* Fruit);

	/// Unregisters the given fruit from being active 
	void UnregisterFruit(AFruit* Fruit);

	/// Gets every active fruit registered
	const TArray<TWeakObjectPtr<AFruit>>& GetActiveFruits() const;
	
	/// Gets the instance of the system
	static UFruitTrackerSystem* Get(const UObject* WorldContextObject);
};
