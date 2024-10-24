#include <iostream>

#include "BinaryUnit.h"
#include "PackagePager.h"

int main()
{
    const unsigned char unitSize = 1;
    const unsigned pageSize = 10;

    {
        std::fstream host("tester.txt", std::ios::out | std::ios::in | std::ios::trunc);
        auto size = 4 * pageSize * unitSize;
        char* zeroes = new char[size];
        memset(zeroes, 0, size);

        host.write(zeroes, size);
        delete[] zeroes;

        host.close();
    }

    std::vector<std::vector<unsigned>> Pages = {
        {0, 2},
        {1, 3}
    };

    std::vector<std::string> BaseStrings = {
            "hello",
            "goodbye, have a"
    };
    std::vector<std::vector<Unit>> Units;
    Units.resize(BaseStrings.size());
    for (unsigned i = 0; i < Units.size(); i++)
    {
        auto& target = Units[i];
        for (const auto& elem : BaseStrings[i])
            target.emplace_back(elem);
    }

    PackagePager pg("tester.txt", unitSize, pageSize);
    for (unsigned i = 0; i < Pages.size(); i++)
    {
        pg.Bind(Pages[i]);
        if (!pg.WriteUnits(Units[i]))
            std::cout << "Write failed!" << std::endl;
        else
        {
            std::vector<Unit> result = pg.ReadAllUnits();
            std::string encoded;
            for (const auto& item : result)
            {
                auto Converted = item.Convert<char>();
                if (Converted == 0)
                    break;
                else
                    encoded += Converted;
            }

            std::cout << "Encoded message was: " << encoded << std::endl;
        }

        std::cout << std::endl;
    }
    return 0;
}
