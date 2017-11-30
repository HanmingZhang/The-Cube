// Fill out your copyright notice in the Description page of Project Settings.

#include "RubiksCube.h"
#include "TheCubePlayGround.h"
#include "RubiksPiece.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARubiksCube::ARubiksCube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Set default cube size
	this->CubeSize = 3;

	//this->HitStartPiece = nullptr;
	//this->HitStartPosition = FVector::ZeroVector;
	//this->HitStartNormal = FVector::ZeroVector;

	//Creates the piece rotator
	DummyRoot = CreateDefaultSubobject<USceneComponent>(FName("Dummy Root"));
	SetRootComponent(DummyRoot);

	PieceRotator = CreateDefaultSubobject<USceneComponent>(FName("Piece Rotator"));

	PieceRotator->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);


	SelfRotator = CreateDefaultSubobject<USceneComponent>(FName("Self Rotator"));

	SelfRotator->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);


	this->CubeExtentScale = 1.0;
	this->totalRotationTime = 1.0;
	passRotationTime = 0.0;
	isRotating = false;
	destRotation = FRotator(0, 0, 0);
}

// Called when the game starts or when spawned
void ARubiksCube::BeginPlay()
{
	Super::BeginPlay();

	//Build Cube Pieces
	this->BuildCube(this->CubeSize);

}

// Called every frame
void ARubiksCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (isRotating) {
		passRotationTime += DeltaTime;
		PieceRotator->SetRelativeRotation((passRotationTime / totalRotationTime)  * destRotation);

		if (passRotationTime >= totalRotationTime) {
			isRotating = false;
			passRotationTime = 0.0;
			destRotation = FRotator(0, 0, 0);
		}
	}

}

void ARubiksCube::DestroyCube()
{
	PiecesToRotate.Empty();
	for (int32 x = 0; x < Pieces.Num(); x++) {
		Pieces[x]->Destroy();
	}

	Pieces.Empty();
	PieceRotator->SetRelativeRotation(FRotator(0, 0, 0));
	SelfRotator->SetRelativeRotation(FRotator(0, 0, 0));
}

void ARubiksCube::BuildCube(int32 size)
{
	//Set new cube size
	this->CubeSize = size;
	UWorld * gWorld = GetWorld();


	//Set PieceRotator to the center of the new cube
	float centerOffset = (CUBE_EXTENT * this->CubeExtentScale * (this->CubeSize - 1)) / 2;

	PieceRotator->SetRelativeLocation(FVector(centerOffset, centerOffset, centerOffset));
	SelfRotator->SetRelativeLocation(FVector(centerOffset, centerOffset, centerOffset));


	int32 cubePieceID = 1;

	UE_LOG(LogTemp, Warning, TEXT("Cube Creation!"));
	//Create cube based on its size
	for (int i = 0; i < this->CubeSize; i++) {
		for (int j = 0; j < this->CubeSize; j++) {
			for (int k = 0; k < this->CubeSize; k++) {
				//Spawn only pieces that belongs to a wall
				if (gWorld && (i == 0 || i == this->CubeSize - 1 || j == 0 || j == this->CubeSize - 1 || k == 0 || k == this->CubeSize - 1)) {
					//Spawn a rubiks piece
					FActorSpawnParameters params;
					params.Owner = this;
					ARubiksPiece * piece = gWorld->SpawnActor<ARubiksPiece>(PieceClass, FVector(CUBE_EXTENT * this->CubeExtentScale * j, CUBE_EXTENT * this->CubeExtentScale * i, CUBE_EXTENT * this->CubeExtentScale * k), FRotator(0.0f, 0.0f, 0.0f), params);
					//piece->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepRelativeOffset, false);
					piece->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
					piece->Tags.Add(CUBE_PIECE_TAG);
					piece->cubePieceID = cubePieceID++;
					Pieces.Add(piece);
				}
			}
		}
	}
}

void ARubiksCube::Scramble()
{
	if (this->isRotating) {
		return;
	}

	//Choose a random group based on a axis
	int random = FMath::RandRange(0, 2);
	ERotationGroup::RotationGroup rotationGroupAxis = ERotationGroup::X;;
	FRotator angle = FRotator(0, 0, 90);
	switch (random)
	{
	case 0:
		rotationGroupAxis = ERotationGroup::X;
		angle = FRotator(0, 0, 90);
		break;
	case 1:
		rotationGroupAxis = ERotationGroup::Y;
		angle = FRotator(90, 0, 0);
		break;
	case 2:
		rotationGroupAxis = ERotationGroup::Z;
		angle = FRotator(0, 90, 0);
		break;
	default:
		break;
	}

	//Choose a random piece for the group
	random = FMath::RandRange(0, Pieces.Num() - 1);
	ARubiksPiece * randomPiece = Pieces[random];

	//Choose a random direction
	random = FMath::RandRange(0, 1);
	if (random) {
		angle *= -1;
	}

	//Scramble!
	RotateGroup(FName("Scramble"), randomPiece, rotationGroupAxis, angle);
}


bool ARubiksCube::IsCubeSolved()
{
	bool solved = true;
	for (int x = 0; x < Pieces.Num(); x++) {
		if (!Pieces[x]->IsAtStartPosition()) {
			solved = false;
			break;
		}
	}

	return solved;
}


int32 ARubiksCube::RotateFromPieceClockwise(FVector normal, class ARubiksPiece * piece) {
	if (this->isRotating) {
		return -1;
	}

	UE_LOG(LogActor, Warning, TEXT("Rotate From Piece Clockwise!"));
	if (normal.Equals(FVector(0, 0, 1))) { //Top Face
		UE_LOG(LogActor, Warning, TEXT("Top face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));

		return 1;
	}
	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));

		return 2;
	}
	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
		UE_LOG(LogActor, Warning, TEXT("Back face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));

		return 3;
	}
	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
		UE_LOG(LogActor, Warning, TEXT("Front face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));

		return 4;
	}
	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
		UE_LOG(LogActor, Warning, TEXT("Right face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));

		return 5;
	}
	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
		UE_LOG(LogActor, Warning, TEXT("Left face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));

		return 6;
	}

	return -1;
}


int32 ARubiksCube::RotateFromPieceCounterClockwise(FVector normal, class ARubiksPiece * piece) {
	if (this->isRotating) {
		return -1;
	}

	UE_LOG(LogActor, Warning, TEXT("Rotate From Piece Clockwise!"));
	if (normal.Equals(FVector(0, 0, 1))) { //Top Face
		UE_LOG(LogActor, Warning, TEXT("Top face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));

		return 1;
	}
	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));

		return 2;
	}
	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
		UE_LOG(LogActor, Warning, TEXT("Back face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));

		return 3;
	}
	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
		UE_LOG(LogActor, Warning, TEXT("Front face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));

		return 4;
	}
	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
		UE_LOG(LogActor, Warning, TEXT("Right face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));

		return 5;
	}
	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
		UE_LOG(LogActor, Warning, TEXT("Left face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));

		return 6;
	}

	return -1;
}



void ARubiksCube::RotateGroup(FName tweenName, class ARubiksPiece* piece, ERotationGroup::RotationGroup groupAxis, FRotator rotation)
{
	//Clean array of the pieces that will rotate
	PiecesToRotate.Empty();

	//Remove parent from all pieces
	for (int32 x = 0; x < Pieces.Num(); x++) {
		Pieces[x]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	//Reset rotation from PieceRotator
	PieceRotator->SetRelativeRotation(FRotator(0, 0, 0));


	//Add all pieces from the same group as the given piece to the PiecesToRotate array (used fabs just to prevent possible minimal erros)
	if (groupAxis == ERotationGroup::RotationGroup::X) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).X -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).X) < CUBE_ROTATE_THRESHOLD) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}
	else if (groupAxis == ERotationGroup::RotationGroup::Y) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Y -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Y) < CUBE_ROTATE_THRESHOLD) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}
	else if (groupAxis == ERotationGroup::RotationGroup::Z) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Z -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Z) < CUBE_ROTATE_THRESHOLD) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}

	for (int32 x = 0; x < Pieces.Num(); x++) {
		Pieces[x]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}


	//Set all the pieces to rotate as child of the PieceRotator
	for (int32 x = 0; x < PiecesToRotate.Num(); x++) {
		PiecesToRotate[x]->AttachToComponent(PieceRotator, FAttachmentTransformRules::KeepWorldTransform);
	}

	UE_LOG(LogActor, Warning, TEXT("%d pieces found."), PiecesToRotate.Num());

	// start Rotation
	destRotation = rotation;
	isRotating = true;
}



ARubiksPiece* ARubiksCube::getCubePieceByID(int32 inputID) {
	if (inputID < 1 || inputID > 8) {
		UE_LOG(LogActor, Warning, TEXT("please input a valid inputID (should be 1 - 8 in 2x2x2 case)!"));
		return NULL;
	}

	for (ARubiksPiece* each : Pieces) {
		if (each->cubePieceID == inputID) {
			UE_LOG(LogActor, Warning, TEXT("cube piece %d return !"), inputID);
			return each;
		}
	}

	UE_LOG(LogActor, Warning, TEXT("can't find cube piece!"));
	return NULL;
}


TArray <class ARubiksPiece*>  ARubiksCube::TargetPiecesToRotateGroup(FVector normal, class ARubiksPiece * piece) {

	PotentialPiecesToRotateGroup.Empty();

	if (piece == NULL) {
		UE_LOG(LogActor, Warning, TEXT("input piece is NULL!"));
		return PotentialPiecesToRotateGroup;
	}

	ERotationGroup::RotationGroup potentialRotationGroup;

	if (normal.Equals(FVector(0, 0, 1))) { //Top Face
		UE_LOG(LogActor, Warning, TEXT("Top face!"));
		
		potentialRotationGroup = ERotationGroup::RotationGroup::Z;
	}
	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));

		potentialRotationGroup = ERotationGroup::RotationGroup::Z;

	}
	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
		UE_LOG(LogActor, Warning, TEXT("Back face!"));

		potentialRotationGroup = ERotationGroup::RotationGroup::X;

	}
	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
		UE_LOG(LogActor, Warning, TEXT("Front face!"));

		potentialRotationGroup = ERotationGroup::RotationGroup::X;

	}
	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
		UE_LOG(LogActor, Warning, TEXT("Right face!"));

		potentialRotationGroup = ERotationGroup::RotationGroup::Y;

	}
	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
		UE_LOG(LogActor, Warning, TEXT("Left face!"));

		potentialRotationGroup = ERotationGroup::RotationGroup::Y;

	}


	//Add all pieces from the same group as the given piece to the PiecesToRotate array (used fabs just to prevent possible minimal erros)
	if (potentialRotationGroup == ERotationGroup::RotationGroup::X) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).X -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).X) < CUBE_ROTATE_THRESHOLD) {
				PotentialPiecesToRotateGroup.Add(Pieces[x]);
			}
		}
	}
	else if (potentialRotationGroup == ERotationGroup::RotationGroup::Y) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Y -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Y) < CUBE_ROTATE_THRESHOLD) {
				PotentialPiecesToRotateGroup.Add(Pieces[x]);
			}
		}
	}

	else if (potentialRotationGroup == ERotationGroup::RotationGroup::Z) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Z -
				this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Z) < CUBE_ROTATE_THRESHOLD) {
				PotentialPiecesToRotateGroup.Add(Pieces[x]);
			}
		}
	}

	return PotentialPiecesToRotateGroup;
}

//void ARubiksCube::RotateWholeCube() {
//
//	if (SelfRotator == NULL) {
//		return;
//	}
//
//	for (int32 x = 0; x < Pieces.Num(); x++) {
//		Pieces[x]->AttachToComponent(SelfRotator, FAttachmentTransformRules::KeepWorldTransform);
//	}
//
//	SelfRotator->SetRelativeRotation(FRotator(0.0, 0.0, 90.0));
//}