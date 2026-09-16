#include "tp_8iar125_tp1/Public/Fruit.h"


AFruit::AFruit()
{
	this->PrimaryActorTick.bCanEverTick = true;
	this->Direction = FVector::ForwardVector;
	this->Speed = 1.0f;
}

void AFruit::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Position = this->GetActorLocation();

	const auto Movement = this->Direction * this->Speed;
	Position += Movement;

	this->SetActorLocation(Position);
}
