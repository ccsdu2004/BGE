#include <BGE/All>

using namespace bge;

class Dispatcher : public SlotHolder
{
public:
    Dispatcher(StackLayout* layout,Button* button)
    {
        layout_ = layout;
        button_ = button;
        count_ = 0;
    }

    void click()
    {
        layout_->setCurrentIndex(count_);
        count_ ++;
        if(count_ > 5)
            count_ = 0;
    }
private:
    StackLayout* layout_;
    Button* button_;
    int count_;
};

int main(int argc,char* argv[])
{
    FileSystem::instance().initial(argv[0]);
    FileSystem::instance().setResourcePackage("data.zip");
    WindowManager::instance().initialize(":accid.ttf",false);

    Device* device = Device::create();
    device->initial();
    device->createWindow(640,480,String("BGE Window"));

    Panel* panel = new Panel();
    panel->setPosition(Vector2f(20,20));
    panel->setSize(Vector2f(128,128+24));

    Panel* stackPanel = new Panel();
    stackPanel->setSize(Vector2f(128,128));

    StackLayout* stackLayout = new StackLayout();
    stackPanel->setLayout(stackLayout);
    stackLayout->setSpacing(.0f);
    for(int i=1;i<7;i++)
    {
        ImageBox* image = new ImageBox();
        std::string file = ":box/side";
        file += toString(i);
        file += ".png";
        image->setImage(file);
        image->setPosition(Vector2f(0,0));
        image->setSize(Vector2f(128,128));
        stackLayout->insertWindow(i,image);
    }

    Button* button = new Button();
    button->loadAppearance(":buttonskin1.xml");
    button->setSize(Vector2f(128,24));
    button->setText(L"Click");

    Dispatcher dispacher(stackLayout,button);
    button->clicked.connect(&dispacher,&Dispatcher::click);

    BoxLayout* layout = new BoxLayout();
    layout->setSpacing(.0f);
    panel->setLayout(layout);
    layout->setJustification(Orientation_vertical);
    layout->addWindow(stackPanel);
    layout->addWindow(button);

    panel->adjust();

    WindowManager::instance().addWindow(panel);

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
