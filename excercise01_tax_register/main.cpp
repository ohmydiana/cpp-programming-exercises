// Úkolem je realizovat třídu CTaxRegister, která bude implementovat
//
// Pro plánované důslednější potírání daňových úniků je potřeba vybudovat databázi všech obyvatel, ve které budou archivované jejích příjmy a výdaje.
// Předpokládáme, že v databázi je zaveden každý občan v okamžiku jeho narození. Od té doby se mu počítají všechny příjmy a všechny výdaje.
// Záznam je z databáze odstraněn v okamžiku jeho úmrtí. Občan je identifikován svým jménem, adresou a číslem účtu. Číslo účtu je unikátní přes celou databázi.
// Jména a adresy se mohou opakovat, ale dvojice (jméno, adresa) je opět v databázi unikátní. Tedy v databázi může být mnoho jmen Jan Novak, mnoho lidí může mít adresu Hlavni 60, ale Jan Novak bydlící na adrese Hlavni 60 může být v databázi pouze jeden.
//
// Veřejné rozhraní třídy CTaxRegister obsahuje následující:
//
// Konstruktor bez parametrů. Tento konstruktor inicializuje instanci třídy tak, že vzniklá instance je zatím prázdná (neobsahuje žádné záznamy).
// Destruktor. Uvolňuje prostředky, které instance alokovala.
// Metoda birth(name, addr, acct) přidá do existující databáze další záznam. Parametry name a addr reprezentují jméno a adresu občana, parametr acct udává číslo účtu. Metoda vrací hodnotu true, pokud byl záznam přidán, nebo hodnotu false, pokud přidán nebyl (protože již v databázi existoval záznam se stejným jménem a adresou, nebo záznam se stejným číslem účtu).
// Metoda death (name, addr) odstraní záznam z databáze. Parametrem je jednoznačná identifikace pomocí jména a adresy. Pokud byl záznam skutečně odstraněn, vrátí metoda hodnotu true. Pokud záznam neodstraní (protože neexistoval občan s touto identifikací), vrátí metoda hodnotu false.
// Metody income zaznamenají na účet pro daného občana příjem ve výši amount. Varianty jsou dvě - občan je buď identifikován svým jménem a adresou, nebo identifikátorem jeho účtu. Pokud metoda uspěje, vrací true, pro neúspěch vrací false (neexistující občan/účet).
// Metody expense zaznamenají na účet daného občana výdaj ve výši amount. Varianty jsou dvě - občan je buď identifikován svým jménem a adresou, nebo identifikátorem jeho účtu. Pokud metoda uspěje, vrací true, pro neúspěch vrací false (neexistující občan/účet).
// Metoda audit ( name, addr, acct, sumIncome, sumExpense ) vyhledá pro občana se zadaným jménem a adresou a všechny dostupné informace (účet, součet příjmů a součet výdajů). Nalezené informace uloží do zadaných výstupních parametrů. Metoda vrátí true pro úspěch, false pro selhání (neexistující dvojice jméno + adresa).
// Metoda listByName vrátí instanci třídy CIterator. Vrácený objekt má rozhraní podle seznamu níže. Jeho pomocí půjde procházet záznamy v registru od počátku do konce. Při načítání budou záznamy procházené v pořadí zvětšujícího se jména (seřazená podle jména), pro stejná jména budou nejdříve vracené záznamy seřazené podle adresy.
// Veřejné rozhraní třídy CIterator obsahuje následující:
//
// Metoda atEnd vrací hodnotu true pokud bylo dosaženo konce registru (tedy iterátor již neodkazuje na platný záznam) nebo false pokud iterátor ještě nedosáhl konce registru (odkazuje na platný záznam).
// Metoda next posune iterátor na další záznam.
// Metoda name zjistí jméno osoby na aktuální pozici iterátoru.
// Metoda addr zjistí adresu osoby na aktuální pozici iterátoru.
// Metoda account zjistí identifikátor účtu osoby na aktuální pozici iterátoru.
// Můžete se spolehnout, že během používání instance iterátoru nejsou prováděné žádné změny v registru, ze kterého iterátor vznikl (zejména není volaná metoda CTaxRegister::birth ani CTaxRegister::death).

// Odevzdávejte soubor, který obsahuje implementované třídy CTaxRegister a CIterator. Třídy musí splňovat veřejné rozhraní podle ukázky - pokud Vámi odevzdané řešení nebude obsahovat popsané rozhraní, dojde k chybě při kompilaci.
// Do tříd si ale můžete doplnit další metody (veřejné nebo i privátní) a členské proměnné. Odevzdávaný soubor musí obsahovat jak deklaraci třídy (popis rozhraní) tak i definice metod, konstruktoru a destruktoru.
// Je jedno, zda jsou metody implementované inline nebo odděleně. Odevzdávaný soubor nesmí obsahovat vkládání hlavičkových souborů a funkci main (funkce main a vkládání hlavičkových souborů může zůstat, ale pouze obalené direktivami podmíněného překladu jako v přiloženém archivu).
//
// Třída je testovaná v omezeném prostředí, kde je limitovaná dostupná paměť (dostačuje k uložení seznamu) a je omezena dobou běhu. Implementovaná třída CTaxRegister se nemusí zabývat kopírujícím konstruktorem ani přetěžováním operátoru =. V této úloze ProgTest neprovádí testy této funkčnosti.
//
// Implementace třídy musí být efektivní z hlediska nároků na čas i nároků na paměť. Jednoduché lineární řešení nestačí (pro testovací data vyžaduje čas přes 5 minut). Předpokládejte, že narození a úmrtí jsou řádově méně časté než ostatní operace.
// Proto vyhovuje řešení s polem seřazených hodnot, kde vyhledávání probíhá půlením intervalu s logaritmickou složitostí. Méně časté operace vkládání a výmaz je pak doporučeno implementovat postupem, který v logaritmickém čase najde pozici vkládaného/odebíraného prvku a pak v lineárním čase posune obsah pole.
//
// Pro uložení hodnot alokujte pole dynamicky případně použijte STL. Pozor, pokud budete pole alokovat ve vlastní režii, zvolte počáteční velikost malou (např. tisíc prvků) a velikost zvětšujte/zmenšujte podle potřeby. Při zaplnění pole není vhodné alokovat nové pole větší pouze o jednu hodnotu, takový postup má obrovskou režii na kopírování obsahu.
// Je rozumné pole rozšiřovat s krokem řádově tisíců prvků, nebo geometrickou řadou s kvocientem ~1.5 až 2.
//
// Pokud budete používat STL, nemusíte se starat o problémy s alokací. Pozor - v STL máte úmyslně zakázané kontejnery map.

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */

struct Person {
    std::pair<std::string, std::string> m_nameAddress;
    std::string m_account;
    int m_sumIncome;
    int m_sumExpense;


    Person(const std::string & name, const std::string & addr, const std::string & acc, const int & sumIncome, const int & sumExpense)
        : m_nameAddress(name, addr), m_account(acc), m_sumIncome(sumIncome), m_sumExpense(sumExpense) {}
};

class CIterator
{
  public:
    CIterator (const std::vector<Person> & people) : m_people(people), m_index(0) {}

    bool atEnd () const
    {
        return m_index >= m_people.size();
    }

    void next ()
    {
        if (!atEnd())
        {
            m_index++;
        }
    }

    const std::string & name () const
    {
        return m_people[m_index].m_nameAddress.first;
    }

    const std::string & addr () const
    {
        return m_people[m_index].m_nameAddress.second;
    }

    const std::string & account () const
    {
        return m_people[m_index].m_account;
    }

  private:
    const std::vector<Person> & m_people;
    size_t m_index;
};

class CTaxRegister
{
  public:

    bool birth(const std::string &name, const std::string &addr, const std::string &account) {
        Person p = Person(name, addr, account, 0, 0);

        if (std::binary_search(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr)
            || std::binary_search(personByAccount.begin(), personByAccount.end(), p, cmpByAccount)) {
            return false;
        }

        auto it1 = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr);
        personByNameAddress.insert(it1, p);

        auto it2 = std::lower_bound(personByAccount.begin(), personByAccount.end(), p, cmpByAccount);
        personByAccount.insert(it2, p);
        return true;
    }

    bool death ( const std::string & name, const std::string & addr )
    {
        Person p1 = Person (name, addr, "", 0, 0);

        auto it1 = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), p1, cmpByNameAddr);
        if (it1 == personByNameAddress.end() || it1->m_nameAddress != std::make_pair(name, addr))
        {
            return false;
        }
        std::string account1 = it1->m_account;
        personByNameAddress.erase(it1);

        Person p2 = Person ("", "", account1, 0, 0);

        auto it2 = std::lower_bound(personByAccount.begin(), personByAccount.end(), p2, cmpByAccount);
        if (it2 == personByAccount.end() || it2->m_nameAddress != std::make_pair(name, addr))
        {
            return false;
        }
        std::string account2 = it2->m_account;
        personByAccount.erase(it2);

        return true;
    }

    bool income ( const std::string & account, int amount )
    {
        Person p = Person("", "", account, 0, 0);

        if (std::binary_search(personByAccount.begin(), personByAccount.end(), p, cmpByAccount)) {
            auto it = std::lower_bound(personByAccount.begin(), personByAccount.end(), p, cmpByAccount);
            int index = std::distance(personByAccount.begin(), it);
            Person tmp = Person(personByAccount[index].m_nameAddress.first, personByAccount[index].m_nameAddress.second, "", 0,0);

            auto it2 = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), tmp, cmpByNameAddr);
            int index2 = std::distance(personByNameAddress.begin(), it2);
            personByNameAddress[index2].m_sumIncome += amount;

            return true;
        }

        return false;
    }

    bool income ( const std::string & name, const std::string & addr, int amount )
    {
        Person p = Person(name, addr, "", 0, 0);

        if (std::binary_search(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr)) {
            auto it = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr);
            int index = std::distance(personByNameAddress.begin(), it);
            personByNameAddress[index].m_sumIncome += amount;

            return true;
        }

        return false;
    }

    bool expense ( const std::string & account, int amount )
    {
        Person p = Person("", "", account, 0, 0);

        if (std::binary_search(personByAccount.begin(), personByAccount.end(), p, cmpByAccount)) {
            auto it = std::lower_bound(personByAccount.begin(), personByAccount.end(), p, cmpByAccount);
            int index = std::distance(personByAccount.begin(), it);
            Person tmp = Person(personByAccount[index].m_nameAddress.first, personByAccount[index].m_nameAddress.second, "", 0,0);

            auto it2 = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), tmp, cmpByNameAddr);
            int index2 = std::distance(personByNameAddress.begin(), it2);
            personByNameAddress[index2].m_sumExpense += amount;

            return true;
        }

        return false;
    }

    bool expense ( const std::string & name, const std::string & addr, int amount )
    {
        Person p = Person(name, addr, "", 0, 0);

        if (std::binary_search(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr)) {
            auto it = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr);
            int index = std::distance(personByNameAddress.begin(), it);
            personByNameAddress[index].m_sumExpense += amount;

            return true;
        }

        return false;
    }

    bool audit ( const std::string & name, const std::string & addr, std::string & account, int & sumIncome, int & sumExpense ) const
    {
        Person p = Person(name, addr, account, sumIncome, sumExpense);

        if (std::binary_search(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr)) {
            auto it = std::lower_bound(personByNameAddress.begin(), personByNameAddress.end(), p, cmpByNameAddr);
            int index = std::distance(personByNameAddress.begin(), it);

            account = personByNameAddress[index].m_account;
            sumIncome = personByNameAddress[index].m_sumIncome;
            sumExpense = personByNameAddress[index].m_sumExpense;

            return true;
        }

        return false;
    }

    CIterator listByName () const
    {
        return CIterator (personByNameAddress);
    }

    void printPeople() const
    {
        for (const auto &p : personByNameAddress) {
            std::cout << "Name: " << p.m_nameAddress.first
                      << ", Address: " << p.m_nameAddress.second
                      << ", Account: " << p.m_account
                        <<  ", Income: " << p.m_sumIncome << std::endl;

        }
    }

  private:
    std::vector<Person> personByNameAddress;
    std::vector<Person> personByAccount;

    static bool cmpByNameAddr(const Person & p1, const Person & p2) {
        return std::tie(p1.m_nameAddress.first, p1.m_nameAddress.second) < std::tie(p2.m_nameAddress.first, p2.m_nameAddress.second);
    }

    static bool cmpByAccount(const Person & p1, const Person & p2) {
        return p1.m_account < p2.m_account;
    }

};

#ifndef __PROGTEST__
int main ()
{
  std::string acct;
  int    sumIncome, sumExpense;
  CTaxRegister b0;
  assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
    b0 . printPeople();
  assert ( b0 . income ( "Xuj5#94", 1000 ) );
  assert ( b0 . income ( "634oddT", 2000 ) );
  assert ( b0 . income ( "123/456/789", 3000 ) );
  assert ( b0 . income ( "634oddT", 4000 ) );
  assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
    b0 . printPeople();
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b0 . expense ( "Xuj5#94", 1300 ) );
  assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );
  CIterator it = b0 . listByName ();
  assert ( ! it . atEnd ()
           && it . name () == "Jane Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "Xuj5#94" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Main Street 17"
           && it . account () == "Z343rwZ" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Oak Road 23"
           && it . account () == "123/456/789" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "Peter Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "634oddT" );
  it . next ();
  assert ( it . atEnd () );

  assert ( b0 . death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b1;
  assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b1 . income ( "634oddT", 4000 ) );
  assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
