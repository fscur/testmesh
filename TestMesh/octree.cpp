#include "octree.h"

#include <GL/glew.h>

#include <algorithm>

octree::octree(aabb aabb, unsigned int maxLevels, unsigned int totalItems)
{
    _index = 0;
    _indexPtr = &_index;

    _parent = nullptr;
    _aabb = aabb;
    _maxLevels = maxLevels;

    if (_maxLevels < 5)
        _maxLevels = 5;

    _maxItems = (unsigned int)(totalItems / glm::pow<float>(8, _maxLevels));

    if (_maxItems < 2)
        _maxItems = 2;
}

octree::octree(octree* parent, unsigned int index, unsigned int maxItems)
{
    _indexPtr = parent->_indexPtr;
    _parent = parent;
    _aabb = createAabb(parent->_aabb, index);
    _maxItems = maxItems;
}

aabb octree::createAabb(aabb parentAabb, unsigned int index)
{
    auto width = parentAabb.halfWidth;
    auto height = parentAabb.halfHeight;
    auto depth = parentAabb.halfDepth;

    auto minX = 0.0f;
    auto minY = 0.0f;
    auto minZ = 0.0f;
    auto maxX = 0.0f;
    auto maxY = 0.0f;
    auto maxZ = 0.0f;

    auto xIndex = index & 4;
    auto yIndex = index & 2;
    auto zIndex = index & 1;

    xIndex = xIndex >> 2;
    yIndex = yIndex >> 1;

    if (xIndex == 0)
    {
        minX = parentAabb.min.x;
        maxX = parentAabb.min.x + width;
    }
    else
    {
        minX = parentAabb.min.x + width;
        maxX = parentAabb.max.x;
    }

    if (yIndex == 0)
    {
        minY = parentAabb.min.y;
        maxY = parentAabb.min.y + height;
    }
    else
    {
        minY = parentAabb.min.y + height;
        maxY = parentAabb.max.y;
    }

    if (zIndex == 0)
    {
        minZ = parentAabb.min.z;
        maxZ = parentAabb.min.z + depth;
    }
    else
    {
        minZ = parentAabb.min.z + depth;
        maxZ = parentAabb.max.z;
    }

    auto min = glm::vec3(minX, minY, minZ);
    auto max = glm::vec3(maxX, maxY, maxZ);

    return aabb(min, max);
}

void octree::insertIntoChildren(glm::vec3 position, std::vector<positionData> posDataList)
{
    auto pos = position;

    pos -= _aabb.min;

    auto x = (int)(pos.x / _aabb.halfWidth);
    auto y = (int)(pos.y / _aabb.halfHeight);
    auto z = (int)(pos.z / _aabb.halfDepth);

    auto childOctreeIndex = 0;

    childOctreeIndex |= x;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= y;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= z;

    auto childOctree = _children[childOctreeIndex];

    childOctree->_positions.push_back(position);
    childOctree->_items[childOctree->_positions.size() - 1] = posDataList;

    /*for (unsigned int i = 0; i < 8; i++)
    {
        auto childOctree = _children[i];

        if (childOctree->_aabb.contains(position))
        {
            childOctree->_positions.push_back(position);
            childOctree->_items[childOctree->_positions.size()-1] = posDataList;
            break;
        }
    }*/
}

bool octree::insertIntoChildren(vertex vertex, unsigned int &index)
{
    /*for (unsigned int i = 0; i < 8; i++)
    {
        auto childOctree = _children[i];

        if (childOctree->_aabb.contains(vertex.GetPosition()))
        {
            return childOctree->insert(vertex, index);
            break;
        }
    }*/

    auto pos = vertex.position;
    
    pos -= _aabb.min;

    auto x = (int)(pos.x / _aabb.halfWidth);
    auto y = (int)(pos.y / _aabb.halfHeight);
    auto z = (int)(pos.z / _aabb.halfDepth);

    auto childOctreeIndex = 0;

    childOctreeIndex |= x;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= y;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= z;

    auto childOctree = _children[childOctreeIndex];

    return childOctree->insert(vertex, index);
}

void octree::subdivide()
{
    for (unsigned int i = 0; i < 8; i++)
        _children[i] = new octree(this, i, _maxItems);

    auto s = _positions.size();
    for (auto i = 0u; i < s; i++)
    {
        insertIntoChildren(_positions[i], _items[i]);
    }

    _items.clear();
    _positions.clear();
    _isSubdivided = true;
}

bool octree::insert(vertex vertex, unsigned int &index)
{
    auto pos = vertex.position;

    if (!_isSubdivided)
    {
        bool found = false;
        unsigned int posIndex = 0;
        auto s = _positions.size();

        for (unsigned int i = 0; i < s; i++)
        {
            if (_positions[i] == pos)
            {
                found = true;
                posIndex = i;
                break;
            }
        }

        if (!found && _positions.size() < _maxItems)
        {
            auto posData = positionData();
            
            posData.index = (*_indexPtr)++;
            posData.vertex = vertex;

            std::vector<positionData> posDataList;
            posDataList.push_back(posData);

            _positions.push_back(pos);
            _items[_positions.size() -1] = posDataList;
            
            index = posData.index;

            return true;
        }
        else if (found)
        {
            auto posDataList = &(_items[posIndex]);
            auto s = posDataList->size();

            for (unsigned int i = 0; i < s; i++)
            {
                auto pData = (*posDataList)[i];
                if (pData.vertex == vertex)
                {
                    index = pData.index;
                    return false;
                }
            }

            auto posData = positionData();

            posData.index = (*_indexPtr)++;
            posData.vertex = vertex;

            posDataList->push_back(posData);

            index = posData.index;
            return true;
        }
        else
        {
            subdivide();
            return insertIntoChildren(vertex, index);
        }
    }
    else
    {
        return insertIntoChildren(vertex, index);
    }
}

/*glMatrixMode(GL_PROJECTION);
glLoadMatrixf(glm::value_ptr(_projectionMatrix));

glMatrixMode(GL_MODELVIEW);
glLoadMatrixf(glm::value_ptr(_viewMatrix * _modelMatrix));

glPushMatrix();

_octree->draw();
glPopMatrix();*/

void octree::draw()
{
    if (_isSubdivided)
        for (unsigned int i = 0; i < 8; i++)
            _children[i]->draw();

    glColor3f(0.0, 1.0, 0.0);
    
    /*auto s = _vertices.size();

    for (unsigned int i = 0; i < s; i++)
    {
        auto position = (_vertices[i])->GetPosition();
        glBegin(GL_POINTS);
        glvertex3f(position.x, position.y, position.z);
        glEnd();
    }*/

    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_LINE_STRIP);

    glVertex3f(_aabb.min.x, _aabb.min.y, _aabb.min.z);
    glVertex3f(_aabb.min.x, _aabb.min.y, _aabb.max.z);
    glVertex3f(_aabb.max.x, _aabb.min.y, _aabb.max.z);
    glVertex3f(_aabb.max.x, _aabb.min.y, _aabb.min.z);
    glVertex3f(_aabb.min.x, _aabb.min.y, _aabb.min.z);

    glEnd();

    glBegin(GL_LINE_STRIP);

    glVertex3f(_aabb.min.x, _aabb.max.y, _aabb.min.z);
    glVertex3f(_aabb.min.x, _aabb.max.y, _aabb.max.z);
    glVertex3f(_aabb.max.x, _aabb.max.y, _aabb.max.z);
    glVertex3f(_aabb.max.x, _aabb.max.y, _aabb.min.z);
    glVertex3f(_aabb.min.x, _aabb.max.y, _aabb.min.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(_aabb.min.x, _aabb.min.y, _aabb.min.z);
    glVertex3f(_aabb.min.x, _aabb.max.y, _aabb.min.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(_aabb.max.x, _aabb.min.y, _aabb.min.z);
    glVertex3f(_aabb.max.x, _aabb.max.y, _aabb.min.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(_aabb.min.x, _aabb.min.y, _aabb.max.z);
    glVertex3f(_aabb.min.x, _aabb.max.y, _aabb.max.z);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(_aabb.max.x, _aabb.min.y, _aabb.max.z);
    glVertex3f(_aabb.max.x, _aabb.max.y, _aabb.max.z);

    glEnd();
}