#include <BGE/All>

using namespace bge;

String text = L"<Blue>这段代码主要测试BGE的富文本显示功能\n</Blue><Red>需要指出的是当前只要能支持多行显示和多颜色显示功能即可(不考虑下划线斜体以及加粗显示)。</Red><White>这句话将被显示为白色</><Blue>这句话应该被蓝色字体</Blue><Green>绿色字体</><Cyan>Cyan色</>";

int main(int argc,char* argv[])
{
    FileSystem::instance().initial(argv[0]);
    FileSystem::instance().setResourcePackage("data.zip");
    WindowManager::instance().initialize("simkai.ttf",false);

    Device* device = Device::create();
    device->initial();
    device->createWindow(640,480,String("BGE Window"));

    StaticText* edit = new StaticText();
    edit->asOneLine(false);
    edit->setRichMode(true);
    edit->setPosition(Vector2f(10.0f,10.0f));
    edit->setSize(Vector2f(620.0f,460.0f));
    edit->setText(text);
    edit->loadAppearance(":StaticTextDesc.xml");

    WindowManager::instance().addWindow(edit);

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
