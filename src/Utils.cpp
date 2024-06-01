#include <EvoAI/Utils.hpp>

namespace EvoAI{
    std::vector<std::vector<std::string>> readCSVFile(std::istream& istr, char delimiter){
        std::vector<std::vector<std::string>> csvData;
        std::string line;
        std::string cell;
        while(std::getline(istr,line)){
            std::vector<std::string> row;
            std::stringstream lineStream(line);
            while(std::getline(lineStream,cell, delimiter)){
                row.emplace_back(cell);
            }
            csvData.emplace_back(row);
        }
        return csvData;
    }
    void writeCSVFile(const std::vector<std::vector<std::string>>& csvData, std::ostream& ostr, char delimiter){
        for(const auto& v:csvData){
            for(const auto& str:v){
                if(&str != &v.back()){
                    ostr << str << delimiter;
                }else{
                    ostr << str;
                }
            }
            ostr << "\n";
        }
    }
}