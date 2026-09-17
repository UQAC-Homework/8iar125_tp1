#pragma once

#include "CoreMinimal.h"
#include "Fruit.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "Catcher.generated.h"

UCLASS()
class TP_8IAR125_TP1_API ACatcher : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere Category="Movement")
	bool HasTarget;

	UPROPERTY(VisibleInstanceOnly, Category="Movement")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category="Movement")
	float SlowMovementRange;

	UPROPERTY(EditAnywhere, Category="Movement")
	float StopMovementRange;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* Movement;

	/// Computes the next fruit to target
	static AFruit* GetNextFruit(const TArray<TWeakObjectPtr<AFruit>>& Fruits, const FVector& Position);

	/// Computes the next velocity to move
	static FVector GetNextVelocity(
		const FVector& Position,
		const FVector& Target,
		float MaxSpeed,
		float SlowRange,
		float StopRange
	);

	/// Moves to the given position
	void MoveTo(const FVector& TargetPosition) const;

	/// Computes a new random target position
	FVector ComputeTarget() const;

protected:
	/// Called when an attack is performed
	UFUNCTION(BlueprintNativeEvent)
	void OnAttack() const;

public:
	ACatcher();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
