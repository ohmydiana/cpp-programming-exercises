// Úkolem je realizovat třídu CPolynomial, která bude reprezentovat polynomy.
//
// Třída CPolynomial ukládá polynom pomocí koeficientů u jednotlivých jeho mocnin. Předpokládáme koeficienty v podobě desetinných čísel typu double.
// Pomocí rozhraní (většina rozhraní má podobu přetížených operátorů) dokáže tato třída s polynomy pracovat. Realizovaná třída musí splňovat následující rozhraní:
//
// konstruktor implicitní
// inicializuje objekt, který bude reprezentovat polynom o hodnotě 0.

// kopírující konstruktor
// bude implementován, pokud to vnitřní struktury Vaší třídy vyžadují.

// destruktor
// bude implementován, pokud to vnitřní struktury Vaší třídy vyžadují.

// přetížený operátor =
// bude umožňovat kopírování polynomů (pokud automaticky generovaný operátor = nevyhovuje).

// operátor <<
// bude umožňovat výstup objektu do C++ streamu. Výpis bude realizován v kompaktní podobě:
// členy jsou zobrazované směrem od nejvyšší mocniny,
// členy s nulovým koeficientem nebudou zobrazované,
// členy s koeficientem +1 či -1 nebudou zobrazovat číslo 1, zobrazí pouze příslušnou mocninu x,
// ve výpisu nejsou zbytečná znaménka - (tedy např. x - 9 nebo - x^2 + 4 je správné, ale x + (-9) je špatně),
// nulový polynom se zobrazí jako samostatné číslo 0.

// operátor *
// umožní vynásobit polynom desetinným číslem nebo dva polynomy,

// operátor *=
// umožní vynásobit polynom desetinným číslem nebo dva polynomy,

// operátory == a !=
// umožní porovnat polynomy na přesnou shodu (neuvažujeme epsilon toleranci),

// operátor []
// umožní nastavit/zjistit hodnotu koeficientu u zadané mocniny polynomu. Čtecí varianta musí fungovat i pro konstantní polynomy,

// operátor ()
// umožní vyhodnotit hodnotu polynomu pro zadanou hodnotu x (x je desetinné číslo),

// degree()
// metoda zjistí stupeň polynomu (např. x^3+4 má stupeň 3, 5 má stupeň 0, 0 má stupeň 0).
// přetypování na typ bool
// vrací true, pokud se nejedná o nulový polynom (má nějaký koeficient nenulový).

// operátor !
// vrací true, pokud se jedná o nulový polynom (všechny koeficienty jsou nulové).
// Nepovinný manipulátor poly_var:
//
// manipulátorem půjde změnit jméno proměnné ve výpisu polynomu. Výchozí jméno proměnné je x, použitím manipulátoru půjde nastavit na libovolný řetězec,
// implementace manipulátoru je nepovinná. Pokud se rozhodnete manipulátor nerealizovat, ponechte jej v podobě, která je součástí ukázkového běhu. Dodaný manipulátor nedělá nic, pouze zajistí, že program půjde zkompilovat (pokud deklaraci zcela odstraníte a nerealizujete manipulátor vlastní, program nepůjde zkompilovat),
// použití manipulátoru je zřejmé z dodané ukázky.
// Odevzdávejte zdrojový soubor, který obsahuje Vaší implementaci třídy CPolynomial a případnou realizaci manipulátoru. V odevzdávaném souboru nenechávejte vkládání hlavičkových souborů, Vaše testovací funkce a funkci main. Pokud v souboru chcete ponechat main nebo vkládání hlavičkových souborů, vložte je do bloku podmíněného překladu.
//
// V tomto příkladu není poskytnutý přesný předpis pro požadované rozhraní třídy. Z textového popisu, ukázky použití v příloze a ze znalostí o přetěžování operátorů byste měli být schopni toto rozhraní vymyslet.

#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <memory>
#include <compare>
#include <complex>
#endif /* __PROGTEST__ */

// keep this dummy version if you do not implement a real manipulator
std::ios_base & ( * poly_var ( const std::string & name ) ) ( std::ios_base & x )
{
  return [] ( std::ios_base & ios ) -> std::ios_base & { return ios; };
}

class CPolynomial
{
  public:
    CPolynomial (size_t size = 0) : polynom(size, 0) {}

    template <typename T>
    CPolynomial operator * (T scalar) const
    {
        CPolynomial result = *this;
        for (auto & coefficient : result.polynom)
        {
            coefficient *= static_cast<double>(scalar);
        }
        return result;
    }

    template <typename T>
    friend CPolynomial operator * (T scalar, const CPolynomial & polynom)
    {
        return polynom * scalar;
    }

    CPolynomial operator * (const CPolynomial & otherPolynom) const
    {
        size_t newSize = polynom.size() + otherPolynom.polynom.size() - 1;
        CPolynomial result (newSize);

        for (size_t i = 0; i < polynom.size(); i++)
        {
            for (size_t j = 0; j < otherPolynom.polynom.size(); j++)
            {
                result[i + j] += polynom[i] * otherPolynom[j];
            }
        }
        return result;
    }

    CPolynomial operator *= (const double scalar)
    {
        if (scalar == 0)
        {
            polynom.clear();
            return *this;
        }
        for (auto & coefficient : polynom)
        {
            coefficient *= scalar;
        }
        return *this;
    }

    CPolynomial operator *= (const CPolynomial & otherPolynom)
    {
        *this = *this * otherPolynom;
        return *this;
    }

    friend bool operator == (const CPolynomial & pol1, const CPolynomial & pol2)
    {
        if (pol1.polynom.size() != pol2.polynom.size())
        {
            bool hasOnlyZeros = true;
            for (const auto & coefficient : pol1.polynom)
            {
                if (coefficient != 0)
                {
                    hasOnlyZeros = false;
                }
            }

            for (const auto & coefficient : pol2.polynom)
            {
                if (coefficient != 0)
                {
                    hasOnlyZeros = false;
                }
            }

            if (hasOnlyZeros)
            {
                return true;
            }
            return false;
        }

        for (size_t i = 0; i < pol1.polynom.size(); i++)
        {
            if (pol1.polynom[i] != pol2.polynom[i])
            {
                return false;
            }
        }
        return true;
    }

    friend bool operator != (const CPolynomial & pol1, const CPolynomial & pol2)
    {
        return !(pol1 == pol2);
    }

    double & operator [] (const size_t index)
    {
        if (index >= polynom.size())
        {
            polynom.resize(index + 1, 0.0);
        }
        return polynom[index];
    }

    double operator [] (const size_t index) const
    {
        if (index >= polynom.size())
        {
            return 0.0;
        }
        return polynom[index];
    }

    double operator () (const double x) const
    {
        double result = 0;
        for (size_t i = 0; i < polynom.size(); i++)
        {
            result += polynom[i] * pow(x, i);
        }
        return result;
    }

    bool operator ! () const
    {
        for (const auto & coefficient : polynom)
        {
            if (coefficient != 0)
            {
                return false;
            }
        }
        return true;
    }

    operator bool () const
    {
        for (const auto & coefficient : polynom)
        {
            if (coefficient != 0)
            {
                return true;
            }
        }
        return false;
    }

    unsigned int degree () const
    {
        for (int i = polynom.size() - 1; i >= 0; i--)
        {
            if (polynom[i] != 0)
            {
                return i;
            }
        }
        return 0;
    }

    friend std::ostream & operator << (std::ostream & os, const CPolynomial & polynomial)
    {
        bool isFirst = true;
        bool onlyZeros = true;
        for (int i = polynomial.polynom.size() - 1; i >= 0; i--)
        {
            if (polynomial.polynom[i] < 0)
            {
                os << (isFirst ? "- " : " - ");
                onlyZeros = false;
            }
            else if (polynomial.polynom[i] > 0 && !isFirst)
            {
                os << " + ";
                onlyZeros = false;
            }

            if (polynomial.polynom[i] == 0)
            {
                continue;
            }

            if (i == 0)
            {
                os << (polynomial.polynom[i] < 0 ? polynomial.polynom[i] * -1 : polynomial.polynom[i]);
                if (polynomial.polynom[i] != 0)
                {
                    onlyZeros = false;
                }
            }
            else
            {
                if (polynomial.polynom[i] == 1 || polynomial.polynom[i] == -1)
                {
                    os << "x^" << i;
                }
                else
                {
                    os << (polynomial.polynom[i] < 0 ? polynomial.polynom[i] * -1 : polynomial.polynom[i]) << "*x^" << i;
                }
                onlyZeros = false;
            }
            isFirst = false;
        }

        if (onlyZeros)
        {
            os << "0";
        }
        return os;
    }

  private:
    std::vector<double> polynom;
};

#ifndef __PROGTEST__

bool smallDiff ( double a, double b )
{
    return std::fabs(a - b) < DBL_EPSILON;
}

bool dumpMatch ( const CPolynomial & x, const std::vector<double> & ref )
{
    for (size_t i = 0; i < ref.size(); i++)
    {
        if (!smallDiff(x[i], ref[i]))
        {
            return false;
        }
    }
    return true;
}

int main ()
{
  CPolynomial a, b, c;
  std::ostringstream out, tmp;

  a[0] = -10;
  a[1] = 3.5;
  a[3] = 1;

  assert ( smallDiff ( a ( 2 ), 5 ) );
  out . str ("");
  out << a;
  assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
  c = a * -2;
  assert ( c . degree () == 3
           && dumpMatch ( c, std::vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );


  out . str ("");
  out << c;
  assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );

  out . str ("");
  out << b;
  assert ( out . str () == "0" );
  b[5] = -1;
  b[2] = 3;
  out . str ("");
  out << b;
  assert ( out . str () == "- x^5 + 3*x^2" );

  c = a * b;
  assert ( c . degree () == 8
           && dumpMatch ( c, std::vector<double>{ -0.0, -0.0, -30.0, 10.5, -0.0, 13.0, -3.5, 0.0, -1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- x^8 - 3.5*x^6 + 13*x^5 + 10.5*x^3 - 30*x^2" );
  a *= 5;
  assert ( a . degree () == 3
           && dumpMatch ( a, std::vector<double>{ -50.0, 17.5, 0.0, 5.0 } ) );

  a *= b;
  assert ( a . degree () == 8
           && dumpMatch ( a, std::vector<double>{ 0.0, 0.0, -150.0, 52.5, -0.0, 65.0, -17.5, -0.0, -5.0 } ) );
  assert ( a != b );
  b[5] = 0;
  assert ( static_cast<bool> ( b ) );
  assert ( ! ! b );
  b[2] = 0;
  assert ( !(a == b) );
  a *= 0;
  assert ( a . degree () == 0
           && dumpMatch ( a, std::vector<double>{ 0.0 } ) );

  assert ( a == b );
  assert ( ! static_cast<bool> ( b ) );
  assert ( ! b );

  a[0] = 10;
  a[1] = -3.5;
  a *= b;
  assert(a . degree() == 0 && dumpMatch ( a, std::vector<double>{ 0.0 } ) );
  out . str ("");
  out << a;
  assert ( out . str () == "0" );


  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
