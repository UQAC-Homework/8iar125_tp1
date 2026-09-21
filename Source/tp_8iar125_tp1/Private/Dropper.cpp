#include "Dropper.h"

#include "Kismet/KismetMathLibrary.h"

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

bool ADropper::MoveTo(const FVector& TargetPosition) const
{
	if (this->Movement == nullptr)
		return false;

	const auto CurrentPosition = this->GetActorLocation();
	const auto NextVelocity = this->GetNextVelocity(
		CurrentPosition,
		TargetPosition,
		this->Movement->MaxSpeed,
		this->SlowMovementRange,
		this->StopMovementRange
	);

	if (NextVelocity.IsZero())
		return false;

	this->Movement->AddInputVector(NextVelocity);
	return true;
}

void ADropper::ChangeTarget()
{
	constexpr auto MaxLeft = -2200;
	constexpr auto MaxRight = 2200;

	const auto MinimumDistance = UKismetMathLibrary::RandomFloatInRange(
		250,
		1500
	);

	this->TargetLocation = this->ComputeTarget(
		this->GetActorLocation(),
		MaxLeft,
		MaxRight,
		MinimumDistance
	);
}

FVector ADropper::ComputeTarget(FVector Position, const int MaxLeft, const int MaxRight, const int MinimumDistance)
{
	const auto SpaceLeft = abs(MaxLeft - Position.Y);

	if (SpaceLeft < MinimumDistance)
	{
		Position.Y = UKismetMathLibrary::RandomFloatInRange(Position.Y + MinimumDistance, MaxRight);
		return Position;
	}

	const auto SpaceRight = abs(MaxRight - Position.Y);

	if (SpaceRight < MinimumDistance)
	{
		Position.Y = UKismetMathLibrary::RandomFloatInRange(MaxLeft, Position.Y - MinimumDistance);
		return Position;
	}

	const auto RandomLeft = UKismetMathLibrary::RandomFloatInRange(MaxLeft, Position.Y - MinimumDistance);
	const auto RandomRight = UKismetMathLibrary::RandomFloatInRange(Position.Y + MinimumDistance, MaxRight);

	if (UKismetMathLibrary::RandomBool())
		Position.Y = RandomLeft;
	else
		Position.Y = RandomRight;

	return Position;
}

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

	this->ChangeTarget();
}

void ADropper::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// If not reached, move to target
	if (this->MoveTo(this->TargetLocation))
		return;

	// Spawn projectile
	this->Spawn();

	// Pick new target
	this->ChangeTarget();
}
