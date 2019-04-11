#include <BGE/All>

using namespace bge;

class Dispatcher : public SlotHolder
{
public:
    Dispatcher(ImageBox* image,Button* button)
    {
        image_ = image;
        button_ = button;
        count_ = 1;
    }

    void click()
    {
        std::string file = ":box/side";
        file += toString(count_);
        file += ".png";
        image_->setImage(file);
        count_ ++;
        if(count_ == 7)
            count_ = 1;
    }
private:
    ImageBox* image_;
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

    ImageBox* image = new ImageBox();
    image->setImage(":box/side6.png");
    image->setPosition(Vector2f(0,0));
    image->setSize(Vector2f(128,128));

    Button* button = new Button();
    button->loadAppearance(":buttonskin1.xml");
    button->setSize(Vector2f(128,24));
    button->setText(L"Click");

    Dispatcher dispacher(image,button);
    button->clicked.connect(&dispacher,&Dispatcher::click);

    BoxLayout* layout = new BoxLayout();
    layout->setSpacing(.0f);
    panel->setLayout(layout);
    layout->setJustification(Orientation_vertical);
    layout->addWindow(image);
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
