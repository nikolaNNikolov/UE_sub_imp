//Sub_Imp Plugin
#pragma once

#include "Sound/SoundWave.h"

static TWeakPtr<SWindow> CurrentSubImpWindow;

#define TIMESPAN_DELIMITER TEXT(":")
#define SUBTITLE_TIME_DELIMITER TEXT("-->")
#define OPEN_FILE_DIALOG_TITLE FString("Choose subtitles file")
#define OPEN_FILE_DIALOG_DEFAULT_FILE FString("")
#define OPEN_FILE_DIALOG_FILE_TYPES FString("Subtitles|*.srt")
#define OPEN_FILE_DIALOG_FLAGS 0

#define OPEN_FILE_HINT_STRING FString("Choose subtitles file...")

//TODO: Migrate these to static values, as well as srt and audio file, don't close sub-imp after use
#define END_SUB_DEFAULT_TAG FText::FromString("NO_SUB")
#define END_SUB_MINIMUM_TIMEOUT 3.0f



enum ESubImpLineType
{
	EmptyLine		= 0,
	SubtitleIndex	= 1,
	SubtitleTime	= 2,
	SubtitleText	= 3
};

class SUBIMP_API SSubImpWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSubImpWindow)
	{}
	SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
	
	SLATE_END_ARGS()
	
	SSubImpWindow();

	void Construct(const FArguments& InArgs);

protected:
	FString LoadedFileString;

	FReply OpenSRTFilePickerWindow();
	FReply DoTheSubImp();
	
	bool GetIsGenerateButtonEnabled() const;
	bool IsLoadedPathStringValid() const;
	
	FText GetLoadedFileStringAsText() const;
	FText GetLoadedFileTooltipText() const;

	void OnSoundWaveSelected(const FAssetData& InSoundWave);
	FString GetSelectedSoundWavePath() const;

	void OnStringTableSelected(const FAssetData& InStringTable);
	FString GetSelectedStringTablePath() const;

	void OnStringTableCheckboxStateChanged(ECheckBoxState bIsChecked);
	ECheckBoxState GetStringTableCheckBoxState() const;
	EVisibility GetStringTableSettingsVisibility() const;

	void OnApplyEndSubTagStatChanged(ECheckBoxState bIsChecked);
	ECheckBoxState GetApplyEndSubTagCheckBoxState() const;
	EVisibility GetEndSubTagSettingsVisibility() const;
	
	void EndSubTagTextBlockCommitted(const FText& Text, ETextCommit::Type CommitType);
	FText GetEndSubTag() const;
	
	void EndSubTagTimeoutNumericCommitted(float Value, ETextCommit::Type CommitType);
	float GetEndSubTagTimeout() const;

private:
	TSharedPtr<SWindow> ParentWindow;
	TArray<FSubtitleCue> GeneratedSubtitleInfo;
	
	USoundWave* SelectedSoundWave;
	FString SelectedSoundWavePath;

	bool bUseStringTableForSubtitles;
	UStringTable* SelectedStringTable;
	FString SelectedStringTablePath;

	bool bApplyEndSubTag;
	float EndSubTagTimeout;
	FText EndSubTag;

	void ResetSubImp();
	void GenerateSubtitleCueArrayFromReadFile();

	float GetTotalSecondsFromTimespanString(const FString& TimespanString) const;
	
};


#define WINDOW_SIZE FVector2D(350.0f, 550.0f)
#define MIN_MOUSE_OFFSET 15.0f

static void CloseSubImpWindow()
{
	if(CurrentSubImpWindow.IsValid())
	{
		CurrentSubImpWindow.Pin()->RequestDestroyWindow();
		CurrentSubImpWindow.Reset();
	}
}

static void OpenSubImpWindow(TSharedPtr<SWindow> ParentWindow)
{
	CloseSubImpWindow();

	const FVector2D CursorPosition = FSlateApplication::Get().GetCursorPos();
	const FSlateRect CursorAnchor (CursorPosition.X /*+ WINDOW_SIZE.X/2*/, CursorPosition.Y + MIN_MOUSE_OFFSET,
		CursorPosition.X /* + WINDOW_SIZE.X/2*/, CursorPosition.Y + MIN_MOUSE_OFFSET);
	const FVector2D SpawnLocation = FSlateApplication::Get().CalculatePopupWindowPosition(CursorAnchor, WINDOW_SIZE);

	TSharedPtr<SWindow> Window = SNew(SWindow)
		.ScreenPosition(SpawnLocation)
		.AutoCenter(EAutoCenter::None)
		.SupportsMaximize(false)
		.SupportsMinimize(true)
		.SizingRule(ESizingRule::Autosized)
		.ClientSize(WINDOW_SIZE)
		.HasCloseButton(true)
		.Title(FText::FromString("sub-imp"))
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
			.Padding(FMargin(8.0f))
			[
				SNew(SSubImpWindow)
				.ParentWindow(ParentWindow)
			]
		];

	if (ParentWindow.IsValid())
	{
		Window = FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), ParentWindow.ToSharedRef());
	}
	else
	{
		Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}
	
	CurrentSubImpWindow = Window;
	
}
