//
// Created by exdisj on 10/21/24.
//

#include "BinaryUnit.h"

Unit::Unit() : Data(nullptr), blockSize(0) {}
Unit::Unit(char* Data, unsigned char Size, bool Copy) : Unit()
{

}
Unit::Unit(const Unit& obj) noexcept
{
    this->blockSize = obj.blockSize;
    this->Data = new char[blockSize];
    memcpy(obj.Data, this->Data, blockSize);
}
Unit::Unit(Unit&& obj) noexcept : Data(std::exchange(obj.Data, nullptr)), blockSize(std::exchange(obj.blockSize, 0)) {}
Unit::~Unit()
{
    Deallocate();
}

void Unit::Deallocate()
{
    delete[] Data;
    Data = nullptr;
    blockSize = 0;
}
void Unit::Allocate(char* data, unsigned char size, bool copy)
{
    if (!size || !data)
        return;

    this->blockSize = size;
    if (copy)
    {
        this->Data = new char[size];
        memcpy(this->Data, data, size);
    }
    else
        this->Data = data;
}

Unit& Unit::operator=(const Unit& obj) noexcept
{
    if (this == &obj)
        return *this;

    if (Data)
        Deallocate();

    this->blockSize = obj.blockSize;
    this->Data = new char[blockSize];
    memcpy(obj.Data, this->Data, blockSize);

    return *this;
}
Unit& Unit::operator=(Unit&& obj) noexcept
{
    if (Data)
        Deallocate();

    this->blockSize = std::exchange(obj.blockSize, 0);
    this->Data = std::exchange(obj.Data, nullptr);
    return *this;
}

const char* Unit::Expose() const noexcept
{
    return this->Data;
}
[[nodiscard]] unsigned char Unit::GetSize() const noexcept
{
    return this->blockSize;
}