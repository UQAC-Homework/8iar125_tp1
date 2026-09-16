#include "Catcher.h"

#include "Fruit.h"
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

	this->TargetLocation = this->PickNewTarget();
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

void ACatcher::MoveToTarget(const FVector& Target) const
{
	if (this->Movement == nullptr)
		return;

	const auto Position = this->GetActorLocation();
	const auto NextVelocity = this->GetNextVelocity(
		Position,
		Target,
		this->Movement->MaxSpeed,
		this->SlowMovementRange,
		this->StopMovementRange
	);

	if (NextVelocity.IsZero())
		return;

	this->Movement->AddInputVector(NextVelocity);
}

FVector ACatcher::PickNewTarget() const
{
	auto Position = this->GetActorLocation();

	Position.Y = UKismetMathLibrary::RandomFloatInRange(-2200, 2200);

	return Position;
}

void ACatcher::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->MoveToTarget(this->TargetLocation);
}

void ACatcher::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Fruit = reinterpret_cast<AFruit*>(OtherActor);

	// If not fruit, skip
	if (Fruit == nullptr)
		return;

	this->OnAttack();

	// Destroy fruit
	Fruit->Destroy();

	// Pick new target
	this->TargetLocation = this->PickNewTarget();
}

void ACatcher::OnAttack_Implementation() const
{
	// Spawn particles
	// Play attack animation
}
