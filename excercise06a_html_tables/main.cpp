// Úkolem je realizovat sadu tříd, které budou implementovat tabulky podobné tabulkám v HTML.
//
// Předpokládáme, že naše zjednodušená tabulka CTable je tvořená zadaným počtem řádků a sloupců, ty jsou vyplněné při vytváření instance. V každé buňce je buď prázdno (CEmpty), text (CText) nebo obrázek (CImage). Tabulka dokáže měnit obsah svých buněk a dokáže se zobrazit. Při zobrazení se tabulka velikostí přizpůsobí obsahu svých buněk (velikost se roztáhne tak, aby v žádné buňce nedošlo k ořezání).
//
// Očekává se, že implementujete následující třídy s následujícím rozhraním:
//
// CEmpty
// Třída reprezentuje prázdnou buňku. Jejím rozhraním je pouze implicitní konstruktor, který slouží k vytvoření instance.

// CText
// Třída reprezentuje text. Text je zobrazen v příslušné buňce tabulky. Při zobrazování se zohledňují zadané konce řádků. Zobrazení lze zvolit horizontálně zarovnané vlevo nebo vpravo (align). Vertikálně je vždy text zarovnán nahoru. Rozhraní obsahuje:
//
// konstruktor CText (str, align)
// konstruktor inicializuje instanci třídy. Parametrem je zobrazovaný text a zarovnání (ALIGN_LEFT / ALIGN_RIGHT).
//
// metoda setText (str)
// metodou lze nastavit jiný text, který má být zobrazován. Zarovnání se nemění.

// CImage
// Třída reprezentuje obrázek. Obrázek je pro jednoduchost reprezentován v ASCII artu, je zadaný jako několik řádků - řetězců. Při zobrazování je obrázek umístěn do požadované buňky a je zarovnán na střed (horizontálně i vertikálně). Pokud by zarovnání na střed nebylo možné (lichý počet mezer v nějakém směru), bude obrázek posunut o jednu pozici vlevo a/nebo nahoru. Rozhraní obsahuje:
//
// konstruktor CImage ()
// konstruktor inicializuje instanci třídy.
//
// metoda addRow (str)
// metodou lze přidat další řádku obrázku. Můžete se spolehnout, že zadávané řádky mají vždy stejnou délku pro celý obrázek (tedy obrázek je vždy obdélníkový).
// .
// Všechny třídy dále budou implementovat porovnnání na přesnou shodu/neshodu pomocí operátorů == a !=.

// CTable
// třída reprezentuje tabulku. Do buněk tabulky se vkládá obsah (volno/text/obrázek). Tabulka nabízí rozhraní, které umožní manipulovat s obsahem buněk a zobrazit celou tabulku:
//
// konstruktor (rows,cols)
// inicializuje instanci a nastaví požadovaný počet řádek a sloupců tabulky. Takto inicializovaná tabulka bude mít všechny buňky prázdné.
//
// getCell (row, col)
// metoda zpřístupní buňku v tabulce pro další operace (nastavení vlastností obsaženého textu/obrázku/... či čtení obsahu buňky.
//
// setCell (row, col, newContent)
// metoda změní obsah zadané buňky tabulky na zadaný nový obsah.
//
// operator ==, !=
// umožní porovnat dvě tabulky na přesnou shodu/neshodu.
//
// operator <<
// umožní výpis zformátované tabulky do zadaného výstupního proudu.


#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST__ */

class CElement
{
public:
    virtual ~CElement() noexcept = default;
    virtual CElement * clone () const = 0;

    virtual int getWidth () const = 0;
    virtual int getHeight () const = 0;
    virtual std::vector<std::string> renderLines (int height, int width) const = 0;
    virtual bool equals (const CElement & other) const = 0;

    bool operator == (const CElement & other) const
    {
        return equals(other);
    }

    bool operator != (const CElement & other) const
    {
        return !equals(other);
    }
};

class CEmpty : public CElement
{
public:
    CEmpty () : CElement() {}

    CElement * clone () const override
    {
        return new CEmpty (*this);
    }

    int getWidth() const override
    {
        int maxWidth = 0;
        for (const auto & line : m_lines)
        {
            maxWidth = std::max(maxWidth, static_cast<int>(line.length()));
        }
        return maxWidth;
    }

    int getHeight() const override
    {
        return static_cast<int>(m_lines.size());
    }

    std::vector<std::string> renderLines(int height, int width) const override
    {
        return std::vector<std::string> (height, std::string(width, ' '));
    }

    bool equals(const CElement & other) const override
    {
        return dynamic_cast<const CEmpty*>(&other) != nullptr;
    }

protected:
    std::vector<std::string> m_lines;
};

class CText : public CElement
{
public:
    enum Align
    {
        ALIGN_LEFT,
        ALIGN_RIGHT
    };

    CText (const std::string & text, const Align & alignment) : CElement(), m_text(text), m_alignment(alignment)
    {
        splitLines();
    }

    CElement * clone () const override
    {
        return new CText (*this);
    }

    void setText (const std::string & text)
    {
        m_text = text;
        splitLines();
    }

    int getWidth() const override
    {
        int maxWidth = 0;
        for (const auto & line : m_lines)
        {
            maxWidth = std::max(maxWidth, static_cast<int>(line.length()));
        }
        return maxWidth;
    }

    int getHeight() const override
    {
        return static_cast<int>(m_lines.size());
    }

    std::vector<std::string> renderLines (int height, int width) const override
    {
        std::vector<std::string> result(height, std::string(width, ' '));
        for (size_t i = 0; i < m_lines.size(); i++)
        {
            if (m_alignment == ALIGN_LEFT)
            {
                result[i].replace(0, m_lines[i].length(), m_lines[i]);
            }
            else
            {
                result[i].replace(width - m_lines[i].length(), m_lines[i].length(), m_lines[i]);
            }
        }
        return result;
    }

    bool equals(const CElement & other) const override
    {
        const CText * ptr = dynamic_cast<const CText*>(&other);
        return ptr && m_text == ptr->m_text && m_alignment == ptr->m_alignment;
    }

protected:
    std::string m_text;
    Align m_alignment;
    std::vector<std::string> m_lines;

    void splitLines ()
    {
        m_lines.clear();
        std::istringstream iss(m_text);
        std::string line;
        while (getline(iss, line))
        {
            m_lines.push_back(line);
        }
    }

};

class CImage : public CElement
{
public:
    CImage () : CElement() {}

    CElement * clone () const override
    {
        return new CImage (*this);
    }

    CImage & addRow (const std::string & imageRow)
    {
        m_image.push_back(imageRow);
        return *this;
    }

    int getWidth() const override
    {
        int maxWidth = 0;
        for (const auto & row : m_image)
        {
            maxWidth = std::max(maxWidth, static_cast<int>(row.length()));
        }
        return maxWidth;
    }

    int getHeight() const override
    {
        return static_cast<int>(m_image.size());
    }

    std::vector<std::string> renderLines(int height, int width) const override
    {
        std::vector<std::string> result(height, std::string(width, ' '));

        int verticalOffset = (height - m_image.size()) / 2;

        for (size_t i = 0; i < m_image.size(); i++)
        {
            int horizontalOffset = (width - m_image[i].length()) / 2;

            result[i + verticalOffset].replace(horizontalOffset, m_image[i].length(), m_image[i]);
        }

        return result;
    }

    bool equals(const CElement & other) const override
    {
        const CImage * ptr = dynamic_cast<const CImage*>(&other);
        return ptr && m_image == ptr->m_image;
    }

protected:
    std::vector<std::string> m_image;
};

class CTable
{
public:
    CTable (int height, int width) : m_height(height), m_width(width)
    {
        m_table.resize(height);
        for (int i = 0; i < height; i++)
        {
            m_table[i].resize(width);
            for (int j = 0; j < width; j++)
            {
                m_table[i][j] = std::make_unique<CEmpty>();
            }
        }
    }

    CTable (const CTable & other) : m_height(other.m_height), m_width(other.m_width)
    {
        m_table.resize(m_height);
        for (int i = 0; i < m_height; i++)
        {
            m_table[i].resize(m_width);
            for (int j = 0; j < m_width; j++)
            {
                m_table[i][j].reset(other.m_table[i][j]->clone());
            }
        }
    }

    CTable & operator = (const CTable & other)
    {
        if (this != &other)
        {
            CTable temp (other);
            std::swap(m_height, temp.m_height);
            std::swap(m_width, temp.m_width);
            std::swap(m_table, temp.m_table);
        }
        return *this;
    }

    void setCell (int height, int width, const CElement & element)
    {
        m_table[height][width].reset(element.clone());
    }

    void setCell(int height, int width, CElement * element)
    {
        if (element)
        {
            m_table[height][width].reset(element->clone());
        }
        else
        {
            m_table[height][width].reset(new CEmpty());
        }
    }

    CElement & getCell(int height, int width) const
    {
        return *m_table[height][width];
    }

    std::vector<int> getColWidths () const
    {
        std::vector<int> widths(m_width, 0);
        for (int col = 0; col < m_width; col++)
        {
            for (int row = 0; row < m_height; row++)
            {
                widths[col] = std::max(widths[col], m_table[row][col]->getWidth());
            }
        }
        return widths;
    }

    std::vector<int> getRowHeights () const
    {
        std::vector<int> heights(m_height, 0);
        for (int row = 0; row < m_height; row++)
        {
            for (int col = 0; col < m_width; col++)
            {
                heights[row] = std::max(heights[row], m_table[row][col]->getHeight());
            }
        }
        return heights;
    }

    void printRowSeparator (std::ostream & out, const std::vector<int> & colWidths) const
    {
        out << '+';
        for (int width : colWidths)
        {
            out << std::string(width, '-') << '+';
        }
        out << std::endl;
    }

    friend std::ostream & operator << (std::ostream & out, const CTable & table);

    bool operator == (const CTable & other) const
    {
        if (m_height != other.m_height || m_width != other.m_width)
        {
            return false;
        }

        for (int row = 0; row < m_height; row++)
        {
            for (int col = 0; col < m_width; col++)
            {
                if (!m_table[row][col]->equals(*other.m_table[row][col]))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool operator != (const CTable & other) const
    {
        return !(*this == other);
    }



protected:
    std::vector<std::vector<std::unique_ptr<CElement>>> m_table;
    int m_height, m_width;
};

std::ostream & operator<< (std::ostream & out, const CTable & table)
{
    std::vector<int> colWidths = table.getColWidths();
    std::vector<int> rowHeights = table.getRowHeights();

    for (int row = 0; row < table.m_height; row++)
    {
        table.printRowSeparator(out, colWidths);

        std::vector<std::vector<std::string>> renderedCells;

        for (int col = 0; col < table.m_width; col++)
        {
            renderedCells.push_back(table.m_table[row][col]->renderLines(rowHeights[row], colWidths[col]));
        }

        for (int line = 0; line < rowHeights[row]; line++)
        {
            out << '|';
            for (int col = 0; col < table.m_width; col++)
            {
                out << renderedCells[col][line] << '|';
            }
            out << std::endl;
        }
    }

    table.printRowSeparator(out, colWidths);
    return out;
}

#ifndef __PROGTEST__
int main ()
{
  std::ostringstream oss;
  CTable t0 ( 3, 2 );
  t0 . setCell ( 0, 0, CText ( "Hello,\n"
        "Hello Kitty", CText::ALIGN_LEFT ) );
  t0 . setCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
  t0 . setCell ( 2, 0, CText ( "Bye,\n"
        "Hello Kitty", CText::ALIGN_RIGHT ) );
  t0 . setCell ( 1, 1, CImage ()
        . addRow ( "###                   " )
        . addRow ( "#  #                  " )
        . addRow ( "#  # # ##   ###    ###" )
        . addRow ( "###  ##    #   #  #  #" )
        . addRow ( "#    #     #   #  #  #" )
        . addRow ( "#    #     #   #  #  #" )
        . addRow ( "#    #      ###    ###" )
        . addRow ( "                     #" )
        . addRow ( "                   ## " )
        . addRow ( "                      " )
        . addRow ( " #    ###   ###   #   " )
        . addRow ( "###  #   # #     ###  " )
        . addRow ( " #   #####  ###   #   " )
        . addRow ( " #   #         #  #   " )
        . addRow ( "  ##  ###   ###    ## " ) );
  t0 . setCell ( 2, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+--------------------------+----------------------+\n"
        "|Hello,                    |                      |\n"
        "|Hello Kitty               |                      |\n"
        "+--------------------------+----------------------+\n"
        "|Lorem ipsum dolor sit amet|###                   |\n"
        "|                          |#  #                  |\n"
        "|                          |#  # # ##   ###    ###|\n"
        "|                          |###  ##    #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #      ###    ###|\n"
        "|                          |                     #|\n"
        "|                          |                   ## |\n"
        "|                          |                      |\n"
        "|                          | #    ###   ###   #   |\n"
        "|                          |###  #   # #     ###  |\n"
        "|                          | #   #####  ###   #   |\n"
        "|                          | #   #         #  #   |\n"
        "|                          |  ##  ###   ###    ## |\n"
        "+--------------------------+----------------------+\n"
        "|                      Bye,|                      |\n"
        "|               Hello Kitty|                      |\n"
        "+--------------------------+----------------------+\n" );
  t0 . setCell ( 0, 1, t0 . getCell ( 1, 1 ) );
  t0 . setCell ( 2, 1, CImage ()
        . addRow ( "*****   *      *  *      ******* ******  *" )
        . addRow ( "*    *  *      *  *      *            *  *" )
        . addRow ( "*    *  *      *  *      *           *   *" )
        . addRow ( "*    *  *      *  *      *****      *    *" )
        . addRow ( "****    *      *  *      *         *     *" )
        . addRow ( "*  *    *      *  *      *        *       " )
        . addRow ( "*   *   *      *  *      *       *       *" )
        . addRow ( "*    *    *****   ****** ******* ******  *" ) );
  dynamic_cast<CText &> ( t0 . getCell ( 1, 0 ) ) . setText ( "Lorem ipsum dolor sit amet,\n"
        "consectetur adipiscing\n"
        "elit. Curabitur scelerisque\n"
        "lorem vitae lectus cursus,\n"
        "vitae porta ante placerat. Class aptent taciti\n"
        "sociosqu ad litora\n"
        "torquent per\n"
        "conubia nostra,\n"
        "per inceptos himenaeos.\n"
        "\n"
        "Donec tincidunt augue\n"
        "sit amet metus\n"
        "pretium volutpat.\n"
        "Donec faucibus,\n"
        "ante sit amet\n"
        "luctus posuere,\n"
        "mauris tellus" );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  CTable t1 ( t0 );
  t1 . setCell ( 1, 0, CEmpty () );
  t1 . setCell ( 1, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-----------+------------------------------------------+\n"
        "|Hello,     |          ###                             |\n"
        "|Hello Kitty|          #  #                            |\n"
        "|           |          #  # # ##   ###    ###          |\n"
        "|           |          ###  ##    #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #      ###    ###          |\n"
        "|           |                               #          |\n"
        "|           |                             ##           |\n"
        "|           |                                          |\n"
        "|           |           #    ###   ###   #             |\n"
        "|           |          ###  #   # #     ###            |\n"
        "|           |           #   #####  ###   #             |\n"
        "|           |           #   #         #  #             |\n"
        "|           |            ##  ###   ###    ##           |\n"
        "+-----------+------------------------------------------+\n"
        "+-----------+------------------------------------------+\n"
        "|       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|           |*    *  *      *  *      *           *   *|\n"
        "|           |*    *  *      *  *      *****      *    *|\n"
        "|           |****    *      *  *      *         *     *|\n"
        "|           |*  *    *      *  *      *        *       |\n"
        "|           |*   *   *      *  *      *       *       *|\n"
        "|           |*    *    *****   ****** ******* ******  *|\n"
        "+-----------+------------------------------------------+\n" );
  t1 = t0;
  t1 . setCell ( 0, 0, CEmpty () );
  t1 . setCell ( 1, 1, CImage ()
        . addRow ( "  ********                    " )
        . addRow ( " **********                   " )
        . addRow ( "**        **                  " )
        . addRow ( "**             **        **   " )
        . addRow ( "**             **        **   " )
        . addRow ( "***         ********  ********" )
        . addRow ( "****        ********  ********" )
        . addRow ( "****           **        **   " )
        . addRow ( "****           **        **   " )
        . addRow ( "****      **                  " )
        . addRow ( " **********                   " )
        . addRow ( "  ********                    " ) );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                              |          ###                             |\n"
        "|                                              |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  assert ( t0 != t1 );
  assert ( !( t0 == t1 ) );
  assert ( t0 . getCell ( 1, 1 ) == t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 1, 1 ) != t0 . getCell ( 0, 1 ) ) );
  assert ( t0 . getCell ( 0, 0 ) != t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 0, 0 ) == t0 . getCell ( 0, 1 ) ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
