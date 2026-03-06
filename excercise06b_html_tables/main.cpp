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

/**
 * Implementation of HTML-like table classes for displaying formatted content.
 *
 * This file contains a set of classes that implement tables similar to HTML tables.
 * The table is composed of a specified number of rows and columns, with each cell
 * containing either empty space (CEmpty), text (CText), or an image (CImage).
 * The table can modify the content of its cells and can display itself.
 * When displayed, the table adapts its size to fit the content of its cells.
 */

/**
 * Abstract base class for all elements that can be placed in table cells.
 *
 * This class defines the interface for all elements that can be placed in table cells.
 * It provides methods for cloning, getting dimensions, rendering, and comparing elements.
 */

class CElement
{
public:
    /**
     * Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~CElement() noexcept = default;

    /**
     * Creates a deep copy of the element.
     *
     * @returns A pointer to a newly allocated copy of the element.
     */
    virtual CElement * clone () const = 0;

    /**
     * Gets the width of the element.
     *
     * @returns The width of the element in characters.
     */
    virtual int getWidth () const = 0;

    /**
     * Gets the height of the element.
     *
     * @returns The height of the element in lines.
     */
    virtual int getHeight () const = 0;

    /**
     * Renders the element into a vector of strings with specified dimensions.
     *
     * @param[in] height  The required height of the rendered element.
     * @param[in] width   The required width of the rendered element.
     *
     * @returns A vector of strings representing the rendered element.
     */
    virtual std::vector<std::string> renderLines (int height, int width) const = 0;

    /**
     * Compares this element with another for equality.
     *
     * @param[in] other  The element to compare with.
     *
     * @returns True if the elements are equal, false otherwise.
     */
    virtual bool equals (const CElement & other) const = 0;

    /**
     * Equality operator overload.
     *
     * @param[in] other  The element to compare with.
     *
     * @returns True if the elements are equal, false otherwise.
     */
    bool operator == (const CElement & other) const
    {
        return equals(other);
    }

    /**
     * Inequality operator overload.
     *
     * @param[in] other  The element to compare with.
     *
     * @returns True if the elements are not equal, false otherwise.
     */
    bool operator != (const CElement & other) const
    {
        return !equals(other);
    }
};

/**
 * Class representing an empty cell in a table.
 *
 * This class is used to represent cells without any content.
 * It implements the CElement interface with empty content.
 */
class CEmpty : public CElement
{
public:
    /**
     * Default constructor initializes an empty element.
     */
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
    std::vector<std::string> m_lines;     // Vector of strings representing the lines of the empty element, typically empty.
};

/**
 * Class representing a text element in a table cell.
 *
 * This class is used to represent cells containing text content.
 * The text can be aligned left or right horizontally, and is always aligned
 * to the top vertically. Line breaks in the text are preserved.
 */
class CText : public CElement
{
public:
    /**
     * Enumeration defining text alignment options.
     */
    enum Align
    {
        ALIGN_LEFT,
        ALIGN_RIGHT
    };

    /**
     * Constructor initializes a text element with the specified text and alignment.
     *
     * @param[in] text       The text content to display.
     * @param[in] alignment  The horizontal alignment of the text (left or right).
     */
    CText (const std::string & text, const Align & alignment) : CElement(), m_text(text), m_alignment(alignment)
    {
        splitLines();
    }

    CElement * clone () const override
    {
        return new CText (*this);
    }

    /**
     * Sets new text content for the element.
     *
     * @param[in] text  The new text content to display.
     */
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
    std::string m_text;                // The text content to display.
    Align m_alignment;                 // The horizontal alignment of the text.
    std::vector<std::string> m_lines;  // Vector of strings representing the lines of the text.

    /**
     * Splits the text into lines based on newline characters.
     *
     * This helper method parses the text and splits it into individual lines
     * that are stored in the m_lines vector for further processing.
     */
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

/**
 * Class representing an image element in a table cell.
 *
 * This class is used to represent cells containing ASCII art images.
 * The image is centered both horizontally and vertically in the cell.
 * If perfect centering is not possible, the image is shifted one position
 * left and/or up.
 */
class CImage : public CElement
{
public:
    /**
     * Default constructor initializes an empty image element.
     */
    CImage () : CElement() {}

    CElement * clone () const override
    {
        return new CImage (*this);
    }

    /**
     * Adds a row to the image.
     *
     * @param[in] imageRow  The string representing a row of the ASCII art image.
     *
     * @returns Reference to this image element (for method chaining).
     */
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
    std::vector<std::string> m_image; // Vector of strings representing the rows of the ASCII art image.
};

/**
 * Class representing a table with rows and columns of cells.
 *
 * This class implements a table similar to HTML tables, with cells that can
 * contain different types of elements (empty, text, image or another table). The table
 * automatically adjusts its size to accommodate the content of its cells.
 */
class CTable : public CElement
{
public:
    /**
     * Constructor initializes a table with the specified number of rows and columns.
     * All cells are initially empty.
     *
     * @param[in] height  The number of rows in the table.
     * @param[in] width   The number of columns in the table.
     */
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

    /**
     * Assignment operator creates a deep copy of another table.
     *
     * @param[in] other  The table to copy.
     *
     * @returns Reference to this table.
     */
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

    CElement * clone() const override
    {
        return new CTable(*this);
    }

    /**
     * Sets the content of a cell to a copy of the specified element.
     *
     * @param[in] height   The row index of the cell to set.
     * @param[in] width    The column index of the cell to set.
     * @param[in] element  The element to copy into the cell.
     */
    void setCell (int height, int width, const CElement & element)
    {
        m_table[height][width].reset(element.clone());
    }

    /**
     * Sets the content of a cell to a copy of the specified element or to empty if null.
     *
     * @param[in] height   The row index of the cell to set.
     * @param[in] width    The column index of the cell to set.
     * @param[in] element  Pointer to the element to copy into the cell, or nullptr for empty.
     */
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

    /**
     * Gets a reference to the element in a specified cell.
     *
     * @param[in] height  The row index of the cell.
     * @param[in] width   The column index of the cell.
     *
     * @returns Reference to the element in the specified cell.
     */
    CElement & getCell(int height, int width) const
    {
        return *m_table[height][width];
    }

    int getWidth() const override
    {
        std::stringstream ss;
        ss << *this;
        std::string line;
        int maxWidth = 0;
        while (getline(ss, line))
        {
            maxWidth = std::max(maxWidth, static_cast<int>(line.length()));
        }
        return maxWidth;
    }

    int getHeight() const override
    {
        std::stringstream ss;
        ss << *this;
        std::string line;
        int height = 0;
        while (getline(ss, line))
        {
            height++;
        }
        return height;
    }

    std::vector<std::string> renderLines(int height, int width) const override
    {
        std::vector<std::string> result(height, std::string(width, ' '));

        std::stringstream ss;
        ss << *this;
        std::string line;
        int i = 0;
        while (i < height && getline(ss, line))
        {
            result[i].replace(0, line.length(), line);
            i++;
        }
        return result;
    }

    bool equals(const CElement& other) const override
    {
        const CTable * ptr = dynamic_cast<const CTable*>(&other);
        return ptr && *this == *ptr;
    }

    /**
     * Calculates the width of each column in the table.
     *
     * @returns A vector containing the width of each column.
     */
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

    /**
     * Calculates the height of each row in the table.
     *
     * @returns A vector containing the height of each row.
     */
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

    /**
     * Prints a horizontal separator row for the table.
     *
     * @param[in,out] out        The output stream to print to.
     * @param[in]     colWidths  The widths of each column.
     */
    void printRowSeparator (std::ostream & out, const std::vector<int> & colWidths) const
    {
        out << '+';
        for (int width : colWidths)
        {
            out << std::string(width, '-') << '+';
        }
        out << std::endl;
    }

    /**
     * Output stream operator for printing the table.
     *
     * @param[in,out] out    The output stream to print to.
     * @param[in]     table  The table to print.
     *
     * @returns The output stream.
     */
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
    std::vector<std::vector<std::unique_ptr<CElement>>> m_table;     // Two-dimensional vector of unique pointers to elements representing the table cells.
    int m_height;                                                    // The number of rows in the table.
    int m_width;                                                     // The number of columns in the table.
};

/**
 * Output stream operator for printing a table.
 *
 * This function formats a table for output, adjusting the size of each cell
 * to accommodate its content and draws borders between cells.
 *
 * @param[in,out] out    The output stream to print to.
 * @param[in]     table  The table to print.
 *
 * @returns The output stream.
 */
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

/**
 * Main function for testing the CTable implementation.
 *
 * This function tests various functionalities of the table classes:
 * - Creating tables
 * - Setting and retrieving cell contents
 * - Rendering tables with different cell types
 * - Testing equality and assignment operations
 * - Setting a table as the content of a cell
 *
 * @returns 0 on successful completion of all tests.
 */
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
  CTable t2 ( 2, 2 );
  t2 . setCell ( 0, 0, CText ( "OOP", CText::ALIGN_LEFT ) );
  t2 . setCell ( 0, 1, CText ( "Encapsulation", CText::ALIGN_LEFT ) );
  t2 . setCell ( 1, 0, CText ( "Polymorphism", CText::ALIGN_LEFT ) );
  t2 . setCell ( 1, 1, CText ( "Inheritance", CText::ALIGN_LEFT ) );
  oss . str ("");
  oss . clear ();
  oss << t2;
  assert ( oss . str () ==
        "+------------+-------------+\n"
        "|OOP         |Encapsulation|\n"
        "+------------+-------------+\n"
        "|Polymorphism|Inheritance  |\n"
        "+------------+-------------+\n" );
  t1 . setCell ( 0, 0, t2 );
  dynamic_cast<CText &> ( t2 . getCell ( 0, 0 ) ) . setText ( "Object Oriented Programming" );
  oss . str ("");
  oss . clear ();
  oss << t2;
  assert ( oss . str () ==
        "+---------------------------+-------------+\n"
        "|Object Oriented Programming|Encapsulation|\n"
        "+---------------------------+-------------+\n"
        "|Polymorphism               |Inheritance  |\n"
        "+---------------------------+-------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|+------------+-------------+                  |          ###                             |\n"
        "||OOP         |Encapsulation|                  |          #  #                            |\n"
        "|+------------+-------------+                  |          #  # # ##   ###    ###          |\n"
        "||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          |\n"
        "|+------------+-------------+                  |          #    #     #   #  #  #          |\n"
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
  t1 . setCell ( 0, 0, t1 );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||+------------+-------------+                  |          ###                             ||                                          |\n"
        "|||OOP         |Encapsulation|                  |          #  #                            ||                                          |\n"
        "||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          |\n"
        "|||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          |\n"
        "||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #      ###    ###          ||                                          |\n"
        "||                                              |                               #          ||                                          |\n"
        "||                                              |                             ##           ||                                          |\n"
        "||                                              |                                          ||                                          |\n"
        "||                                              |           #    ###   ###   #             ||                                          |\n"
        "||                                              |          ###  #   # #     ###            ||                                          |\n"
        "||                                              |           #   #####  ###   #             ||                                          |\n"
        "||                                              |           #   #         #  #             ||          ###                             |\n"
        "||                                              |            ##  ###   ###    ##           ||          #  #                            |\n"
        "|+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          |\n"
        "||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          |\n"
        "||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          |\n"
        "||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          |\n"
        "||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          |\n"
        "||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          |\n"
        "||sociosqu ad litora                            |      **             **        **         ||                             ##           |\n"
        "||torquent per                                  |      **             **        **         ||                                          |\n"
        "||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             |\n"
        "||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            |\n"
        "||                                              |      ****           **        **         ||           #   #####  ###   #             |\n"
        "||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             |\n"
        "||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           |\n"
        "||pretium volutpat.                             |       **********                         ||                                          |\n"
        "||Donec faucibus,                               |        ********                          ||                                          |\n"
        "||ante sit amet                                 |                                          ||                                          |\n"
        "||luctus posuere,                               |                                          ||                                          |\n"
        "||mauris tellus                                 |                                          ||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                          Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                              |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                              |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                              |****    *      *  *      *         *     *||                                          |\n"
        "||                                              |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                              |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                              |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                |                                          |\n"
        "|consectetur adipiscing                                                                     |                                          |\n"
        "|elit. Curabitur scelerisque                                                                |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                 |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                             |      **        **                        |\n"
        "|sociosqu ad litora                                                                         |      **             **        **         |\n"
        "|torquent per                                                                               |      **             **        **         |\n"
        "|conubia nostra,                                                                            |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                    |      ****        ********  ********      |\n"
        "|                                                                                           |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                      |      ****           **        **         |\n"
        "|sit amet metus                                                                             |      ****      **                        |\n"
        "|pretium volutpat.                                                                          |       **********                         |\n"
        "|Donec faucibus,                                                                            |        ********                          |\n"
        "|ante sit amet                                                                              |                                          |\n"
        "|luctus posuere,                                                                            |                                          |\n"
        "|mauris tellus                                                                              |                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                           |*    *  *      *  *      *           *   *|\n"
        "|                                                                                           |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                           |****    *      *  *      *         *     *|\n"
        "|                                                                                           |*  *    *      *  *      *        *       |\n"
        "|                                                                                           |*   *   *      *  *      *       *       *|\n"
        "|                                                                                           |*    *    *****   ****** ******* ******  *|\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n" );
  t1 . setCell ( 0, 0, t1 );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|||+------------+-------------+                  |          ###                             ||                                          ||                                          |\n"
        "||||OOP         |Encapsulation|                  |          #  #                            ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          ||                                          |\n"
        "||||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #      ###    ###          ||                                          ||                                          |\n"
        "|||                                              |                               #          ||                                          ||                                          |\n"
        "|||                                              |                             ##           ||                                          ||                                          |\n"
        "|||                                              |                                          ||                                          ||                                          |\n"
        "|||                                              |           #    ###   ###   #             ||                                          ||                                          |\n"
        "|||                                              |          ###  #   # #     ###            ||                                          ||                                          |\n"
        "|||                                              |           #   #####  ###   #             ||                                          ||                                          |\n"
        "|||                                              |           #   #         #  #             ||          ###                             ||                                          |\n"
        "|||                                              |            ##  ###   ###    ##           ||          #  #                            ||                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          ||                                          |\n"
        "|||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          ||                                          |\n"
        "|||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          ||                                          |\n"
        "|||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          ||                                          |\n"
        "|||sociosqu ad litora                            |      **             **        **         ||                             ##           ||                                          |\n"
        "|||torquent per                                  |      **             **        **         ||                                          ||                                          |\n"
        "|||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             ||                                          |\n"
        "|||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            ||                                          |\n"
        "|||                                              |      ****           **        **         ||           #   #####  ###   #             ||                                          |\n"
        "|||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             ||                                          |\n"
        "|||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           ||          ###                             |\n"
        "|||pretium volutpat.                             |       **********                         ||                                          ||          #  #                            |\n"
        "|||Donec faucibus,                               |        ********                          ||                                          ||          #  # # ##   ###    ###          |\n"
        "|||ante sit amet                                 |                                          ||                                          ||          ###  ##    #   #  #  #          |\n"
        "|||luctus posuere,                               |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "|||mauris tellus                                 |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||          #    #      ###    ###          |\n"
        "|||                                          Bye,|*****   *      *  *      ******* ******  *||                                          ||                               #          |\n"
        "|||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          ||                             ##           |\n"
        "|||                                              |*    *  *      *  *      *           *   *||                                          ||                                          |\n"
        "|||                                              |*    *  *      *  *      *****      *    *||                                          ||           #    ###   ###   #             |\n"
        "|||                                              |****    *      *  *      *         *     *||                                          ||          ###  #   # #     ###            |\n"
        "|||                                              |*  *    *      *  *      *        *       ||                                          ||           #   #####  ###   #             |\n"
        "|||                                              |*   *   *      *  *      *       *       *||                                          ||           #   #         #  #             |\n"
        "|||                                              |*    *    *****   ****** ******* ******  *||                                          ||            ##  ###   ###    ##           |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||Lorem ipsum dolor sit amet,                                                                |                                          ||                                          |\n"
        "||consectetur adipiscing                                                                     |                                          ||                                          |\n"
        "||elit. Curabitur scelerisque                                                                |        ********                          ||                                          |\n"
        "||lorem vitae lectus cursus,                                                                 |       **********                         ||                                          |\n"
        "||vitae porta ante placerat. Class aptent taciti                                             |      **        **                        ||                                          |\n"
        "||sociosqu ad litora                                                                         |      **             **        **         ||                                          |\n"
        "||torquent per                                                                               |      **             **        **         ||                                          |\n"
        "||conubia nostra,                                                                            |      ***         ********  ********      ||                                          |\n"
        "||per inceptos himenaeos.                                                                    |      ****        ********  ********      ||                                          |\n"
        "||                                                                                           |      ****           **        **         ||                                          |\n"
        "||Donec tincidunt augue                                                                      |      ****           **        **         ||                                          |\n"
        "||sit amet metus                                                                             |      ****      **                        ||                                          |\n"
        "||pretium volutpat.                                                                          |       **********                         ||                                          |\n"
        "||Donec faucibus,                                                                            |        ********                          ||                                          |\n"
        "||ante sit amet                                                                              |                                          ||                                          |\n"
        "||luctus posuere,                                                                            |                                          ||                                          |\n"
        "||mauris tellus                                                                              |                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                                                                       Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                                                                Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                                                                           |****    *      *  *      *         *     *||                                          |\n"
        "||                                                                                           |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                                                                           |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                                                                           |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                                                             |                                          |\n"
        "|consectetur adipiscing                                                                                                                  |                                          |\n"
        "|elit. Curabitur scelerisque                                                                                                             |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                                                              |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                                                                          |      **        **                        |\n"
        "|sociosqu ad litora                                                                                                                      |      **             **        **         |\n"
        "|torquent per                                                                                                                            |      **             **        **         |\n"
        "|conubia nostra,                                                                                                                         |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                                                                 |      ****        ********  ********      |\n"
        "|                                                                                                                                        |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                                                                   |      ****           **        **         |\n"
        "|sit amet metus                                                                                                                          |      ****      **                        |\n"
        "|pretium volutpat.                                                                                                                       |       **********                         |\n"
        "|Donec faucibus,                                                                                                                         |        ********                          |\n"
        "|ante sit amet                                                                                                                           |                                          |\n"
        "|luctus posuere,                                                                                                                         |                                          |\n"
        "|mauris tellus                                                                                                                           |                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                                                                    Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                                                             Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *           *   *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                                                                        |****    *      *  *      *         *     *|\n"
        "|                                                                                                                                        |*  *    *      *  *      *        *       |\n"
        "|                                                                                                                                        |*   *   *      *  *      *       *       *|\n"
        "|                                                                                                                                        |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
