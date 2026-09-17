#include "ScoreSystem.h"


UScoreSystem::UScoreSystem()
{
	this->CaughtAmount = 0;
	this->TotalAmount = 0;
}

void UScoreSystem::NotifyRecord() const
{
	if (!this->OnScoreChanged.IsBound())
		return;

	this->OnScoreChanged.Broadcast(
		this->CaughtAmount,
		this->TotalAmount
	);
}

void UScoreSystem::RecordMiss()
{
	this->TotalAmount++;
	this->NotifyRecord();
}

void UScoreSystem::RecordCapture()
{
	this->CaughtAmount++;
	this->TotalAmount++;
	this->NotifyRecord();
}
