#include <BGE/System/Err.h>
#include <BGE/System/Serialization.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/Layout.h>
#include <algorithm>
#if defined(_MSC_VER)
#include <numeric>
#endif

namespace bge
{

IMPLEMENT_OBJECT_CREATE(GridLayout,Layout)

GridLayout& GridLayout::setColumnWidths(const std::vector<float>& widths)
{
    while(widths.size() > columnWidths_.size())
    {
        columnWidths_.push_back(0);
    }

    std::copy(widths.begin(),widths.end(),columnWidths_.begin());
    return *this;
}

GridLayout& GridLayout::setRowHeights(const std::vector<float>& heights)
{
    while(heights.size() > rowHeights_.size())
    {
        rowHeights_.push_back(0);
    }

    std::copy(heights.begin(),heights.end(),rowHeights_.begin());
    return *this;
}

float GridLayout::columnWidth(int index)const
{
    return (index < int(columnWidths_.size())) ?  columnWidths_[index] : 0.0f;
}

float GridLayout::rowHeight(int index)const
{
    return (index < int(rowHeights_.size())) ? rowHeights_[index] : 0.0f;
}

Layout& GridLayout::addWindow(Window* window)
{
    if(parent())
    {
        addWindow(window,GridCellInfo());
        parent()->insertChild(window);
    }
    else
        err() << "parent is NULL.\n";
    return *this;
}

Layout& GridLayout::addWindow(Window* window,const LayoutInfo& inf)
{
    if(parent())
	{
	    GridCellInfo info = (const GridCellInfo&)(inf);

	    Cell cell(window,info);
	    cells_.push_back(cell);
		parent()->insertChild(window);
	}
    else
        err() << "parent is NULL.\n";
    return *this;
}

Layout& GridLayout::removeWindow(Window* window)
{
    std::list<Cell>::iterator it;
    for(it = cells_.begin(); it != cells_.end(); ++it)
    {
        if((*it).window_ == window)
            break;
    }
    if(it != cells_.end())
        cells_.erase(it);
    parent()->removeChild(window);
    return *this;
}

Layout& GridLayout::arrangeWithin()
{
    Window* window_ = (Window*)parent();
	if(!window_)
		return *this;

    const float width  = window_->size().x() - (2.0f * border_.x());
    const float height = window_->size().y() - (2.0f * border_.y());

    const Vector2f offset = window_->position() + border_;

    const uint32_t numWindows = cells_.size();

    if(numWindows > 0)
    {
        calculateBlocks();

        std::vector<float> colWidths = columnWidths_;
        std::vector<float> rowHeights = rowHeights_;

        calculateColWidths(colWidths, width);
        calculateRowHeights(rowHeights, height);

        std::map<uint32_t, Block>::iterator it;
        for(it = blockMap_.begin(); it != blockMap_.end(); ++it)
        {
            const Block& block = (*it).second;

            if(block.windowStart)
            {
                auto xStartIt = colWidths.begin();
                std::advance(xStartIt, block.col);

                auto xEndIt = xStartIt;
                std::advance(xEndIt, block.hSpan);

                auto yStartIt = rowHeights.begin();
                std::advance(yStartIt, block.row);

                auto yEndIt = yStartIt;
                std::advance(yEndIt, block.vSpan);

                const float x = std::accumulate(colWidths.begin(),  xStartIt, 0.0f) + block.hSpace;
                const float y = std::accumulate(rowHeights.begin(), yStartIt, 0.0f) + block.vSpace;

                const float childWidth  = std::accumulate(xStartIt, xEndIt, 0.0f) - (2.0f * block.hSpace);
                const float childHeight = std::accumulate(yStartIt, yEndIt, 0.0f) - (2.0f * block.vSpace);

                block.window->setPosition(Vector2f(x, y) + offset);
                block.window->setSize(Vector2f(childWidth,childHeight));
				err()<<block.window->objectName()<<" "<<block.vSpace<<" "<<block.window->size().x_<<" "<<block.window->size().y_<<std::endl;
            }
        }
    }

    return *this;
}

void GridLayout::calculateColWidths(std::vector<float>& colWidths, float width)
{
    float totalWidth = 0;
    float curWidth = 0;
    unsigned int numUnset = 0;
	for(std::vector<float>::iterator it = colWidths.begin(); it != colWidths.end(); ++it)
    {
        float& w = *it;
        if(w == 0.0f)
            numUnset++;
        else
        {
            totalWidth += w;
            if(totalWidth > width)
            {
                w = width - curWidth;
                totalWidth = width;
            }
            curWidth = totalWidth;
        }
    }

    const float numUnsetCols = numCols_ - colWidths.size() + numUnset;

    if(numUnsetCols > 0.0f)
    {
        float w = 0;
        if(totalWidth < width)
        {
            const float remainingWidth = width - totalWidth;
            if(remainingWidth > numUnsetCols) 
				w = remainingWidth / numUnsetCols;
        }
 
        for(size_t i = 0; i < colWidths.size(); ++i)
        {
            if(colWidths[i] == 0.0f) 
				colWidths[i] = w;
        }

		int32_t count = numCols_ - colWidths.size();
		//if(count < 0)
		//	assert(0);
		//	count = -count;
		//if(count > 0)
		{
			for(size_t j = 0; j < count; ++j)
			{
				colWidths.push_back(w);
			}
		}
    }
}

void GridLayout::calculateRowHeights(std::vector<float>& rowHeights,float height)
{
    float totalHeight = 0;
    float curHeight = 0;
    uint32_t numUnset = 0;
    for(auto it = rowHeights.begin(); it != rowHeights.end(); ++it)
    {
        float& w = *it;
        if(w == 0.0f)
            numUnset++;
        else
        {
            totalHeight += w;
            if(totalHeight > height)
            {
                w = height - curHeight;
                totalHeight = height;
            }
            curHeight = totalHeight;
        }
    }

    const float numUnsetRows = static_cast<float>(numRows_ - rowHeights.size() + numUnset);
    if(numUnsetRows > 0.0f)
    {
        float h = 0;
        if(totalHeight < height)
        {
            const float remainingHeight = height - totalHeight;
            if(remainingHeight > numUnsetRows)
                h = remainingHeight / numUnsetRows;
        }
        for(int i = 0; i < int(rowHeights.size()); ++i)
        {
            if(rowHeights[i] == 0)
                rowHeights[i] = h;
        }
        for(unsigned j = 0; j < numRows_ - rowHeights.size(); ++j)
        {
            rowHeights.push_back(h);
        }
    }
}

void GridLayout::calculateBlocks()
{
    numCols_ = 0;
    numRows_ = 0;

    uint32_t nextCol = 0;
    uint32_t nextRow = 0;

    blockMap_.clear();

    std::list<Cell>::iterator it;
    for(it = cells_.begin(); it != cells_.end(); ++it)
    {
        const GridCellInfo& info = (*it).info_;

        if(info.columnIndex_ != GridCellInfo::relativeToPrevious )
            nextCol = info.columnIndex_;

        if(info.rowIndex_ != GridCellInfo::relativeToPrevious)
            nextRow = info.rowIndex_;

        while(blockMap_.find(nextRow*256 + nextCol) != blockMap_.end())
            ++nextCol;

        uint32_t hSpan = info.horizontalSpan_;
        uint32_t vSpan = info.verticalSpan_;

        for(uint32_t i = 0; i != hSpan; ++i )
        {
            for(uint32_t j = 0; j != vSpan; ++j )
            {
                Block block;
                block.windowStart = ( i == 0 && j == 0 );
                block.col = nextCol + i;
                block.row = nextRow + j;
                block.hSpan = hSpan;
                block.vSpan = vSpan;
                block.hSpace = spacing();
                block.vSpace = spacing();
                block.window = (*it).window_;
                addBlockToMap(block);
            }
        }

        if(nextCol + hSpan > numCols_)
            numCols_ = nextCol + hSpan;
        if(nextRow + vSpan > numRows_)
            numRows_ = nextRow + vSpan;
        nextCol += hSpan;
    }
}

void GridLayout::parseItemNode(TiXmlElement* item)
{
	if(item)
	{
		TiXmlElement* windowNode = item->FirstChildElement("window");
		Window* window = parse(windowNode);

		TiXmlElement* infoNode = item->FirstChildElement("info");
		GridCellInfo cellinfo;
		if(infoNode)
		{
			std::string classname = infoNode->Attribute("class");
			Rtti* rtti = Rtti::rttiByObjectName(classname);
			if(rtti)
			{
				LayoutInfo* info = Object::castTo<LayoutInfo>(rtti->createObject());
				if(info)
			        info->loadProperty(infoNode);
			}
			cellinfo.loadProperty(infoNode);
		}

		if(window)
		{
			addWindow(window,cellinfo);
		}
	}
}

bool GridLayout::loadProperty(TiXmlNode* node)
{
	numCols_ = 0;
	numRows_ = 0;

	DESERIALIZE_OBJECT(border,node,Vector2<float>)
	
	std::vector<float> colwidth = deserializeContainer<std::vector<float>,float>("columnWidths",node);
	std::vector<float> colheight = deserializeContainer<std::vector<float>,float>("rowHeights",node);
	setColumnWidths(colwidth);
	setRowHeights(colheight);
	//DESERIALIZE_OBJECT_CONTAINER(columnWidths,node,std::vector<float>,float)
	//DESERIALIZE_OBJECT_CONTAINER(rowHeights,node,std::vector<float>,float)
	//DESERIALIZE_OBJECT(numCols,node,uint32_t)
	//DESERIALIZE_OBJECT(numRows,node,uint32_t)

    TiXmlElement* child = node->FirstChildElement("windows");
	child = child->FirstChildElement("item");
    while(child)
    {
        parseItemNode(child);
		child = child->NextSiblingElement();
    }
    TiXmlNode* layout = node->FirstChildElement("Layout");
    Layout::loadProperty(layout);
    return false;
}

bool GridLayout::saveProperty(TiXmlNode* node)
{
    node->ToElement()->SetAttribute("class",getRtti()->name());
    SERIALIZE_OBJECT(border,node)
    SERIALIZE_CONTAINER(columnWidths,node)
    SERIALIZE_CONTAINER(rowHeights,node)
    SERIALIZE_OBJECT(numCols,node)
    SERIALIZE_OBJECT(numRows,node)

    TiXmlElement windows("windows");
    for(auto itr = cells_.begin();itr != cells_.end();itr++)
    {
        TiXmlElement item("item");
        {
            TiXmlElement window("window");
            window.SetAttribute("class",(*itr).window_->getRtti()->name());
            (*itr).window_->saveProperty(&window);
            item.InsertEndChild(window);

            TiXmlElement info("info");
            info.SetAttribute("class",(*itr).info_.getRtti()->name());
            (*itr).info_.saveProperty(&info);
            item.InsertEndChild(info);
        }
        windows.InsertEndChild(item);
    }

    node->InsertEndChild(windows);

    TiXmlElement element("Layout");
    Layout::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

IMPLEMENT_OBJECT(GridCellInfo,LayoutInfo)
const uint32_t GridCellInfo::relativeToPrevious = 100000;

GridCellInfo::GridCellInfo():
    horizontalSpan_(1),
    verticalSpan_(1),
    columnIndex_(relativeToPrevious),
    rowIndex_(relativeToPrevious)
{
}

bool GridCellInfo::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(horizontalSpan,node,uint32_t)
    DESERIALIZE_OBJECT(verticalSpan,node,uint32_t)
    DESERIALIZE_OBJECT(columnIndex,node,uint32_t)
    DESERIALIZE_OBJECT(rowIndex,node,uint32_t)
    TiXmlNode* window = node->FirstChildElement("LayoutInfo");
    LayoutInfo::loadProperty(window);
    return true;
}

bool GridCellInfo::saveProperty(TiXmlNode* node)
{
    node->ToElement()->SetAttribute("class",getRtti()->name());

    SERIALIZE_OBJECT(horizontalSpan,node)
    SERIALIZE_OBJECT(verticalSpan,node)
    SERIALIZE_OBJECT(columnIndex,node)
    SERIALIZE_OBJECT(rowIndex,node)

    TiXmlElement element("LayoutInfo");
    LayoutInfo::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
