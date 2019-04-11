#include <cstdlib>
#include <BGE/All>

using namespace bge;

Window* createButton(const std::string& name,const String& text)
{
    Button* button = new Button(0,text);
    button->setSize(Vector2f(92,42));
	button->setName(name);
    return button;
}

int main(int argc,char* argv[])
{
    FileSystem::instance().initial(argv[0]);
    FileSystem::instance().setResourcePackage("data.zip");

    Device* device = Device::create();
    device->initial();
    device->createWindow(640,480,bge::String("BGE Window"));

    WindowManager::instance().initialize(":accid.ttf",true);

	FrameWindow* window = new FrameWindow();
	window->setText(L"BGE");
	window->setSize(Vector2f(160,154));
	window->enableMovement();
	Panel* panel = new Panel();
	panel->setSize(Vector2f(100,90));
	window->setClientPanel(panel);

	BoxLayout* layout = new BoxLayout(Orientation_vertical);
	panel->setLayout(layout);
	layout->addWindow(createButton("confirm",L"OK"));
	layout->addWindow(createButton("cancel",L"Cancel"));
	panel->adjust();

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
