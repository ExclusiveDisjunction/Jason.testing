//
// Created by exdisj on 10/21/24.
//

#include "PackagePager.h"

PackagePager::PackagePager(std::filesystem::path location, unsigned char UnitSize, unsigned PageSize) : handle(std::move(location), std::ios::out | std::ios::in | std::ios::binary), binding(), location(std::make_pair(0u, 0u)), unitSize(UnitSize), pageSize(PageSize)
{
    handle.file.seekg(0, std::ios::beg);
}
PackagePager::PackagePager(PackagePager&& obj) noexcept : handle(std::move(obj.handle)), binding(std::move(obj.binding)), location(std::move(obj.location)), unitSize(std::exchange(obj.unitSize, 0)), pageSize(std::exchange(obj.pageSize, 0))
{

}
PackagePager::~PackagePager()
{
    handle.Close();
    binding = {};
    location = std::make_pair(0u, 0u);
    unitSize = 0;
    pageSize = 0;
}

PackagePager& PackagePager::operator=(PackagePager&& obj) noexcept
{
    Close();

    this->handle = std::move(obj.handle);
    this->binding = std::move(obj.binding);
    this->location = std::move(obj.location);
    this->unitSize = std::exchange(obj.unitSize, 0);
    this->pageSize = std::exchange(obj.pageSize, 0);
    return *this;
}

bool PackagePager::EndOfFile() const noexcept
{
    return handle.file.eof() || boundEof;
}
bool PackagePager::IsBound() const noexcept
{
    return binding.has_value() && boundPageIndex.has_value();
}

Unit PackagePager::ReadUnit()
{
    if (!IsBound() || EndOfFile())
        throw std::logic_error("Invalid operation: The reader is not bound, or is EOF.");

    char* data = new char[unitSize];
    this->handle.file.read(data, unitSize);

    //Now we need to check our location
    if (!Advance())
        throw std::logic_error("Could not advance");

    return { data, unitSize, false };
}
bool PackagePager::Advance()
{
    if (!IsBound() || EndOfFile())
        return false;

    location.second++;
    if (location.second / pageSize > 0) // We have moved to the next page
        return AdvancePage();
    else
        return true;
}
bool PackagePager::AdvancePage()
{
    if (!IsBound() || EndOfFile())
        return false;

    auto& index = boundPageIndex.value();
    auto& list = binding.value();
    index++;

    if (index > list.size()) //Out of range
    {
        boundEof = true;
        MoveAbsolute(0, 0);
    }
    else
    {
        unsigned nextPage = list[index];
        MoveAbsolute(nextPage, 0);
    }

    return !boundEof;
}
std::vector<Unit> PackagePager::ReadUnits(unsigned int Units)
{
    std::vector<Unit> result;
    result.resize(Units);

    try
    {
        for (unsigned i = 0; i < Units; i++)
            result[i] = std::move(ReadUnit());

        return result;
    }
    catch (...)
    {
        return {};
    }
}
std::vector<Unit> PackagePager::ReadAllUnits()
{
    if (!IsBound() || EndOfFile())
        throw std::logic_error("No information can be found.");

    MoveRelative(0u);
    std::vector<Unit> result;
    unsigned size = binding.value().size() * pageSize;
    result.resize(size);

    unsigned i = 0;
    //The approach is to read page by page, selecting an array out of the stream one at a time.
    do
    {
        auto thisSize = unitSize * pageSize;
        char* thisPage = new char[thisSize];
        memset(thisPage, 0, thisSize);
        handle.file.read(thisPage, thisSize);

        for (unsigned j = 0; j < pageSize && i < size; i++, j++)
            result[i].Allocate(thisPage + (j * unitSize), unitSize, true);

        delete[] thisPage;
    } while (AdvancePage());

    try
    {
        for (auto& elem : result)
            elem = std::move(ReadUnit());
    }
    catch (...)
    {
        return {};
    }

    return result;
}
bool PackagePager::WriteUnits(const std::vector<Unit>& units)
{
    if (!IsBound() || EndOfFile())
        return false;

    auto curr = units.begin(), end = units.end();
    do
    {
        if (curr->GetSize() != unitSize)
            return false;

        handle.file.write(curr->Expose(), unitSize);
        curr++;
    } while (Advance() && curr != end);

    return curr == end; //We know all elements were written.
}
bool PackagePager::WipeAll()
{
    if (!MoveRelative(0))
        return false;

    if (!IsBound() || EndOfFile())
        return false;

    char* sequence = new char[unitSize];
    memset(sequence, 0, unitSize);

    do
    {
        handle.file.write(sequence, unitSize);
    } while (Advance());

    delete[] sequence;
    return MoveRelative(0);
}

void PackagePager::Bind(std::vector<unsigned> pages)
{
    Reset();
    binding = pages;
    boundPageIndex = 0;
    MoveRelative(0);
}
void PackagePager::Reset()
{
    binding = {};
    boundPageIndex = {};
    (void)MoveAbsolute(0, 0);
}
void PackagePager::Close()
{
    Reset();
    handle.Close();
}
void PackagePager::Flush()
{
    handle.file.flush();
}

bool PackagePager::MoveRelative(unsigned unitPosition)
{
    if (!IsBound())
        return false;

    unsigned pageLoc = unitPosition / pageSize;
    unitPosition -= pageLoc * pageSize;

    //We are moving from the first unit in the bound element.
    auto& pages = *binding;
    if (pageLoc > pages.size())
        return false;

    return MoveAbsolute(pages[pageLoc], unitPosition);
}
bool PackagePager::MoveAbsolute(unsigned pagePosition, unsigned unitPosition)
{
    return MoveAbsolute(std::make_pair(pagePosition, unitPosition));
}
bool PackagePager::MoveAbsolute(std::pair<unsigned, unsigned> loc)
{
    std::streamoff truePos = (loc.first * pageSize + loc.second) * unitSize;
    handle.file.seekg(truePos);

    if (handle.file.bad() || handle.file.eof()) //EOF
    {
        location = std::make_pair<unsigned, unsigned>(0, 0);
        return false;
    }
    else
    {
        location = loc;
        return true;
    }
}
unsigned PackagePager::GetRelativePosition()
{
    return 0;
}
std::pair<unsigned, unsigned> PackagePager::GetAbsolutePosition()
{
    return location;
}
