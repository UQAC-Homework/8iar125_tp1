#include "ScoreSystem.h"

#include "Kismet/GameplayStatics.h"

AScoreSystem::AScoreSystem()
{
	this->CaughtAmount = 0;
	this->TotalAmount = 0;
}

void AScoreSystem::NotifyRecord() const
{
	if (!this->OnScoreChanged.IsBound())
		return;

	this->OnScoreChanged.Broadcast(
		this->CaughtAmount,
		this->TotalAmount
	);
}

void AScoreSystem::RecordMiss()
{
	this->TotalAmount++;
	this->NotifyRecord();
}

void AScoreSystem::RecordCapture()
{
	this->CaughtAmount++;
	this->TotalAmount++;
	this->NotifyRecord();
}

AScoreSystem* AScoreSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
		return nullptr;

	const auto World = WorldContextObject->GetWorld();

	if (World == nullptr)
		return nullptr;

	return Cast<AScoreSystem>(UGameplayStatics::GetGameState(WorldContextObject));
}
