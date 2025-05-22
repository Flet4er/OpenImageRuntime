// Copyright (c) Blizickij Vladimir. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImportImageLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DND_PROJECT_API UImportImageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static UTexture2D* OpenFileDialogueAndLoadImage();
};
