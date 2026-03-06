// Úkolem je realizovat třídu, která implementuje zjednodušenou variantu linkeru.
//
// Linker je součást kompilátoru, která má na vstupu object soubory, ze kterých vyrobí finální spustitelný program. Každý object soubor obsahuje část přeloženého zdrojového kódu, zpravidla jednoho modulu. Object soubory obsahují tabulky importovaných a exportovaných symbolů. Linker tyto object soubory načte, propojí požadované importy s dostupnými exporty, zkontroluje jejich správnost, dosadí vypočtené adresy a výsledný soubor uloží na disk. Ve zjednodušené podobě to bude práce pro implementovanou třídu.
//
// Základem object souboru je přeložený strojový kód. Ten obsahuje instrukce procesoru (v úloze uvažujeme pouze funkce). Bajty, které tvoří instrukce, jsou uložené v jednom soubislém bloku. V instrukcích ale mohou chybět některé adresy - bajty tvořící adresy zatím obsahují neplatné hodnoty, často nuly. Linker tyto bajty přepíše platnými adresami, které vzniknou po serializaci funkcí do výsledného souboru. Přeložený kód v naší úloze představuje posloupnost náhodných bajtů nějaké délky, nejedná se o platné instrukce nějakého procesoru.
//
// Linker potřebuje znát jména funkcí, které se v daném object souboru nachází. K tomu slouží tabulka exportů. V úloze má tabulka zjednodušenou strukturu - obsahuje pouze jméno funkce (symbolu) a její pozici v přeloženém kódu (offset, počítaný v bajtech od počátku bloku přeloženého kódu). Object soubor může obsahovat více funkcí, funkce se v přeloženém kódu nepřekrývají. Proto lze snadno identifikovat začátek a konec funkce v přeloženém kódu; funkce končí tam, kde začíná funkce následující.
// Pozor, pořadí funkcí v tabulce exportů nemusí odpovídat pořadí funkcí v přeloženém kódu (viz obrázek).
//
// Linker dále potřebuje znát pozice v přeloženém kódu, kde bude nahrazovat odkazy na používané funkce. K tomu slouží tabulka importů. Pro každý importovaný symbol je uvedeno jeho jméno, počet výskytů a jednotlivé pozice v přeloženém kódu, kde má být adresa zapsána (opět ofset v bajtech od počátku přeloženého kódu).
//
// Linkování dostane na vstupu jméno počáteční funkce. Je to obdoba funkce main, ale jméno může být libovolný řetězec. Tato funkce bude zkopírovaná do výsledného souboru jako první, na jeho začátek. Zkopíruje se tělo této funkce, tedy odpovídající bajty přeloženého kódu. Tato funkce může potřebovat volat další funkce. To, které funkce to jsou, lze snadno poznat. Do těla "main" funkce budou odkazovat odkazy z tabulky importů, tedy tyto funkce jsou volané z "main".
// Do výsledného souboru tedy bude potřeba nakopírovat tyto odkazované funkce (v nějakém pořadí za první "main") a ve zkopírované main odpovídajícím způsobem vyplnit odkazy na tyto funkce. Celý postup je potřeba aplikovat opakovaně, protože každá zkopírovaná funkce může mít své další závislosti. Linkování se snaží, aby výsledný soubor byl co nejkratší. Tedy stejnou funkci nekopíruje opakovaně a mezi funkce nevkládá žádné bajty navíc. Dále, do výsledného souboru nejsou ukládané nedostupné funkce.
// Pořadí funkcí ve výsledném souboru není určeno (zvolte si), ale první musí být funkce odpovídající zadanému vstupnímu bodu ("main").
//
// Pro zjednodušení předpokládáme, že všechny adresy mají velikost 32 bitů, jsou kódované v little-endian pořadí bajtů (intel) a všechny adresy ve výsledném souboru jsou absolutní (pozice od začátku výstupního souboru).

// Požadovaná třída CLinker má rozhraní:
//
// implicitní konstruktor, destruktor
// pracují v obvyklém významu

// kopírovací konstruktor a operátor =
// nejsou používané, mohou být potlačené (=delete).

// addFile(objFileName)
// přidá další vstupní object soubor, jméno souboru je udané parametrem. Metoda soubor může načíst, případně si jen jméno souboru poznamená pro další použití. Metoda může vyhodit výjimku std::runtime_error, pokud v ní načítáte object soubory a čtení selže.

// linkOutput ( outFileName, entryPoint )
// metoda fakticky vytvoří výsledný slinkovaný soubor outFileName. Druhý parametr udává jméno počáteční funkce (tj. ekvivalent funkce "main"). Metoda nic nevrací, případnou chybu oznamuje vyhozením výjimky std::runtime_error. Do popisu výjimky si můžete uložit příčinu problému (chybějící/duplicitní symbol, chyba I/O, ...). Testovací prostředí ale popis výjimky nijak nekontroluje.

#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

struct ExportedSymbol
{
    std::string name;
    uint32_t offset;
};

struct ImportedSymbol
{
    std::string name;
    std::vector<uint32_t> positions;
};

struct ObjectFile
{
    std::string fileName;
    std::vector<uint8_t> code;
    std::map<std::string, ExportedSymbol> exports;
    std::map<std::string, ImportedSymbol> imports;
};

struct FunctionLocation
{
    std::shared_ptr<ObjectFile> sourceFile;
    uint32_t offset;
    uint32_t size;
};

class CLinker
{
  public:
    CLinker() = default;

    CLinker & addFile(const std::string & fileName)
    {
        std::ifstream fin(fileName, std::ios::binary);

        if (!fin)
        {
            throw std::runtime_error("Cannot read input file");
        }

        std::shared_ptr<ObjectFile> obj = std::make_shared<ObjectFile>();
        obj->fileName = fileName;

        uint32_t exportCount = 0, importCount = 0, codeSize = 0;
        fin.read(reinterpret_cast<char*>(&exportCount), sizeof(exportCount));
        fin.read(reinterpret_cast<char*>(&importCount), sizeof(importCount));
        fin.read(reinterpret_cast<char*>(&codeSize), sizeof(codeSize));

        for (uint32_t i = 0; i < exportCount; i++)
        {
            uint8_t nameLen;
            fin.read(reinterpret_cast<char*>(&nameLen), 1);
            std::string name(nameLen, '\0');
            fin.read(&name[0], nameLen);

            uint32_t offset;
            fin.read(reinterpret_cast<char*>(&offset), sizeof(offset));

            if (globalExportMap.count(name) > 0)
            {
                throw std::runtime_error("Duplicate symbol: " + name);
            }

            obj->exports[name] = {name, offset};
            globalExportMap[name] = {obj, offset};
        }

        for (uint32_t i = 0; i < importCount; i++)
        {
            uint8_t nameLen;
            fin.read(reinterpret_cast<char*>(&nameLen), 1);
            std::string name(nameLen, '\0');
            fin.read(&name[0], nameLen);

            uint32_t count;
            fin.read(reinterpret_cast<char*>(&count), sizeof(count));

            ImportedSymbol import;
            import.name = name;

            for (uint32_t j = 0; j < count; j++)
            {
                uint32_t pos;
                fin.read(reinterpret_cast<char*>(&pos), sizeof(pos));
                import.positions.push_back(pos);
            }

            obj->imports[name] = import;
        }

        obj->code.resize(codeSize);
        fin.read(reinterpret_cast<char*>(obj->code.data()), codeSize);

        if (!fin)
        {
            throw std::runtime_error("Error while reading data");
        }

        objectFiles.push_back(obj);
        return *this;
    }

    void linkOutput(const std::string & fileName, const std::string & entryPoint)
    {
        auto entryExportIt = globalExportMap.find(entryPoint);
        if (entryExportIt == globalExportMap.end())
        {
            throw std::runtime_error("Undefined symbol " + entryPoint);
        }

        std::queue<std::string> functionsToProcess;
        std::map<std::string, FunctionLocation> functionsToInclude;
        std::set<std::string> processedFunctions;

        functionsToProcess.push(entryPoint);

        while (!functionsToProcess.empty())
        {
            std::string currentFunctionName = functionsToProcess.front();
            functionsToProcess.pop();

            if (processedFunctions.count(currentFunctionName) > 0)
                continue;

            processedFunctions.insert(currentFunctionName);

            auto exportIt = globalExportMap.find(currentFunctionName);
            if (exportIt == globalExportMap.end())
            {
                throw std::runtime_error("Undefined symbol " + currentFunctionName);
            }

            std::shared_ptr<ObjectFile> sourceFile = exportIt->second.first;
            uint32_t offset = exportIt->second.second;

            uint32_t size = calculateFunctionSize(sourceFile, offset);

            functionsToInclude[currentFunctionName] = {sourceFile, offset, size};

            auto& functionImports = sourceFile->imports;
            for (const auto& [importName, importData] : functionImports)
            {
                bool isImportedByThisFunction = false;
                for (const auto & pos : importData.positions)
                {
                    if (pos >= offset && pos < offset + size)
                    {
                        isImportedByThisFunction = true;
                        break;
                    }
                }

                if (isImportedByThisFunction && processedFunctions.count(importName) == 0)
                {
                    functionsToProcess.push(importName);
                }
            }
        }

        std::vector<uint8_t> outputCode;
        std::map<std::string, uint32_t> symbolAddresses;

        const auto& entryFunc = functionsToInclude[entryPoint];
        auto sourceFile = entryFunc.sourceFile;
        uint32_t offset = entryFunc.offset;
        uint32_t size = entryFunc.size;

        uint32_t outputOffset = outputCode.size();
        symbolAddresses[entryPoint] = outputOffset;

        outputCode.insert(outputCode.end(),
                        sourceFile->code.begin() + offset,
                        sourceFile->code.begin() + offset + size);

        for (const auto & [funcName, funcLoc] : functionsToInclude)
        {
            if (funcName == entryPoint) continue;

            auto sourceFile = funcLoc.sourceFile;
            uint32_t offset = funcLoc.offset;
            uint32_t size = funcLoc.size;

            uint32_t outputOffset = outputCode.size();
            symbolAddresses[funcName] = outputOffset;

            outputCode.insert(outputCode.end(), sourceFile->code.begin() + offset, sourceFile->code.begin() + offset + size);
        }

        for (const auto & [funcName, funcLoc] : functionsToInclude)
        {
            auto sourceFile = funcLoc.sourceFile;
            uint32_t srcOffset = funcLoc.offset;
            uint32_t outOffset = symbolAddresses[funcName];

            for (const auto & [importName, importData] : sourceFile->imports)
            {
                if (symbolAddresses.count(importName) == 0) continue;

                uint32_t importAddress = symbolAddresses[importName];

                for (const auto & pos : importData.positions)
                {
                    if (pos >= srcOffset && pos < srcOffset + funcLoc.size)
                    {
                        uint32_t outPos = outOffset + (pos - srcOffset);
                        memcpy(&outputCode[outPos], &importAddress, sizeof(importAddress));
                    }
                }
            }
        }

        std::ofstream fout(fileName, std::ios::binary);
        if (!fout)
        {
            throw std::runtime_error("Unable to write to output file");
        }

        fout.write(reinterpret_cast<const char*>(outputCode.data()), outputCode.size());
        if (!fout)
        {
            throw std::runtime_error("Error while writing output data");
        }
    }

  private:
    std::map<std::string, std::pair<std::shared_ptr<ObjectFile>, uint32_t>> globalExportMap;
    std::vector<std::shared_ptr<ObjectFile>> objectFiles;

    uint32_t calculateFunctionSize(std::shared_ptr<ObjectFile> file, uint32_t offset)
    {
        uint32_t nextOffset = file->code.size();

        for (const auto & [name, symbol] : file->exports)
        {
            if (symbol.offset > offset && symbol.offset < nextOffset)
            {
                nextOffset = symbol.offset;
            }
        }

        return nextOffset - offset;
    }
};

#ifndef __PROGTEST__
int main ()
{
  CLinker () . addFile ( "0in0.o" ) . linkOutput ( "0out", "strlen" );

  CLinker () . addFile ( "1in0.o" ) . linkOutput ( "1out", "main" );

  CLinker () . addFile ( "2in0.o" ) . addFile ( "2in1.o" ) . linkOutput ( "2out", "main" );

  CLinker () . addFile ( "3in0.o" ) . addFile ( "3in1.o" ) . linkOutput ( "3out", "towersOfHanoi" );

  try
  {
    CLinker () . addFile ( "4in0.o" ) . addFile ( "4in1.o" ) . linkOutput ( "4out", "unusedFunc" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol qsort
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "5in0.o" ) . linkOutput ( "5out", "main" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Duplicate symbol: printf
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "6in0.o" ) . linkOutput ( "6out", "strlen" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Cannot read input file
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "7in0.o" ) . linkOutput ( "7out", "strlen2" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol strlen2
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

