#include <BGE/All>

using namespace bge;

int index = 0;

Window* createButton(const std::string& name,const String& text)
{
    Button* button = new Button(0,text);
	button->setName(name);
    button->setSize(Vector2f(92,42));
	if(index == 0)
		button->loadAppearance(":buttonskin1.xml");
	index ++;
    return button;
}

int main(int argc, char* argv[])
{
    FileSystem::instance().initial(argv[0]);
    FileSystem::instance().setResourcePackage("data.zip");
    WindowManager::instance().initialize("simkai.ttf",false);

    Device* device = Device::create();
    device->initial();
    device->createWindow(640,480,bge::String("BGE Window"));

    Window* window = WindowManager::instance().createWindow(":grid.xml");
    WindowManager::instance().addWindow(window);

    while(device->isRunning())
    {
        device->preRender();
        WindowManager::instance().update();
        device->swapBuffers();
        device->pollEvents();
    }

    WindowManager::instance().terminate();
    device->closeWindow();
    device->terminate();
    device->deleteLater();
    return 0;
}

