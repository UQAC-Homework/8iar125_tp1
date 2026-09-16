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

	virtual void Tick(float DeltaTime) override;
};
