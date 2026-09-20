#include "tp_8iar125_tp1/Public/Fruit.h"

#include "FruitTrackerSystem.h"
#include "ScoreSystem.h"


AFruit::AFruit()
{
	this->PrimaryActorTick.bCanEverTick = true;
	this->Direction = FVector::ForwardVector;
	this->Speed = 1.0f;
}

void AFruit::BeginPlay()
{
	Super::BeginPlay();

	const auto GameInstance = this->GetGameInstance();

	if (GameInstance == nullptr)
		return;

	const auto Tracker = GameInstance->GetSubsystem<UFruitTrackerSystem>();

	if (Tracker == nullptr)
		return;

	Tracker->RegisterFruit(this);
}

void AFruit::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Position = this->GetActorLocation();

	const auto Movement = this->Direction * this->Speed;
	Position += Movement;

	this->SetActorLocation(Position);
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
