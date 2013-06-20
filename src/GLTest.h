#pragma once

#include "ContextBase.h"


#define GLTEST_DEFAULT_WIDTH 160
#define GLTEST_DEFAULT_HEIGHT 90
#define GLTEST_DEFAULT_DEPTH 4


//Basic interface for OpenGL tests
class GLTest
{
protected:
	std::shared_ptr<ContextBase> context;
    int w;
    int h;
    int d;
    int size;
	int mode;

	/*Constructor. Set up all necessary objects objects here.
	*/
	GLTest(std::shared_ptr<ContextBase> & c, const int width = GLTEST_DEFAULT_WIDTH, const int height = GLTEST_DEFAULT_HEIGHT, const int depth = GLTEST_DEFAULT_DEPTH);

public:
    /*Return the name of the test and the mode set
    */
    virtual std::string name() const;

	/*Set the mode the test is in.
	*/
	virtual void setMode(const int value);
    virtual int getMode() const;

	/*Initialize test before drawing.
	*/
	virtual bool pre();

	/*De-initialize test after drawing.
	*/
	virtual bool post();
};
