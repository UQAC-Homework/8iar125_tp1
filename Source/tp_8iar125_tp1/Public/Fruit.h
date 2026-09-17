#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Fruit.generated.h"

UCLASS()
class TP_8IAR125_TP1_API AFruit : public APawn
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Movement")
	FVector Direction;

	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed;
	
public:
	AFruit();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void LifeSpanExpired() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
