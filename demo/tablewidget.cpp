#include <BGE/All>

using namespace bge;

Window* createWindow(const std::string& file)
{
    TiXmlDocument document(file);
    if(!document.LoadFile())
	{
		err() << "load file "<<file<<" failed\n";
        return NULL;
	}
	TiXmlElement* root = document.RootElement();
    if(!root)
	{
		err() << "get "<<file<<" root element of xml failed\n";
        return NULL;
	}
	std::string classname = root->Attribute("class");
    Rtti* rtti = Rtti::rttiByObjectName(classname);
    if(!rtti)
	{
		err() << "get "<<classname << "'s rtti failed\n";
        return NULL;
	}
	Object* object = rtti->createObject();
	if(!object)
	{
	    err() <<"create object from class:"<<classname <<" failed\n";
		return NULL;
	}
    Window* window = Object::castTo<Window>(object);
    if(window)
        window->loadProperty(root);
    else
        err()<<"create class:"<<classname<<" failed\n";
    return window;
}

int main(int argc,char* argv[])
{
    FileSystem::instance().initial(argv[0]);
    FileSystem::instance().setResourcePackage("data.zip");
    WindowManager::instance().initialize("simkai.ttf",false);

    Device* device = Device::create();
    device->initial();
    device->createWindow(640,480,String("BGE Window"));

    Window* tableWidget = createWindow(":tablewidget.xml");
    if(tableWidget)
    {
        tableWidget->loadAppearance(":tableWidgetSkin.xml");
        WindowManager::instance().addWindow(tableWidget);
    }

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
