#include "Catcher.h"

#include "FruitTrackerSystem.h"
#include "ScoreSystem.h"
#include "Kismet/KismetMathLibrary.h"

/// Finds the fruit tracker system from the given instance
static UFruitTrackerSystem* GetFruitTrackerSystem(const ACatcher* Catcher)
{
	if (Catcher == nullptr)
		return nullptr;

	const auto GameInstance = Catcher->GetGameInstance();

	if (GameInstance == nullptr)
		return nullptr;

	return GameInstance->GetSubsystem<UFruitTrackerSystem>();
}

/// Computes the next fruit to target
static AFruit* GetNextFruit(
	const TArray<TWeakObjectPtr<AFruit>>& Fruits,
	const FVector& Position
)
{
	AFruit* ClosestFruit = nullptr;
	float ClosestDistance = FLT_MAX;

	for (const auto Instance : Fruits)
	{
		const auto Fruit = Instance.Get();

		if (Fruit == nullptr)
			continue;

		const auto FruitPosition = Fruit->GetActorLocation();

		if (FruitPosition.X > Position.X)
			continue;

		const auto Distance = Position.X - FruitPosition.X;

		if (Distance >= ClosestDistance)
			continue;

		ClosestDistance = Distance;
		ClosestFruit = Fruit;
	}

	return ClosestFruit;
}

/// Computes the velocity needed to move from the given position to the given target position
static FVector GetNextVelocity(
	const FVector& Position,
	const FVector& Target,
	const float MaxSpeed,
	const float SlowRange,
	const float StopRange
)
{
	auto DesiredVelocity = Target - Position;

	if (DesiredVelocity.IsNearlyZero())
		return FVector::ZeroVector;

	const auto DesiredMovement = DesiredVelocity.Length();

	// If arrived, fail movement
	if (DesiredMovement <= StopRange)
		return FVector::ZeroVector;

	auto Speed = MaxSpeed;

	// If going to over-shoot
	if (DesiredMovement <= SlowRange)
	{
		Speed = UKismetMathLibrary::MapRangeClamped(
			DesiredMovement,
			0,
			SlowRange,
			0,
			MaxSpeed
		);
	}

	DesiredVelocity.Normalize();

	DesiredVelocity *= Speed / MaxSpeed;

	return DesiredVelocity;
}

ACatcher::ACatcher()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	this->BoxCollision = CreateDefaultSubobject<UBoxComponent>("Collision");
	this->BoxCollision->SetupAttachment(RootComponent);

	this->CurrentTarget = nullptr;
	this->SlowMovementRange = 1000;
	this->StopMovementRange = 20;
	this->Movement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
	this->Movement->UpdatedComponent = RootComponent;
}

void ACatcher::BeginPlay()
{
	Super::BeginPlay();

	this->RequestNewTarget();
}

void ACatcher::OnFruitSpawned()
{
	const auto Tracker = GetFruitTrackerSystem(this);

	if (Tracker == nullptr)
		return;

	// If no target and failed to find a new one, keep event
	if (this->CurrentTarget == nullptr && !this->TryFindingTarget(Tracker))
		return;

	Tracker->OnFruitRegistered.Remove(this->OnFruitSpawnedHandle);
}

void ACatcher::RequestNewTarget()
{
	const auto Tracker = GetFruitTrackerSystem(this);

	if (Tracker == nullptr)
		return;

	if (this->TryFindingTarget(Tracker))
		return;

	this->OnFruitSpawnedHandle = Tracker->OnFruitRegistered.AddUObject(this, &ACatcher::OnFruitSpawned);
}

bool ACatcher::TryFindingTarget(const UFruitTrackerSystem* Tracker)
{
	const auto Fruit = GetNextFruit(
		Tracker->GetActiveFruits(),
		this->GetActorLocation()
	);

	this->CurrentTarget = Fruit;

	return Fruit != nullptr;
}

void ACatcher::MoveTo(const AFruit* Target) const
{
	if (this->Movement == nullptr)
		return;

	const auto Position = this->GetActorLocation();
	FVector TargetPosition = Position;
	TargetPosition.Y = Target->GetActorLocation().Y;

	const auto NextVelocity = GetNextVelocity(
		Position,
		TargetPosition,
		this->Movement->MaxSpeed,
		this->SlowMovementRange,
		this->StopMovementRange
	);

	if (NextVelocity.IsZero())
		return;

	this->Movement->AddInputVector(NextVelocity);
}

void ACatcher::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->CurrentTarget == nullptr)
		return;

	if (this->CurrentTarget->GetActorLocation().X > this->GetActorLocation().X - 200)
	{
		this->RequestNewTarget();
		return;
	}

	this->MoveTo(this->CurrentTarget);
}

void ACatcher::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Fruit = Cast<AFruit>(OtherActor);

	// If not fruit, skip
	if (Fruit == nullptr)
		return;

	this->OnAttack();
	
	GetGameInstance()->GetSubsystem<UScoreSystem>()->RecordCapture();

	// Destroy fruit
	Fruit->Destroy();

	// Pick new target
	this->RequestNewTarget();
}

void ACatcher::OnAttack_Implementation() const
{
	// Spawn particles
	// Play attack animation
}
