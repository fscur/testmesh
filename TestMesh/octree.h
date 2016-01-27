#include "aabb.h"
#include "vertex.h"
#include <vector>
#include <map>

class octree
{
private:

    struct positionData
    {
        unsigned int index;
        vertex vertex;
    };

    aabb _aabb;
    std::vector<glm::vec3> _positions;
    std::map<unsigned int, std::vector<positionData>> _items;

    unsigned int* _indexPtr;
    unsigned int _index;

    octree* _parent;
    octree* _children[8];
    bool _isSubdivided = false;
    unsigned int _maxItems;
    unsigned int _maxLevels;
    unsigned int _currentLevel;

    bool insertIntoChildren(vertex vertex, unsigned int &index);
    void insertIntoChildren(glm::vec3 position, std::vector<positionData> posDataList);
    aabb createAabb(aabb parentAabb, unsigned int index);
    void subdivide();
public:
    octree(aabb aabb, unsigned int maxLevels, unsigned int totalItems);
    octree(octree* parent, unsigned int index, unsigned int maxItems);
    bool insert(vertex vertex, unsigned int &index);
    void draw();
};