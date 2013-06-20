#include "main_common.cpp"


LRESULT CALLBACK wndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
		case WM_SIZE:
			//TODO: update gl view
			break;
        case WM_KEYUP:
            switch (wParam) {
                case '0':
                    newTest = nullptr;
                    std::cout << "Testing off. Plain rendering to backbuffer..." << std::endl;
                    break;
                case '1':
                    newTest = tests[0];
                    std::cout << newTest->name() << std::endl;
                    break;
                case '2':
                    newTest = tests[1];
                    std::cout << newTest->name() << std::endl;
                    break;
                case '3':
                    newTest = tests[2];
                    std::cout << newTest->name() << std::endl;
                    break;
				//case VK_OEM_102: // < and >
					//if (GetAsyncKeyState(VK_SHIFT)) {
				case VK_OEM_MINUS:
					if (currentTest) {
                        currentTest->setMode(currentTest->getMode() - 1);
                        std::cout << currentTest->name() << std::endl;
                    }
					break;
				case VK_OEM_PLUS:
					if (currentTest) {
                        currentTest->setMode(currentTest->getMode() + 1);
                        std::cout << currentTest->name() << std::endl;
                    }
					break;
                case 'F':
                    showFPS = !showFPS;
                    break;
                case 'M':
                    doMultisampling = !doMultisampling;
                    break;
            }
            //return TRUE;
            break;
    }
    return DefWindowProc(wnd, message, wParam, lParam);
}

int main(int argc, char **argv)
{
	Timing::getInstance();

	//set stuff up
	if (!setup()) {
		std::cout << "Setup failed!" << std::endl;
		return -1;
	}

	std::string cmd(GetCommandLine());
	if (cmd.find("-i") != std::string::npos) {
		//free running test. print keys
		std::cout << "Possible keys are:" << std::endl;
		std::cout << "0-3: Select test (none/glReadPixels/glGetTexImage/PBOs)" << std::endl;
		std::cout << "-/+: Change test mode (none/GL_BGRA/GL_RGBA)" << std::endl;
		std::cout << "f: Show frame time" << std::endl;
		std::cout << "ESC: Quit" << std::endl;

		//start loop
		unsigned int startTicks = Timing::getTimeMsui();
		do {
			interactive();

            //show frames/sec in title
            frames++;
            unsigned long endTicks = Timing::getTimeMsui();
            unsigned long tickDelta = endTicks - startTicks;
            if (tickDelta >= 3000) {
                std::cout.setf(std::ios::fixed, std::ios::floatfield);
                std::cout.precision(2);
                std::cout << "frame time " << (float)tickDelta / frames << " ms" << std::endl;
                startTicks = endTicks;
                frames = 0;
            }

            //process windows messages
            MSG msg;
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
                if (WM_QUIT == msg.message) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            //Sleep(1);
        } while (!GetAsyncKeyState(VK_ESCAPE));
    }
	else {
		//run benchmark
		benchmark();
	}
    //SendMessage(glWinHWND, WM_CLOSE, 0, 0);
    //ExitProcess(0);
	return 0;
}