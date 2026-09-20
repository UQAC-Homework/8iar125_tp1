#include "Fruit.h"

#include "FruitTrackerSystem.h"
#include "ScoreSystem.h"


AFruit::AFruit()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<USceneComponent>("Root");

	this->Direction = FVector::ForwardVector;
	this->Movement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
	this->Movement->UpdatedComponent = RootComponent;
}

void AFruit::BeginPlay()
{
	Super::BeginPlay();

	const auto Tracker = UFruitTrackerSystem::Get(this);

	if (Tracker == nullptr)
		return;

	Tracker->RegisterFruit(this);
}

void AFruit::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->Movement == nullptr)
		return;

	const auto Force = this->Direction * this->Movement->MaxSpeed;
	this->Movement->AddInputVector(Force);
}

void AFruit::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	const auto ScoreSystem = UScoreSystem::Get(this);

	if (ScoreSystem != nullptr)
		ScoreSystem->RecordMiss();
}

void AFruit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const auto Tracker = UFruitTrackerSystem::Get(this);

	if (Tracker != nullptr)
		Tracker->UnregisterFruit(this);

	Super::EndPlay(EndPlayReason);
}
