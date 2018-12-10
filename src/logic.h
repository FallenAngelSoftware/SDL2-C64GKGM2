#ifndef LOGIC
#define LOGIC

class Logic
{
public:

	Logic(void);
	virtual ~Logic(void);

    int CommandScrollNumberMoved;
    int CommandScrollSpeed;

    int CommandDisplayStartIndex;
    int CommandDisplayEndIndex;

    void RunCodeEditor(void);

};

#endif

