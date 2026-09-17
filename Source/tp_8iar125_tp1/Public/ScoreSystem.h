#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ScoreSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, Current, int32, Total);

UCLASS()
class TP_8IAR125_TP1_API UScoreSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	int CaughtAmount;
	int TotalAmount;

	UPROPERTY(BlueprintAssignable, Category="Score")
	FOnScoreChanged OnScoreChanged;

	/// Notifies that a new record has occured
	void NotifyRecord() const;

public:
	UScoreSystem();


	/// Records that a miss occured
	void RecordMiss();

	/// Records that a capture occured
	void RecordCapture();
};
