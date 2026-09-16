#include "Dropper.h"

#include "Kismet/KismetMathLibrary.h"

ADropper::ADropper()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = this->CreateDefaultSubobject<USceneComponent>("Root");

	this->SpawnItem = nullptr;
	this->SpawnOffset = FVector::ZeroVector;

	this->TargetLocation = FVector::ZeroVector;
	this->SlowMovementRange = 1000;
	this->StopMovementRange = 20;
	this->Movement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
	this->Movement->UpdatedComponent = RootComponent;
}

void ADropper::BeginPlay()
{
	Super::BeginPlay();

	this->TargetLocation = this->PickNewTarget();
}

void ADropper::Spawn() const
{
	if (this->SpawnItem == nullptr)
		return;

	const auto World = this->GetWorld();

	if (World == nullptr)
		return;

	const FVector SpawnLocation = this->GetActorLocation() + this->SpawnOffset;
	const FRotator SpawnRotation = this->GetActorRotation();
	const FActorSpawnParameters SpawnParams;

	World->SpawnActor<AActor>(
		this->SpawnItem,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}

FVector ADropper::GetNextVelocity(
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

bool ADropper::MoveToTarget(const FVector& Target) const
{
	if (this->Movement == nullptr)
		return false;

	const auto Position = this->GetActorLocation();
	const auto NextVelocity = this->GetNextVelocity(
		Position,
		Target,
		this->Movement->MaxSpeed,
		this->SlowMovementRange, 
		this->StopMovementRange
	);

	if (NextVelocity.IsZero())
		return false;

	this->Movement->AddInputVector(NextVelocity);
	return true;
}

FVector ADropper::PickNewTarget() const
{
	auto Position = this->GetActorLocation();

	Position.Y = UKismetMathLibrary::RandomFloatInRange(-2200, 2200);

	return Position;
}

void ADropper::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// If not reached, move to target
	if (this->MoveToTarget(this->TargetLocation))
		return;

	// Spawn projectile
	this->Spawn();

	// Pick new target
	this->TargetLocation = this->PickNewTarget();
}
