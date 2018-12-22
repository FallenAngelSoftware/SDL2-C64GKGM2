#ifndef LOGIC
#define LOGIC

class Logic
{
public:

	Logic(void);
	virtual ~Logic(void);

    int ScrollNumberMoved;
    int ScrollSpeed;

    int CommandDisplayStartIndex;
    int CommandDisplayEndIndex;

    int CommandSelectedByMouseOld;
    int CommandSelectedByMouse;

    #define NumberOfCodes     65000
    struct Code
    {
        int CodeCommandLineNumber;
        int CodeCommandIndex;
        bool CodeCommandLineActive;

    } Codes[NumberOfCodes];

    int CurrentCodeLine;

    int CodeLastLine;

    int CodeDisplayStartIndex;
    int CodeDisplayEndIndex;

    int CodeSelectedByMouseOld;
    int CodeSelectedByMouse;

    int CodeSelectedForEdit;
    int CodeSelectorSelected;

    int CodeSelectedForLineNumberEdit;

    int CommandBoxMaxY;
    int CodeBoxMaxY;

    int CodeBoxOffsetY;

    void CalculateCodeLastLine(void);

    bool ThereIsCodeAfterThisLine(int line);
    void ShowHideCodeSelectLineNumberBoxes(void);

    #define DialogNothing               0
    #define DialogLineNumberSelect      1
    #define DialogClearCode             2
    #define DialogFindLineNumber        3
    int DialogToShow;
    int DialogToShowOld;

    int LineNumberArray[3];

    bool ThereAreLineNumbers(void);

    void ClearAllCodeSelectors(void);

    void SetupClickableButtons(void);
    void CheckForClearButton(void);
    void CheckForInsertButton(void);
    void CheckForDeleteButton(void);
    void CheckForFindButton(void);
    void CheckForScrollArrowButtons(void);
    void CheckForCodeSelectButtons(void);
    void CheckForCodeLineSelectButtons(void);

    void RunCodeEditor(void);

};

#endif

