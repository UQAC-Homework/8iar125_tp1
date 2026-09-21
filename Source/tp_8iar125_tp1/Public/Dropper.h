#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "Dropper.generated.h"

UCLASS()
class TP_8IAR125_TP1_API ADropper : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AActor> SpawnItem;
	
	UPROPERTY(EditDefaultsOnly, Category="Spawn")
	FVector SpawnOffset;
	
	UPROPERTY(VisibleInstanceOnly, Category="Movement")
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float SlowMovementRange;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float StopMovementRange;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> Movement;

	/// Spawns an item
	void Spawn() const;

	/// Computes the next velocity to move
	static FVector GetNextVelocity(const FVector& Position, const FVector& Target, float MaxSpeed, float SlowRange, float StopRange);
	
	/// Moves to the given position
	bool MoveTo(const FVector& TargetPosition) const;
	
	/// Changes the current target to a random target position
	void ChangeTarget();
	
	/// Computes a new random target position
	static FVector ComputeTarget(FVector Position, int MaxLeft, int MaxRight, int MinimumDistance);
	
public:
	ADropper();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
