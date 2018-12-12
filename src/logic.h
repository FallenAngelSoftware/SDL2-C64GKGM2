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

    void CalculateCodeLastLine(void);

    bool ThereIsCodeAfterThisLine(int line);
    void ShowHideCodeSelectLineNumberBoxes(void);

    #define Nothing                 0
    #define LineNumberSelect        1
    #define ClearCode               2
    int DialogToShow;
    int DialogToShowOld;

    int LineNumberArray[3];

    void RunCodeEditor(void);

};

#endif

