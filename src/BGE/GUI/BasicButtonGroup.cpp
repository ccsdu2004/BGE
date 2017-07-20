#include <BGE/GUI/BasicButton.h>

namespace bge
{

BasicButton* ButtonGroup::selected() const
{
    BasicButtons::const_iterator it;
    for(it = buttons_.begin(); it != buttons_.end(); ++it)
    {
        BasicButton* btn = *it;
        if(btn->isMouseOver()) return btn;
    }
    return 0;
}

ButtonGroup& ButtonGroup::select(BasicButton* button)
{
    button->setMouseOverStatus(true);
    return *this;
}

int ButtonGroup::indexOf(BasicButton* button) const
{
    int ret = 0;
    bool found = false;
    BasicButtons::const_iterator it;
    for(it = buttons_.begin(); it != buttons_.end(); ++it)
    {
        BasicButton* btn = *it;
        if(btn == button)
        {
            found = true;
            break;
        }
        ++ret;
    }
    if(!found)
        return -1;
    return ret;
}

}
