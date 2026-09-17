#include "Catcher.h"

#include "FruitTrackerSystem.h"
#include "Kismet/KismetMathLibrary.h"

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
	const UFruitTrackerSystem* Tracker,
	const FVector& Position
)
{
	AFruit* ClosestFruit = nullptr;
	float ClosestDistance = FLT_MAX;

	for (const auto Instance : Tracker->GetActiveFruits())
	{
		const auto Fruit = Instance.Get();

		if (Fruit == nullptr)
			continue;

		const auto Distance = abs(Position.X - Fruit->GetActorLocation().X);

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

	this->HasTarget = false;
	this->TargetLocation = FVector::ZeroVector;
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
	if (!this->HasTarget && !this->TryFindingTarget(Tracker))
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

void ACatcher::SetTarget(const AFruit* Target)
{
	if (Target == nullptr)
	{
		this->HasTarget = false;
		return;
	}

	FVector Position = this->GetActorLocation();
	Position.Y = Target->GetActorLocation().Y;

	this->TargetLocation = Position;
	this->HasTarget = true;
}

bool ACatcher::TryFindingTarget(const UFruitTrackerSystem* Tracker)
{
	const auto Fruit = GetNextFruit(
		Tracker,
		this->GetActorLocation()
	);

	this->SetTarget(Fruit);

	return Fruit != nullptr;
}

void ACatcher::MoveTo(const FVector& TargetPosition) const
{
	if (this->Movement == nullptr)
		return;

	const auto Position = this->GetActorLocation();
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

	if (!this->HasTarget)
		return;

	this->MoveTo(this->TargetLocation);
}

void ACatcher::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Fruit = Cast<AFruit>(OtherActor);

	// If not fruit, skip
	if (Fruit == nullptr)
		return;

	this->OnAttack();

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
