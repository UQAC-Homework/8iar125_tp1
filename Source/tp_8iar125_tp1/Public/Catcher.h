#pragma once

#include "CoreMinimal.h"
#include "Fruit.h"
#include "FruitTrackerSystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "Catcher.generated.h"

UCLASS()
class TP_8IAR125_TP1_API ACatcher : public APawn
{
	GENERATED_BODY()

	FDelegateHandle OnFruitSpawnedHandle;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleInstanceOnly, Category="Movement")
	AFruit* CurrentTarget;

	UPROPERTY(EditAnywhere, Category="Movement")
	float SlowMovementRange;

	UPROPERTY(EditAnywhere, Category="Movement")
	float StopMovementRange;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* Movement;

	/// Called when a fruit spawns
	void OnFruitSpawned();

	/// Requests a new target to follow
	void RequestNewTarget();

	/// Sets the given fruit as the current target
	//void SetTarget(const AFruit* Target);

	/// Attempts to find a new target
	bool TryFindingTarget(const UFruitTrackerSystem* Tracker);

	/// Moves to the given position
	void MoveTo(const AFruit* Target) const;

protected:
	/// Called when an attack is performed
	UFUNCTION(BlueprintNativeEvent)
	void OnAttack(const AFruit* Fruit) const;

public:
	ACatcher();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
