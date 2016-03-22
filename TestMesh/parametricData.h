#pragma once

#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

#include <string>
#include <vector>

class entity
{
public:
    int parasolidId;
    int entityType;
public:
    entity(int parasolidId = -1, int entityType = -1) :
        parasolidId(parasolidId),
        entityType(entityType)
    {
    }
};

//class variable
//{
//public:
//    std::string systemName;
//    std::string name;
//    std::string displayName;
//    std::string formula;
//    double value;
//    std::vector<double> discreteValues;
//};

class dimension
{
public:
    std::string systemName;
    std::string name;
    std::string displayName;
    std::string formula;
    double value;
    std::vector<double> discreteValues;
    std::vector<entity*> related;
};

class parametricData
{
public:
    std::vector<dimension*> dimensions;
    std::map<int, entity*> entities;
    std::map<entity*, std::vector<dimension*>> entityDimensions;

public:
    parametricData()
    {}

    ~parametricData()
    {}

    static parametricData* load(std::string fileName)
    {
        parametricData* data = new parametricData();

        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document doc;
        doc.ParseStream(is);

        auto variablesCount = doc.Size();
        for (rapidjson::SizeType i = 0; i < variablesCount; i++)
        {
            const rapidjson::Value& variable = doc[i];

            auto systemName = std::string(variable["SystemName"].GetString());
            auto name = std::string(variable["Name"].GetString());
            auto displayName = std::string(variable["DisplayName"].GetString());
            auto formula = std::string(variable["Formula"].GetString());
            auto value = variable["Value"].GetDouble();
            auto type = variable["Type"].GetInt();

            auto dim = new dimension();
            dim->systemName = systemName;
            dim->name = name;
            dim->displayName = displayName;
            dim->formula = formula;
            dim->value = value;

            const rapidjson::Value& discreteValues = variable["DiscreteValues"];
            auto discreteValuesCount = discreteValues.Size();
            for (rapidjson::SizeType i = 0; i < discreteValuesCount; i++)
                dim->discreteValues.push_back(discreteValues[i].GetDouble());

            const rapidjson::Value& related = variable["Related"];
            auto relatedCount = related.Size();
            for (rapidjson::SizeType i = 0; i < relatedCount; i++)
            {
                const rapidjson::Value& child = related[i];
                auto entityType = child["EntityType"].GetInt();
                auto parasolidId = child["ParasolidId"].GetInt();

                auto it = data->entities.find(parasolidId);

                entity* t;

                if (it != data->entities.end())
                    t = it->second;
                else
                {
                    t = new entity(parasolidId, entityType);
                    data->entities[parasolidId] = t;
                }

                dim->related.push_back(t);
                data->entityDimensions[t].push_back(dim);
            }

            data->dimensions.push_back(dim);
        }

        fclose(fp);

        return data;
    }
};