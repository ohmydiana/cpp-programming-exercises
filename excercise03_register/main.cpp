// Úkolem je realizovat třídu CRegister, která bude implementovat registr obyvatelstva.
//
// Uvažovaný registr je specifický tím, že chceme ukládat všechny změny (všechny nahlášené změny bydliště daného člověka). Dále, pro účely archivace chceme ukládat kopie registru.
//
// Úloha je směřována k procvičení pochopení mělké a hluboké kopie objektu. Má části povinné, dobrovolné a bonusové. Pro splnění povinné části úlohy postačuje vytvořit funkční třídu, která bude splňovat požadované rozhraní. Pro zvládnutí nepovinných a bonusových částí je potřeba rozmyslet ukládání dat tak, aby se při kopírování zbytečně neplýtvalo pamětí.
//
// Úloha má procvičit práci s kopírováním objektů. Z tohoto důvodu jsou v úloze potlačené části standardní C++ knihovny, zejména STL a datový typ C++ string.
//
// Požadovaná třída CRegister má následující rozhraní:
//
// implicitní konstruktor
// vytvoří prázdnou instanci registru,

// kopírující konstruktor
// vytvoří hlubokou kopii instance. Implementujte jej, pokud kompilátorem automaticky vytvořený kopírující konstruktor nevyhovuje (asi bude potřeba),

// destruktor
// uvolní prostředky alokované instancí.

// operátor =
// zkopíruje obsah jedné instance do druhé (hluboká kopie). Implementujte jej, pokud kompilátorem automaticky vytvořený operátor = nevyhoví,

// add(id,name,surname,date,street,city)
// metoda přidá zadaného člověka do databáze a nastaví u něj adresu pobytu. Parametrem je 6 řetězců s následujícím významem:
// id udává jednoznačný identifikátor člověka. Jedná se o řetězec, můžete předpokládat, že je vždy ve formátu "XXXXXX/YYYY",
// name udává jméno člověka. Jméno může být libovolné, libovolně dlouhé.
// surname udává příjmení člověka. Může být libovolné, libovolně dlouhé.
// date udává datum počátku platnosti místa pobytu. Jedná se o řetězec, můžete předpokládat, že je vždy ve formátu "YYYY-MM-DD".
// street udává adresu pobytu, řetězec, libovolný obsah a délka.
// city udává adresu místa pobytu, řetězec, libovolný obsah a délka.
// Metoda add vrací hodnotu true pro signalizaci úspěchu, false pro neúspěch (osoba se stejným id již byla zadaná).

// resettle(id,date,street,city)
// metoda přidá místo pobytu do záznamu daného člověka. Význam parametrů je stejný jako u metody add. Metoda vrací true pokud operace uspěje, false pokud selže (člověk se zadaným id neexistuje nebo daný člověk v ten samý den již dříve oznámil jinou adresu (tedy v jeden den se lze přestěhovat pouze jedenkrát). Pozor - informace o změně adresy nemusí být zpracovávány v pořadí vzrůstajícího času.

// print(stream,id)
// metoda zobrazí záznam pro člověka zadaného id do zadaného výstupního proudu stream. Metoda vrací true pro signalizaci úspěchu, false pro neúspěch (člověk se zadaným id neexistuje). Formát výstupu je uveden v ukázkách běhu, adresy trvalého bydliště jsou vypisované v rostoucím pořadí dle data stěhování.

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

struct AddressRecord
{
    char date[11];
    char* street;
    char* city;
};

struct Person
{
    char id[12];
    char* name;
    char* surname;
    AddressRecord* addresses;
    int addressSize;
    int addressCapacity;
};

class CRegister
{
public:
    // default constructor
    CRegister () : m_people(nullptr), m_sizePerson(0), m_capacityPerson(0) {}

    // copy constructor
    CRegister (const CRegister & src)
    {
        copy(src);
    }

    // destructor
    ~CRegister()
    {
        clear();
    }

    // operator =
    CRegister & operator = (const CRegister & src)
    {
        if (this == &src) return *this;

        clear();
        copy(src);
        return *this;
    }

    bool add (const char id[], const char name[], const char surname[], const char date[], const char street[], const char city[])
    {
        if (m_people == nullptr)
        {
            m_capacityPerson = 10;
            m_people = new Person[m_capacityPerson];
        }

        if (findPerson(id))
        {
            return false;
        }

        if (m_sizePerson >= m_capacityPerson)
        {
            m_capacityPerson += m_capacityPerson / 2 + 10;
            Person * newPeople = new Person [m_capacityPerson];

            for (int i = 0; i < m_sizePerson; i++)
            {
                newPeople[i] = m_people[i];
            }

            delete [] m_people;
            m_people = newPeople;
        }


        strcpy(m_people[m_sizePerson].id, id);
        m_people[m_sizePerson].name = new char[strlen(name) + 1];
        strcpy(m_people[m_sizePerson].name, name);
        m_people[m_sizePerson].surname = new char[strlen(surname) + 1];
        strcpy(m_people[m_sizePerson].surname, surname);

        m_people[m_sizePerson].addressSize = 0;
        m_people[m_sizePerson].addressCapacity = 10;
        m_people[m_sizePerson].addresses = new AddressRecord[m_people[m_sizePerson].addressCapacity];

        if (m_people[m_sizePerson].addressSize >= m_people[m_sizePerson].addressCapacity)
        {
            m_people[m_sizePerson].addressCapacity += m_people[m_sizePerson].addressCapacity / 2 + 10;
            AddressRecord* newAddresses = new AddressRecord[m_people[m_sizePerson].addressCapacity];

            for (int i = 0; i < m_people[m_sizePerson].addressSize; i++)
            {
                newAddresses[i] = m_people[m_sizePerson].addresses[i];
            }

            delete[] m_people[m_sizePerson].addresses;
            m_people[m_sizePerson].addresses = newAddresses;
        }

        int index = m_people[m_sizePerson].addressSize;
        strcpy(m_people[m_sizePerson].addresses[index].date, date);
        m_people[m_sizePerson].addresses[index].street = new char[strlen(street) + 1];
        strcpy(m_people[m_sizePerson].addresses[index].street, street);
        m_people[m_sizePerson].addresses[index].city = new char[strlen(city) + 1];
        strcpy(m_people[m_sizePerson].addresses[index].city, city);

        m_people[m_sizePerson].addressSize++;
        m_sizePerson++;

        return true;
    }

    bool resettle (const char id[], const char date[], const char street[], const char city[])
    {
        Person * person = findPerson(id);
        if (!person) return false;

        for (int i = 0; i < person->addressSize; i++)
        {
            if (strcmp(person->addresses[i].date, date) == 0)
            {
                return false;
            }
        }

        if (person->addressSize >= person->addressCapacity)
        {
            person->addressCapacity += person->addressCapacity / 2 + 10;
            AddressRecord* newAddresses = new AddressRecord[person->addressCapacity];

            for (int i = 0; i < person->addressSize; i++)
            {
                newAddresses[i] = person->addresses[i];
            }

            delete[] person->addresses;
            person->addresses = newAddresses;
        }

        int index = person->addressSize;
        strcpy(person->addresses[index].date, date);
        person->addresses[index].street = new char[strlen(street) + 1];
        strcpy(person->addresses[index].street, street);
        person->addresses[index].city = new char[strlen(city) + 1];
        strcpy(person->addresses[index].city, city);

        person->addressSize++;
        sortAddresses(*person);

        return true;
    }

    bool print (std::ostream & os, const char id[]) const
    {
        Person * person = findPerson(id);
        if (!person) return false;

        os << person->id << " " << person->name << " " << person->surname << std::endl;

        for (int i = 0; i < person->addressSize; i++)
        {
            os << person->addresses[i].date << " "
                << person->addresses[i].street << " "
                << person->addresses[i].city << std::endl;
        }

        return true;
    }

private:
    Person * m_people;
    int m_sizePerson;
    int m_capacityPerson;

    Person * findPerson (const char id[]) const
    {
        for (int i = 0; i < m_sizePerson; i++)
        {
            if (strcmp(m_people[i].id, id) == 0)
            {
                return &m_people[i];
            }
        }
        return nullptr;
    }

    void clear ()
    {
        for (int i = 0; i < m_sizePerson; i++)
        {
            delete [] m_people[i].addresses;
        }

        delete [] m_people;
        m_people = nullptr;
        m_sizePerson = 0;
        m_capacityPerson = 0;
    }

    void copy (const CRegister & src)
    {
        m_sizePerson = src.m_sizePerson;
        m_capacityPerson = src.m_capacityPerson;
        m_people = new Person[m_capacityPerson];

        for (int i = 0; i < m_sizePerson; i++)
        {
            m_people[i] = src.m_people[i];
            m_people[i].addresses = new AddressRecord[m_people[i].addressCapacity];

            for (int j = 0; j < m_people[i].addressSize; j++)
            {
                m_people[i].addresses[j] = src.m_people[i].addresses[j];
            }
        }
    }

    void swap (AddressRecord & a, AddressRecord & b)
    {
        AddressRecord temp = a;
        a = b;
        b = temp;
    }

    void sortAddresses (Person & person)
    {
        for (int i = 0; i < person.addressSize - 1; i++)
        {
            for (int j = 0; j < person.addressSize - i - 1; j++)
            {
                if (strcmp(person.addresses[j].date, person.addresses[j + 1].date) > 0)
                {
                    swap(person.addresses[j], person.addresses[j + 1]);
                }
            }
        }
    }
};

#ifndef __PROGTEST__
int main ()
{
  char   lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
  std::ostringstream oss;
  CRegister  a;
  assert ( a . add ( "123456/7890", "John", "Smith", "2000-01-01", "Main street", "Seattle" ) == true );
  assert ( a . add ( "987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street", "Sacramento" ) == true );
  assert ( a . resettle ( "123456/7890", "2003-05-12", "Elm street", "Atlanta" ) == true );
  assert ( a . resettle ( "123456/7890", "2002-12-05", "Sunset boulevard", "Los Angeles" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "123456/7890" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
)###" ) );
   CRegister b ( a );
   assert ( b . resettle ( "987654/3210", "2008-04-12", "Elm street", "Cinccinati" ) == true );
   assert ( a . resettle ( "987654/3210", "2007-02-11", "Elm street", "Indianapolis" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2007-02-11 Elm street Indianapolis
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  a = b;
  assert ( a . resettle ( "987654/3210", "2011-05-05", "Elm street", "Salt Lake City" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
2011-05-05 Elm street Salt Lake City
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  assert ( b . add ( "987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road", "London" ) == false );
  assert ( a . resettle ( "987654/3210", "2001-02-03", "Second street", "Milwaukee" ) == false );
  oss . str ( "" );
  assert ( a . print ( oss, "666666/6666" ) == false );

  CRegister  c;
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lName, "John", sizeof ( lName ) );
  strncpy ( lSurname, "Smith", sizeof ( lSurname ) );
  strncpy ( lDate, "2000-01-01", sizeof ( lDate) );
  strncpy ( lStreet, "Main street", sizeof ( lStreet ) );
  strncpy ( lCity, "Seattle", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "987654/3210", sizeof ( lID ) );
  strncpy ( lName, "Freddy", sizeof ( lName ) );
  strncpy ( lSurname, "Kruger", sizeof ( lSurname ) );
  strncpy ( lDate, "2001-02-03", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Sacramento", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2003-05-12", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Atlanta", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2002-12-05", sizeof ( lDate) );
  strncpy ( lStreet, "Sunset boulevard", sizeof ( lStreet ) );
  strncpy ( lCity, "Los Angeles", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  oss . str ( "" );
  assert ( c . print ( oss, "123456/7890" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
