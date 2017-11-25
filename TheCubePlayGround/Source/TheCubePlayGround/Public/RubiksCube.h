// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "iTween/iTween.h"
#include "iTween/iTInterface.h"
#include "RubiksCube.generated.h"

UENUM(BlueprintType)
namespace ERotationGroup
{
	enum RotationGroup
	{
		X UMETA(DisplayName = "Pieces with same X"),
		Y UMETA(DisplayName = "Pieces with same Y"),
		Z UMETA(DisplayName = "Pieces with same Z")
	};
}


#define CUBE_EXTENT 94
#define CUBE_PIECE_TAG "CubePiece"

UCLASS(Blueprintable)
class THECUBEPLAYGROUND_API ARubiksCube : public AActor, public IiTInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray <class ARubiksPiece*> Pieces;

	UPROPERTY()
	TArray <class ARubiksPiece*> PiecesToRotate;

	UPROPERTY()
	FVector HitStartPosition;

	UPROPERTY()
	FVector HitStartNormal;

	UPROPERTY()
	class ARubiksPiece * HitStartPiece;
    
    UPROPERTY()
    bool bIsCubeAnimating;

	int32 ScrambleCounter;

	int32 StepsCounter;

	bool bIsScrambling;

	void Scramble();

public:	
	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ARubiksPiece> PieceClass;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
	int32 CubeSize;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
	float CubeExtentScale;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
	float CubeRotationSpeed;

	UPROPERTY(Category = Rubiks, VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent * DummyRoot;

	UPROPERTY(Category = Rubiks, VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent * PieceRotator;

	// Sets default values for this actor's properties
	ARubiksCube();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	void DestroyCube();

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	void BuildCube(int32 size);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	void Scramble(int32 steps);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	int32 GetSteps();

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	bool IsCubeSolved();

	//Rotate Cube
	UFUNCTION(Category = Rubiks, BlueprintCallable)
	int32 RotateFromPieceClockwise(FVector normal, class ARubiksPiece * piece);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
	int32 RotateFromPieceCounterClockwise(FVector normal, class ARubiksPiece * piece);


	//void RotateFromPiece(FVector normal, FVector direction, class ARubiksPiece * piece);

	void RotateGroup(FName name, class ARubiksPiece * piece, ERotationGroup::RotationGroup groupAxis, FRotator rotation, float speed = 0.3f);

    virtual void OnTweenComplete_Implementation(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, FHitResult sweepHitResultForMoveEvents, bool successfulTransform) override;

    virtual void OnTweenStart_Implementation(AiTweenEvent* eventOperator, AActor* actorTweening = nullptr, USceneComponent* componentTweening = nullptr, UWidget* widgetTweening = nullptr, FName tweenName = "") override;
};
