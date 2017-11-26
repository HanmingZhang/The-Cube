// Fill out your copyright notice in the Description page of Project Settings.

#include "RubiksPiece.h"
#include "TheCubePlayGround.h"


// Sets default values
ARubiksPiece::ARubiksPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARubiksPiece::BeginPlay()
{
	Super::BeginPlay();
	this->StartPosition = this->GetActorLocation();
}

// Called every frame
void ARubiksPiece::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

//Check if ht piece is at its start position
bool ARubiksPiece::IsAtStartPosition()
{
	if (this->StartPosition.Equals(this->GetActorLocation(), .01f)) { //Use a small tolerance because we move/rotate the pieces too many times
		return true;
	}
	return false;
}

