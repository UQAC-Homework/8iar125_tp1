#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(VisibleInstanceOnly, Category="Movement")
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float SlowMovementRange;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float StopMovementRange;
	
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* Movement;
	
	static FVector GetNextVelocity(const FVector& Position, const FVector& Target, float MaxSpeed, float SlowRange, float StopRange);
	void MoveToTarget(const FVector& Target) const;
	FVector PickNewTarget() const;

public:	
	ACatcher();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnAttack() const;
};
