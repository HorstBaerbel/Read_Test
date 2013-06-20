#include "main_common.cpp"


int main(int argc, char **argv)
{
	#ifdef BCMHOST
		bcm_host_init();
	#endif

	Timing::getInstance();

	//set stuff up
	if (!setup()) {
		std::cout << "Setup failed!" << std::endl;
		return -1;
	}

	/*XEvent event;
	bool done = False;

	//wait for events
	while (!done) {
		//handle the events in the queue
		while (XPending(display) > 0) {
			XNextEvent(display, &event);
			switch (event.type) {
				case Expose:
					if (event.xexpose.count != 0) {
						break;
					}
					renderGL();
					break;
				case ConfigureNotify:
					//call resizeGL only if our window-size changed
					if ((event.xconfigure.width != GLWin.width) || (event.xconfigure.height != GLWin.height)) {
						width = event.xconfigure.width;
						height = event.xconfigure.height;
						resizeGL(width, height);
					}
					break;
					//exit in case of a mouse button press
				case ButtonPress:
					done = True;
					break;
				case KeyPress:
					if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
						done = True;
					}
					if (XLookupKeysym(&event.xkey,0) == XK_F11) {
						destroyWindow();
						fullscreen = !fullscreen;
						createWindow();
					}
					break;
				case ClientMessage:
					if (strcmp(XGetAtomName(display, event.xclient.message_type), "WM_PROTOCOLS") == 0) {
						done = True;
					}
					break;
				default:
					break;
			}
		}
		renderGL();
		//usleep(1000);
	}

destroyWindow();

return 0;*/

	//run benchmark
	benchmark();

	return 0;
}