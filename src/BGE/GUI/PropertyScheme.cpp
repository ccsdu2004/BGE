#include <BGE/System/Err.h>
#include <BGE/GUI/PropertyScheme.h>
#include <fstream>
#include <iterator>

namespace bge
{

PropertyScheme::PropertyScheme()
{
}

PropertyScheme::~PropertyScheme()
{
}

bool PropertyScheme::load(const std::string& file)
{
    TiXmlDocument document(file);
    if(!document.LoadFile())
	{
		err() << "load shceme file failed.\n";
        return false;
	}
    TiXmlElement* root = document.RootElement();
	if(!root)
	{
		err() << "get root element of shceme failed.\n";
		return false;
	}

	buttonDesc_.loadProperty(root);
    panelDesc_.loadProperty(root);
    editFieldDesc_.loadProperty(root);
    scrollBarDesc_.loadProperty(root);
    listBoxDesc_.loadProperty(root);
    listControlDesc_.loadProperty(root);
    progressBarDesc_.loadProperty(root);
    checkBoxDesc_.loadProperty(root);
    sliderDesc_.loadProperty(root);
    frameWindowDesc_.loadProperty(root);
    staticTextDesc_.loadProperty(root);
    comboBoxDesc_.loadProperty(root);
    return true;
}

bool PropertyScheme::save(const std::string& file)
{
    TiXmlDocument document(file);
    TiXmlElement root("scheme");
    buttonDesc_.saveProperty(&root);
    panelDesc_.saveProperty(&root);
    editFieldDesc_.saveProperty(&root);
    scrollBarDesc_.saveProperty(&root);
    listBoxDesc_.saveProperty(&root);
    listControlDesc_.saveProperty(&root);
    progressBarDesc_.saveProperty(&root);
    checkBoxDesc_.saveProperty(&root);
    sliderDesc_.saveProperty(&root);
    frameWindowDesc_.saveProperty(&root);
    staticTextDesc_.saveProperty(&root);
    comboBoxDesc_.saveProperty(&root);
    document.InsertEndChild(root);
    return document.SaveFile();
}

}
