// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RubiksPiece.generated.h"

UCLASS()
class THECUBEPLAYGROUND_API ARubiksPiece : public AActor
{
	GENERATED_BODY()


private:

	FVector StartPosition;
	
public:	

	// Sets default values for this actor's properties
	ARubiksPiece();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Check if ht piece is at its start position
	bool IsAtStartPosition();
	
};
