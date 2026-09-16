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
	UFloatingPawnMovement* Movement;

	void Spawn() const;

	static FVector GetNextVelocity(const FVector& Position, const FVector& Target, float MaxSpeed, float SlowRange, float StopRange);
	bool MoveToTarget(const FVector& Target) const;
	FVector PickNewTarget() const;
	
public:
	ADropper();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
