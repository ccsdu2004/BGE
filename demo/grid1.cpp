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

	//WindowManager::instance().propertyScheme()->load(":skin1.xml");

    Panel* panel = new Panel();
    GridLayout* layout = new GridLayout();
    panel->setLayout(layout);

    std::vector<float> widths;
    widths.push_back(0.0f);
    layout->setColumnWidths(widths);

    std::vector<float> heights;
    for(int k = 0; k < 6; ++k)
        heights.push_back(48.0f);
    layout->setRowHeights(heights);

    GridCellInfo info;
    info.columnIndex_ = 0; info.rowIndex_ = 0; layout->addWindow(createButton("button1",L"开始"),info);
    info.columnIndex_ = 1; info.rowIndex_ = 0; layout->addWindow(createButton("button2",L"保存"),info);
    info.columnIndex_ = 0; info.rowIndex_ = 1; layout->addWindow(createButton("button3",L"放弃"),info);
    info.columnIndex_ = 1; info.rowIndex_ = 1; layout->addWindow(createButton("button4",L"载入"),info);

	info.columnIndex_ = 0;
    info.rowIndex_ = 2;
    info.horizontalSpan_ = 2;
	Window* edit = new EditField(0,L"名将夏侯渊");
	edit->setSize(Vector2f(196,36));
    layout->addWindow(edit,info);

	info.columnIndex_ = 0;
	info.rowIndex_ = 3;
	info.horizontalSpan_ = 2;
	Slider* slider = new Slider();
	slider->setTickPlacement(bge::TickPlacement_below);
	slider->setSize(Vector2f(186,48));
	layout->addWindow(slider,info);

	ProgressBar* bar = new ProgressBar();
	bar->setSize(Vector2f(186,24));
	bar->setPercentage(24.0f);
	info.columnIndex_ = 0;
	info.rowIndex_ = 4;
	info.horizontalSpan_ = 2;
	layout->addWindow(bar,info);

	ComboBox* combobox = new ComboBox();
	combobox->setSize(Vector2f(96,36));
	combobox->addString(L"曹操");
	combobox->addString(L"吕布");
	combobox->addString(L"孙权");
	combobox->addString(L"刘备");
	info.columnIndex_ = 0;
	info.rowIndex_ = 5;
	info.horizontalSpan_ = 2;
	layout->addWindow(combobox,info);

	FrameWindow* window = new FrameWindow();
	window->enableMovement();
	window->setSize(Vector2f(200,340));
	window->setText(L"BGE窗体");
	window->setClientPanel(panel);
	window->adjust();

	Window* button1 = Object::castTo<Window>(panel->findChild("button4"));
	if(button1)
		button1->loadAppearance(":buttonskin1.xml");

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

