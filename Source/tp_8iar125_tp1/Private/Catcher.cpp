#include "Catcher.h"

#include "Fruit.h"
#include "Kismet/GameplayStatics.h"
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
	const auto World = this->GetWorld();
	FVector Position = this->GetActorLocation();

	if (World == nullptr)
		return Position;

	TArray<AActor*> InstancesFound;
	UGameplayStatics::GetAllActorsOfClass(
		World,
		AFruit::StaticClass(),
		InstancesFound
	);

	const AFruit* ClosestFruit = nullptr;
	float ClosestDistance = FLT_MAX;

	for (const auto Instance : InstancesFound)
	{
		const auto Fruit = Cast<AFruit>(Instance);

		if (Fruit == nullptr)
			continue;

		const auto Distance = FVector::Dist2D(
			Position,
			Fruit->GetActorLocation()
		);


		if (Distance >= ClosestDistance)
			continue;

		ClosestDistance = Distance;
		ClosestFruit = Fruit;
	}

	if (ClosestFruit == nullptr)
		return FVector::ZeroVector;

	Position.Y = ClosestFruit->GetActorLocation().Y;

	return Position;
}

void ACatcher::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	this->TargetLocation = this->ComputeTarget();
}

void ACatcher::OnAttack_Implementation() const
{
	// Spawn particles
	// Play attack animation
}
