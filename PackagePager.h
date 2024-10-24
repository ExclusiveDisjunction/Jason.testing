//
// Created by exdisj on 10/21/24.
//

#ifndef JASON_PACKAGEPAGER_H
#define JASON_PACKAGEPAGER_H

#include "FileHandle.h"
#include "BinaryUnit.h"
//#include "PackageEntryIndex.h"

#include <vector>
#include <optional>

class PackagePager
{
private:
    FileHandle handle;
    std::optional<std::vector<unsigned>> binding;
    std::optional<unsigned long> boundPageIndex;
    std::pair<unsigned, unsigned> location;

    bool boundEof = false;

    unsigned char unitSize;
    unsigned pageSize;

public:
    PackagePager(std::filesystem::path location, unsigned char UnitSize, unsigned PageSize);
    PackagePager(const PackagePager& obj) = delete;
    PackagePager(PackagePager&& obj) noexcept;
    ~PackagePager();

    PackagePager& operator=(const PackagePager& obj) = delete;
    PackagePager& operator=(PackagePager&& obj) noexcept;

    [[nodiscard]] [[maybe_unused]] unsigned char UnitSize() const noexcept;
    [[nodiscard]] [[maybe_unused]] unsigned PageSize() const noexcept;

    Unit ReadUnit();
    bool Advance();
    bool AdvancePage();
    std::vector<Unit> ReadUnits(unsigned int Units);
    std::vector<Unit> ReadAllUnits();
    bool WriteUnits(const std::vector<Unit>& units);
    bool WipeAll();

    //bool Allocate(unsigned pages, const PackageEntryIndex& index);

    //void Bind(const PackageEntryIndex& index);
    void Bind(std::vector<unsigned> pages);
    void Reset();
    void Close();
    void Flush();

    bool EndOfFile() const noexcept;
    bool IsBound() const noexcept;

    bool MoveRelative(unsigned unitPosition);
    bool MoveAbsolute(unsigned pagePosition, unsigned unitPosition);
    bool MoveAbsolute(std::pair<unsigned, unsigned> loc);
    unsigned GetRelativePosition();
    std::pair<unsigned, unsigned> GetAbsolutePosition();

    //bool IsFragmented();
    //bool IsFragemented(const PackageEntryIndex& target);
    //void Defragment(const std::vector<class PackageEntry>& entries);
};


#endif //JASON_PACKAGEPAGER_H
