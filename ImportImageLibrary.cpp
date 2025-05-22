// Copyright (c) Blizickij Vladimir. All rights reserved

#include "ImportImageLibrary.h"
#include "DesktopPlatformModule.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "Misc/Paths.h"


UTexture2D* UImportImageLibrary::OpenFileDialogueAndLoadImage()
{

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	if (DesktopPlatform)
	{

		TArray<FString> OutFilenames;
		bool bOpened = DesktopPlatform->OpenFileDialog
		(
			nullptr,
			TEXT("Choose Image"),
			FPaths::ProjectContentDir(),
			TEXT(""),
			TEXT("Image Files (*.png; *.jpg| *.png;*.jpg)"),
			EFileDialogFlags::None,
			OutFilenames
		);

		if (bOpened && OutFilenames.Num() > 0)
		{

			FString SelectedImagePath = OutFilenames[0];

			TArray <uint8> ImageData;

			if (FFileHelper::LoadFileToArray(ImageData, *SelectedImagePath))
			{
				IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

				EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());

				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
				{

					TArray64<uint8> RawData;

					if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, RawData))
					{
						TArray64<uint8>* RawDataProcessed = &RawData;
						UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);

						if (Texture)
						{

							FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
							void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

							FMemory::Memcpy(Data, RawDataProcessed->GetData(), RawDataProcessed->Num());
							Mip.BulkData.Unlock();

							Texture->GetPlatformData()->SetNumSlices(1);

							Texture->NeverStream = true;

							Texture->UpdateResource();

							return Texture;
						}
					}
				}
			}
		}
	}
	return nullptr;
}