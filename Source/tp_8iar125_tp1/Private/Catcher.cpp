#include "Catcher.h"

#include "Fruit.h"
#include "FruitTrackerSystem.h"
#include "Kismet/KismetMathLibrary.h"

ACatcher::ACatcher()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	this->BoxCollision = CreateDefaultSubobject<UBoxComponent>("Collision");
	this->BoxCollision->SetupAttachment(RootComponent);

	this->TargetLocation = FVector::ZeroVector;
	this->SlowMovementRange = 1000;
	this->StopMovementRange = 20;
	this->Movement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
	this->Movement->UpdatedComponent = RootComponent;
}

void ACatcher::BeginPlay()
{
	Super::BeginPlay();

	this->TargetLocation = this->ComputeTarget();
}

FVector ACatcher::GetNextVelocity(
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

void ACatcher::MoveTo(const FVector& TargetPosition) const
{
	if (this->Movement == nullptr)
		return;

	const auto Position = this->GetActorLocation();
	const auto NextVelocity = this->GetNextVelocity(
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

FVector ACatcher::ComputeTarget() const
{
	FVector Position = this->GetActorLocation();
	const auto GameInstance = this->GetGameInstance();

	if (GameInstance == nullptr)
		return Position;

	const auto Tracker = GameInstance->GetSubsystem<UFruitTrackerSystem>();

	if (Tracker == nullptr)
		return Position;

	const AFruit* ClosestFruit = nullptr;
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

	if (ClosestFruit == nullptr)
		return Position;

	Position.Y = ClosestFruit->GetActorLocation().Y;

	return Position;
}

void ACatcher::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->MoveTo(this->ComputeTarget());

	//this->MoveTo(this->TargetLocation);
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
	this->TargetLocation = this->ComputeTarget();
}

void ACatcher::OnAttack_Implementation() const
{
	// Spawn particles
	// Play attack animation
}
