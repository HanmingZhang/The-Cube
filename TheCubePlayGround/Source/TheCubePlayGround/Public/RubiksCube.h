// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
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
#define CUBE_ROTATE_THRESHOLD 15.0


UCLASS(Blueprintable)
class THECUBEPLAYGROUND_API ARubiksCube : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
		TArray <class ARubiksPiece*> Pieces;

	UPROPERTY()
		TArray <class ARubiksPiece*> PiecesToRotate;

	UPROPERTY()
		TArray <class ARubiksPiece*> PotentialPiecesToRotateGroup;

	float passRotationTime;

	bool isRotating;

	FRotator destRotation;

	void RotateGroup(FName name, class ARubiksPiece * piece, ERotationGroup::RotationGroup groupAxis, FRotator rotation);


	ERotationGroup::RotationGroup potentialRotationGroup;
	FRotator potentialRotator;
	class ARubiksPiece * potentialPiece;

public:
	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ARubiksPiece> PieceClass;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
		int32 CubeSize;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
		float CubeExtentScale;

	UPROPERTY(Category = Rubiks, EditAnywhere, BlueprintReadWrite)
		float totalRotationTime;

	UPROPERTY(Category = Rubiks, VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent * DummyRoot;

	UPROPERTY(Category = Rubiks, VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent * PieceRotator;


	UPROPERTY(Category = Rubiks, VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent * SelfRotator;


	// Sets default values for this actor's properties
	ARubiksCube();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		void DestroyCube();

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		void BuildCube(int32 size);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		void Scramble();

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		bool IsCubeSolved();

	

	// --------------------- Used in project -------------------------------------------

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		ARubiksPiece* getCubePieceByID(int32 inputID);

	//Rotate Cube
	UFUNCTION(Category = Rubiks, BlueprintCallable)
		int32 RotateFromPieceClockwise(FVector normal, class ARubiksPiece * piece);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		int32 RotateFromPieceCounterClockwise(FVector normal, class ARubiksPiece * piece);

	//Return target potential cube's pieces group
	UFUNCTION(Category = Rubiks, BlueprintCallable)
		TArray <class ARubiksPiece*> TargetPiecesToRotateGroup(FVector normal, class ARubiksPiece * piece);

	//Return target potential cube's pieces group
	UFUNCTION(Category = Rubiks, BlueprintCallable)
		void RotateWholeCube(ERotationGroup::RotationGroup directionGroup, int clockWise);


	//--------- Use these three together! ---------
	//Collision Detection during the rotation process
	UFUNCTION(Category = Rubiks, BlueprintCallable)
		int32 RotateFromPieceClockwiseWithCollisionDetection(FVector normal, class ARubiksPiece * piece);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		int32 RotateFromPieceCounterClockwiseWithCollisionDetection(FVector normal, class ARubiksPiece * piece);

	UFUNCTION(Category = Rubiks, BlueprintCallable)
		void RotateFromPieceDoRotation();

};
