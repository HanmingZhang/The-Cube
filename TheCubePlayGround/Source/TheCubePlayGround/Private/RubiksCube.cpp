// Fill out your copyright notice in the Description page of Project Settings.

#include "RubiksCube.h"
#include "TheCubePlayGround.h"
#include "RubiksPiece.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "iTween/iTween.h"

// Sets default values
ARubiksCube::ARubiksCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->ScrambleCounter = 10;
	this->StepsCounter = 0;

	this->bIsScrambling = false;

	//Set default cube size
	this->CubeSize = 3;


	this->HitStartPiece = nullptr;
	this->HitStartPosition = FVector::ZeroVector;
	this->HitStartNormal = FVector::ZeroVector;  

	//Creates the piece rotator
	DummyRoot = CreateDefaultSubobject<USceneComponent>(FName("Dummy Root"));
	SetRootComponent(DummyRoot);

	PieceRotator = CreateDefaultSubobject<USceneComponent>(FName("Piece Rotator"));
	PieceRotator->AttachTo(GetRootComponent());
	
	this->CubeExtentScale = 1.0;
	this->CubeRotationSpeed = 0.3;
}

// Called when the game starts or when spawned
void ARubiksCube::BeginPlay()
{
	Super::BeginPlay();

	//Build Cube Pieces
	this->BuildCube(this->CubeSize);
	
	//this->SetActorRotation(FRotator(30, 30, 30));
	//this->SetActorLocation(FVector(-320, 400, 400));
}

// Called every frame
void ARubiksCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bIsScrambling) {
		return;
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
}

void ARubiksCube::BuildCube(int32 size)
{
	//Prevent to build a new cube if it is scrambling
	if (this->bIsScrambling) {
		return;
	}

	//Set new cube size
	this->StepsCounter = 0;
	this->CubeSize = size;
	UWorld * gWorld = GetWorld();


	//Set PieceRotator to the center of the new cube
	float centerOffset = (CUBE_EXTENT * this->CubeExtentScale * (this->CubeSize - 1)) / 2;
	PieceRotator->SetRelativeLocation(FVector(centerOffset, centerOffset, centerOffset));


	UE_LOG(LogTemp, Warning, TEXT("Cube Creation!"));
	//Create cube based on its size
	for (int i = 0; i < this->CubeSize; i++) {
		for (int j = 0; j < this->CubeSize; j++) {
			for (int k = 0; k < this->CubeSize; k++) {
				//Spawn only pieces that belongs to a wall
				if (gWorld && (i == 0 || i == this->CubeSize-1 || j == 0 || j == this->CubeSize-1 || k ==0 || k == this->CubeSize-1)) { 
					//Spawn a rubiks piece
					FActorSpawnParameters params;
					params.Owner = this;
					ARubiksPiece * piece = gWorld->SpawnActor<ARubiksPiece>(PieceClass, FVector(CUBE_EXTENT * this->CubeExtentScale * j, CUBE_EXTENT * this->CubeExtentScale * i, CUBE_EXTENT * this->CubeExtentScale * k), FRotator(0.0f, 0.0f, 0.0f), params);
					piece->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepRelativeOffset, false);
					piece->Tags.Add(CUBE_PIECE_TAG);
					Pieces.Add(piece);
				}
			}
		}
	}
}

void ARubiksCube::Scramble()
{
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
	RotateGroup(FName("Scramble"), randomPiece, rotationGroupAxis, angle, this->CubeRotationSpeed);
}

void ARubiksCube::Scramble(int32 steps)
{
	//Not scramble if it is already scrambling
	if (this->bIsScrambling) {
		return;
	}

	//Start scramble chain
	this->ScrambleCounter = steps;
	this->bIsScrambling = true;
	this->StepsCounter = 0;
	this->Scramble();
}

int32 ARubiksCube::GetSteps()
{
	return this->StepsCounter;
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
	if (this->bIsCubeAnimating) {
		return -1;
	}
	UE_LOG(LogActor, Warning, TEXT("Rotate From Piece Clockwise!"));
	if (normal.Equals(FVector(0, 0, 1))) { //Top Face
		UE_LOG(LogActor, Warning, TEXT("Top face!"));
		
		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0,90, 0), this->CubeRotationSpeed);
		
		return 1;
	}
	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0), this->CubeRotationSpeed);
	
		return 2;
	}
	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
		UE_LOG(LogActor, Warning, TEXT("Back face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90), this->CubeRotationSpeed);
	
		return 3;
	}
	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
		UE_LOG(LogActor, Warning, TEXT("Front face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90), this->CubeRotationSpeed);
		
		return 4;
	}
	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
		UE_LOG(LogActor, Warning, TEXT("Right face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0), this->CubeRotationSpeed);
		
		return 5;
	}
	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
		UE_LOG(LogActor, Warning, TEXT("Left face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0), this->CubeRotationSpeed);
		
		return 6;
	}

	return -1;
}


int32 ARubiksCube::RotateFromPieceCounterClockwise(FVector normal, class ARubiksPiece * piece) {
	if (this->bIsCubeAnimating) {
		return -1;
	}
	UE_LOG(LogActor, Warning, TEXT("Rotate From Piece Clockwise!"));
	if (normal.Equals(FVector(0, 0, 1))) { //Top Face
		UE_LOG(LogActor, Warning, TEXT("Top face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0), this->CubeRotationSpeed);
	
		return 1;
	}
	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0), this->CubeRotationSpeed);
		
		return 2;
	}
	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
		UE_LOG(LogActor, Warning, TEXT("Back face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90), this->CubeRotationSpeed);
	
		return 3;
	}
	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
		UE_LOG(LogActor, Warning, TEXT("Front face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90), this->CubeRotationSpeed);
	
		return 4;
	}
	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
		UE_LOG(LogActor, Warning, TEXT("Right face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0), this->CubeRotationSpeed);
		
		return 5;
	}
	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
		UE_LOG(LogActor, Warning, TEXT("Left face!"));

		RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0), this->CubeRotationSpeed);
	
		return 6;
	}

	return -1;
}



//void ARubiksCube::RotateFromPiece(FVector normal, FVector direction, class ARubiksPiece * piece)
//{	
//
//	if (this->bIsCubeAnimating) {
//		return;
//	}
//
//    UE_LOG(LogActor, Warning, TEXT("Rotate Group!"));
//	if (normal.Equals(FVector(0,0,1))) { //Top Face
//		UE_LOG(LogActor, Warning, TEXT("Top face!"));
//		if (direction.X > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));
//		}
//		else if (direction.X < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));
//		}
//		else if (direction.Y > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));
//		}
//		else if (direction.Y < -0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));
//		}
//	}
//	else if (normal.Equals(FVector(0, 0, -1))) { //Bottom Face
//		UE_LOG(LogActor, Warning, TEXT("Bottom face!"));
//        if (direction.X > 0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));
//        }
//        else if (direction.X < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));
//        }
//        else if (direction.Y > 0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));
//        }
//        else if (direction.Y < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));
//		}
//	}
//	else if (normal.Equals(FVector(1, 0, 0))) { //Back face
//        UE_LOG(LogActor, Warning, TEXT("Back face!"));
//        if (direction.Z > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));
//        }
//        else if (direction.Z < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));
//        }
//        else if (direction.Y > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));
//        }
//        else if (direction.Y < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));
//        }
//	}
//	else if (normal.Equals(FVector(-1, 0, 0))) { //Front Face
//		UE_LOG(LogActor, Warning, TEXT("Front face!"));
//        if (direction.Z > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(-90, 0, 0));
//        }
//        else if (direction.Z < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Y, FRotator(90, 0, 0));
//        }
//        else if (direction.Y > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));
//        }
//        else if (direction.Y < -0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));
//        }
//	}
//	else if (normal.Equals(FVector(0, -1, 0))) { //Right Face
//		UE_LOG(LogActor, Warning, TEXT("Right face!"));
//        if (direction.Z > 0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));
//        }
//        else if (direction.Z < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));
//        }
//        else if (direction.X > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));
//        }
//        else if (direction.X < -0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));
//        }
//	}
//	else if (normal.Equals(FVector(0, 1, 0))) { //Left Face
//		UE_LOG(LogActor, Warning, TEXT("Left face!"));
//        if (direction.Z > 0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, -90));
//        }
//        else if (direction.Z < -0.5f) {
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::X, FRotator(0, 0, 90));
//        }
//        else if (direction.X > 0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, -90, 0));
//        }
//        else if (direction.X < -0.5f) { 
//			RotateGroup(FName("Group Rotation"), piece, ERotationGroup::RotationGroup::Z, FRotator(0, 90, 0));
//        }
//	}
//    
//}

void ARubiksCube::RotateGroup(FName tweenName, class ARubiksPiece* piece, ERotationGroup::RotationGroup groupAxis, FRotator rotation, float speed)
{	
	//Clean array of the pieces that will rotate
	PiecesToRotate.Empty();

	//Remove parent from all pieces
	for (int32 x = 0; x < Pieces.Num(); x++) {
		Pieces[x]->DetachRootComponentFromParent(true);
	}

	//Reset rotation from PieceRotator
	PieceRotator->SetRelativeRotation(FRotator(0, 0, 0));

	//Add all pieces from the same group as the given piece to the PiecesToRotate array (used fabs just to prevent possible minimal erros)
	if (groupAxis == ERotationGroup::RotationGroup::X) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).X - 
					 this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).X) < 0.2) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}
	else if (groupAxis == ERotationGroup::RotationGroup::Y) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Y -
				     this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Y) < 0.2) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}
	else if (groupAxis == ERotationGroup::RotationGroup::Z) {
		for (int32 x = 0; x < Pieces.Num(); x++) {
			if (fabs(this->GetTransform().InverseTransformPosition(Pieces[x]->GetActorLocation()).Z -
				     this->GetTransform().InverseTransformPosition(piece->GetActorLocation()).Z) < 0.2) {
				PiecesToRotate.Add(Pieces[x]);
			}
		}
	}

	for (int32 x = 0; x < Pieces.Num(); x++) {
		Pieces[x]->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition, false);
	}


	//Set all the pieces to rotate as child of the PieceRotator
	for (int32 x = 0; x < PiecesToRotate.Num(); x++) {
		PiecesToRotate[x]->AttachRootComponentTo(PieceRotator, NAME_None, EAttachLocation::KeepWorldPosition);
	}


	//Rotate PieceRotator
	UiTween::ComponentRotateFromToSimple(tweenName, PieceRotator, this->GetActorRotation(), rotation, ECoordinateSpace::self, true, speed, EEaseType::linear, "", this, nullptr, this);
}

void ARubiksCube::OnTweenComplete_Implementation(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, FHitResult sweepHitResultForMoveEvents, bool successfulTransform)
{
	if (tweenName == FName("Group Rotation")) {
		UE_LOG(LogActor, Warning, TEXT("Finished Rotation"));

		this->bIsCubeAnimating = false;
	}
	else if (tweenName == FName("Scramble")) {
		UE_LOG(LogActor, Warning, TEXT("Scrambling"));
		ScrambleCounter--;
		if (ScrambleCounter > 0) {
			this->Scramble();
		}
		else {
			this->bIsScrambling = false;
			this->bIsCubeAnimating = false;
		}
	}
}

void ARubiksCube::OnTweenStart_Implementation(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName)
{
    UE_LOG(LogActor, Warning, TEXT("Started Rotation!"));

	this->bIsCubeAnimating = true;
	this->HitStartPiece = nullptr;
	this->HitStartNormal = FVector::ZeroVector;
	this->HitStartPosition = FVector::ZeroVector;
}

